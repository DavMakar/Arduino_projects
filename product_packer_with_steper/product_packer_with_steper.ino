#include <Keypad.h>
#include <Wire.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>

#define stopPin 13
#define startPin 10
#define stepPin1 12
#define directionPin1 11
#define ardukPin 3
#define stepPin2 6
#define directionPin2 5

LiquidCrystal_I2C lcd(0x27, 20, 4);

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {A1, 2, 8, A3};
byte colPins[COLS] = {A2, A0, 7};

int vals[15] = {3, 4, 5, 4, 8, 5, 5, 3, 3, 2, 1, 2, 5, 6, 7};

const char *item_1[] = {"1.Saqar", "2.Sev Pxpex", "3.Surch", "4.Karmir Pxpex", "5.Ax"};
const char *item_2[] = {"Motor(sm)", "Klapan(sec)"};

byte index = 0;
int nav = 0;

byte navig = 1;
int num;

int helper = 0;
//byte dir =0;

int stopState = 0;
int stoper;

int startState = 1;
byte startIndex = 0;

int i = 0, j = 18;
int n;

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


int getKeypadIntegerMulti()
{
  int value = 0;                                // the number accumulator
  long keyvalue;                                     // the key pressed at current moment
  int isnum;
  do
  {
    keyvalue = keypad.getKey() ;
    isnum = (keyvalue >= '0' && keyvalue <= '9');         // is it a digit?
    if (isnum)
    {
      value = value * 10 + keyvalue - '0';               // accumulate the input number
    }

  } while (isnum || !keyvalue);                          // until not a digit or while no key pressed
  
  return value;
}

//function for stop button

void checkStop(int p){
    stopState = digitalRead(stopPin);
        if (stopState == LOW) {
          startIndex = 0;
          lcd.setCursor(2, 2);
          lcd.print("STOP");
          if(p ==0){
            digitalWrite(stepPin1, LOW);
          }else{
            digitalWrite(stepPin2, LOW);
          }
          stoper=1;
          menu_0(nav);
        }
}

// steper 1
 
void rotate(byte dir){
    digitalWrite(directionPin1, dir);
    for (int n = 0; n < vals[helper*3]*500 ; n++) {
        digitalWrite(stepPin1, HIGH);

        delay(1);
        digitalWrite(stepPin1, LOW);
        delayMicroseconds(15);
    }
    //dir = (dir ==0) ? 1 : 0;  //change step_motor direction
}

//steper 2
 
void steper2(int p){
    digitalWrite(directionPin2, p);
    digitalWrite(stepPin2, HIGH);
    delay(64);
    digitalWrite(stepPin2, LOW);
}

// start menu

void menu_0(int p) { // p = 0 ; 4  Eeprom saved product index from item_1[p] 
  lcd.noBlink();
  lcd.clear();
  lcd.print("1.start  ");
  lcd.print(item_1[p]);  
  lcd.setCursor(0, 1);
  lcd.print("2.select");
  lcd.setCursor(0, 2);
  lcd.print("3.setting");
  lcd.setCursor(0, 3);
  lcd.print("4.motorConfig");
  index = 0;
  stoper=0;
}

//product select menu

void menu_1() {
  lcd.noBlink();
  index = 1;
  // flag = 0;
  lcd.clear();
  for (int k = 0; k < 4; k++) {
    lcd.setCursor(0, k);
    lcd.print(item_1[k]);
  }
  lcd.setCursor(14, 0);
  lcd.print(item_1[4]);
}

//working menu

void menu_3(int p) {
  index = 3;
  helper = p;
  lcd.clear();
  lcd.setCursor(2, 1);
  lcd.print(item_1[p]);
  lcd.setCursor(2, 3);
  lcd.print("BEGINN......");
}

//product settings menu

void menu_2(int p) {
  index = 2;
  lcd.clear();
  lcd.blink();
  lcd.print(item_1[p]);
  lcd.setCursor(j, 0);
  lcd.print(vals[p * 3]);
  for (int k = 1; k < 3; k++) {
    lcd.setCursor(0, k);
    lcd.print(item_2[k - 1]);
    lcd.setCursor(j, k);
    lcd.print(vals[p * 3 + k]);
  }
  num = p * 3 + 3;
}


