#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Wire.h>
#include <EEPROM.h>
LiquidCrystal lcd(8,9,4,5,6,7);

const int startPin=10;
const int buttonPin = 11;
const int detectPin=1;
const int mkrat_pin=3;
const int motor_pin=13;

//1 impulsy 10.66 mm = 1.066 sm
//100 * 1.066

const int conf=89;
int count=89;

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {A1, 2, A5, A3};
byte colPins[COLS] = {A2, A0, A4};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

int vals[3]={1,2,2};
int buttonPushCounter = 0;
int n = 0;
byte change=0;
byte navig = 1;
int buttonState = 0;
byte flag=0;
int detectState = 1;
int startIndex = 1;
int startState = 1;
int lastButtonState = 1; 
byte index = 0;
int x = 0, y = 16;
int k;

int getKeypadIntegerMulti()
{
  int value = 0;                                // the number accumulator
  long keyvalue;                                     // the key pressed at current moment
  int isnum;
  do
  {
    keyvalue = keypad.getKey();
    isnum = (keyvalue >= '0' && keyvalue <= '9');         // is it a digit?
    if (isnum)
    {
  
      value = value * 10 + keyvalue - '0';              
    }

  } while (isnum || !keyvalue);                          // until not a digit or while no key pressed
  //
  return value;
}

void buttonCount(){
  buttonState = digitalRead(buttonPin);
  // compare the buttonState to its previous state
  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {
      // if the current state is LOW counter++:
      buttonPushCounter++;
      lcd.setCursor(2,2);
      lcd.print(buttonPushCounter);
    }
    // Delay a little bit to avoid bouncing
    delay(10);
  }
  // save the current state as the last state, for next time through the loop
  lastButtonState = buttonState; 
}

void moving(char key) {
  switch (key) {
    // Up
    case '2' :
      if (x > 0) {
        x--;
      }
      break;
    case '8':
      if (x < 2) {
        x++;
      }
      else if (x == 2) {
        x = 0;
      }
      break;
    case '#':
      navig = 0;
      change=1;
      break;
    case '0':
      menu_0();
      if(change==1){
        EEPROM.put(1, vals);
        if(vals[0]>5){
          count= conf*vals[0]-(vals[0]/5);
        }
        else{
          count= conf*vals[0];
        }
        EEPROM.put(8,count);
        change=0;
      }
      break;
  }
}

void menu_0(){
  lcd.noBlink();
  //
  lcd.clear();
  lcd.print("meter");
  lcd.setCursor(7,0);
  lcd.print(vals[0]);
  lcd.setCursor(12,0);
  lcd.print("hat");
  lcd.setCursor(17,0);
  lcd.print(vals[1]);
  lcd.setCursor(0,1);
  lcd.print("1.start");
  lcd.setCursor(0,2);
  lcd.print("2.setting");
  lcd.setCursor(0,3);
  lcd.print("3.patatel");
  index=0;
  
  if(flag==1){
    if(vals[0]>5){
      count= conf*vals[0]-(vals[0]/5);
    }
    else{
      count= conf*vals[0];
    }
    EEPROM.put(8,count);
    flag=0;
  }
}

void settings(){
  lcd.clear();
  lcd.blink();
  lcd.print("meter");
  lcd.setCursor(y,0);
  lcd.print(vals[0]);
  lcd.setCursor(0,1);
  lcd.print("hat");
  lcd.setCursor(y,1);
  lcd.print(vals[1]);
  lcd.setCursor(0,2);
  lcd.print("mkrat(sec)");
  lcd.setCursor(y,2);
  lcd.print(vals[2]);
}

void setup(){
  
 // EEPROM.put(1, vals);
  //EEPROM.put(8, count);
  EEPROM.get(1, vals);
  EEPROM.get(8,count);
  lcd.begin(20,4);
  lcd.clear();
  menu_0();
  
  pinMode(startPin, INPUT_PULLUP);
  pinMode(buttonPin, INPUT);
  pinMode(detectPin, INPUT_PULLUP);
  pinMode(mkrat_pin, OUTPUT);
  pinMode(motor_pin, OUTPUT);
  digitalWrite(mkrat_pin, HIGH);
  digitalWrite(motor_pin, HIGH);
}

void loop() {
  lcd.setCursor(y, x);
  char key = keypad.getKey();
  if (index == 0) {
    switch (key) {
      case '1':
        lcd.clear();
        index = 2;
        break;
      case '2':
        index=1;
        settings();
        break;
      case '3':
        lcd.clear();
        index=3;
        break;
   }
  }else if (index == 1) {
    if ((key >= '0' && key <= '8')||key=='#') {
      moving(key);
    }
    if (navig == 0) {
      lcd.print("   ");
      lcd.setCursor(y,x);
      vals[x] = getKeypadIntegerMulti();
      lcd.print(vals[x]);
      //Serial.println(num-3+i);
      navig = 1;
    }
  }
  else if(index ==2){
    startState = digitalRead(startPin);
    lcd.setCursor(2,0);
    lcd.print("Press Start");
    if(startState == LOW){
      startIndex=0;
    }
    if(startIndex==0){
      //digitalWrite(ankap_pin, LOW);
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("Counting...");
      while(n<vals[1]){
        digitalWrite(motor_pin, LOW);
        if(buttonPushCounter<count){
          lcd.setCursor(2,2);
          lcd.print(buttonPushCounter);
          lcd.setCursor(2,0);
          lcd.print("Counting...");
          buttonCount();
        }
        else if(buttonPushCounter == count){
          digitalWrite(motor_pin, HIGH);
          n++;  
          lcd.setCursor(14,0);
          lcd.print("Done");
          buttonPushCounter=0;
          
          delay(1024);
          digitalWrite(mkrat_pin, LOW);
          delay(vals[2]*512);
          digitalWrite(mkrat_pin, HIGH);

          for(;;){
            detectState = digitalRead(detectPin);
            if(detectState==LOW){
              break;
            } 
          }
          lcd.clear();
          delay(300);      
        }
      }
      if(n==vals[1]){
          digitalWrite(motor_pin, HIGH);
          lcd.clear();
          lcd.setCursor(8,0);
          lcd.print("VERJ");
          startIndex=1;
          n=0;
          delay(3000);
          menu_0();
      }
    }
  }
  else if(index==3){
      lcd.setCursor(2,0);
      lcd.print("Counting...");
      lcd.setCursor(2,2);
      lcd.print(n);        
    while(n<vals[1]){
      if(buttonPushCounter < count){
          digitalWrite(motor_pin, LOW);
          
          buttonCount();
      }
      else if(buttonPushCounter == count){
        buttonPushCounter=0;
        n++;
        lcd.setCursor(2,2);
        lcd.print("   ");
        lcd.setCursor(2,2);
        lcd.print(n);
      }
   }   
   if(n==vals[1]){
          digitalWrite(motor_pin, HIGH);
          lcd.clear();
          lcd.setCursor(8,0);          
          lcd.print("VERJ");
          n=0;
          delay(3000);
          menu_0();
   }
  }
}