// steper 2 configuration menu

void menu_4() {
  lcd.noBlink();
  lcd.clear();
  lcd.print("stepMotor");
  lcd.setCursor(0, 1);
  lcd.print("4 araj");
  lcd.setCursor(0, 2);
  lcd.print("6 het");
  lcd.setCursor(0, 3);
  lcd.print("0 ok");
  index = 4;
}

void moving(char key) {
  
  switch (key) {
    // Up
    case '2' :
      if (i > 0) {
        i--;
      }
      break;
    case '8'  :
      if (i < 2) {
        i++;
      }
      else if (i == 2) {
        i = 0;
      }
      break;
    case '#':
      navig = 0;
      break;
    case '*':
      menu_0(nav);
      break;
  }
}

void setup() { 
  //EEPROM.put(5,vals);
  EEPROM.get(5, vals);
  EEPROM.get(3, nav);
  lcd.init();
  lcd.backlight();
  menu_0(nav);
  pinMode(directionPin1, OUTPUT);
  pinMode(stepPin1, OUTPUT);
  pinMode(directionPin2, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(startPin, INPUT_PULLUP);
  pinMode(stopPin, INPUT);
  pinMode(ardukPin, OUTPUT);
  digitalWrite(ardukPin, HIGH);
}

void loop() {
  lcd.setCursor(j, i);
  char key = keypad.getKey();
  
  if (index == 0) {
    switch (key) {
      case '1':
        menu_3(nav);
        break;
      case '2':
        menu_1();
        break;
      case '3':
        menu_2(nav);
        break;
      case '4':
        menu_4();
        break;
    }
  }
  else if (index == 1) {
    switch (key) {
      case '1':
        nav = 0;
        menu_0(nav);
        EEPROM.put(3, nav);
        break;
      case '2':
        nav = 1;
        menu_0(nav);
        EEPROM.put(3, nav);
        break;
      case '3':
        nav = 2;
        menu_0(nav);
        EEPROM.put(3, nav);
        break;
      case '4':
        nav = 3;
        menu_0(nav);
        EEPROM.put(3, nav);
        break;
      case '5':
        nav = 4;
        menu_0(nav);
        EEPROM.put(3, nav);
        break;
    }
  }
  else if (index == 2) {
    if ((key >= '0' && key <= '8')|| key =='#' || key == '*'){
      moving(key);
    }
    if (navig == 0){
      lcd.print("   ");
      lcd.setCursor(j, i);
      vals[num - 3 + i] = getKeypadIntegerMulti();
      lcd.print(vals[num - 3 + i]);
      navig = 1;
      EEPROM.put(5, vals);
    }
  }
  else if (index == 3) {
    startState = digitalRead(startPin);
    if (startState == LOW) {
      lcd.setCursor(2, 0);
      lcd.print("START");
      delay(150);
      startIndex = 1;
    }
    
    if (startIndex == 1){
      digitalWrite(ardukPin, LOW);
      delay(vals[helper*3+2]*512);
      digitalWrite(ardukPin, HIGH);
      delay(50);
      rotate(0); //dir 0 
      rotate(1); // dir 1
      
      lcd.setCursor(10, 0);
      lcd.print(" ");
      lcd.print("weigthed");
      delay(50);
      digitalWrite(directionPin2, LOW);
      for (n = 0; n < vals[helper * 3 + 1] * 10; n++) {
        digitalWrite(stepPin2, HIGH);
        checkStop(1);
        if(stoper==1){
          break;
        }
        delay(15);
        digitalWrite(stepPin2, LOW);
        
      }
      for (int p = 10; p < 20; p++) {
        lcd.setCursor(p, 0);
        lcd.print(" ");
      }
      delay(150);
    }
  }else if(index==4){
    switch (key) {
      case '4':
        steper2(0);
        break;
      case '6':
        steper2(1);
        break;
      case '0':
        menu_0(nav);
        break;
    }
  }
}
