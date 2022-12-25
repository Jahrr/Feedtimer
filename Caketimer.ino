/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe
 modified 7 Nov 2016
 by Arturo Guadalupi

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystalHelloWorld

*/

// include the library code:
#include <LiquidCrystal.h>
#include <LiquidMenu.h>
#include <ArxContainer.h>
#include "Timer.h"
#include "Button.h"

//Initialize pins
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int upButtonPin = 30;
const int middleButtonPin = 34;
const int downButtonPin = 38;


struct Selector{
  char selector = '>';
  int row = 0;
  int column = 0;
};

struct TimerData{
    TimerData(String title, int pos, int length) :  position{pos}, duration{length}{
      for(int i = 0; i < 8; i++){
        name[i] = title[i];
      }
    }
    char name[8];
    int position;
    int duration;  
};

enum mode{
MENU, TIMER
  
};


//variables
Timer timer;


unsigned int TimerEntries = 4;
unsigned int QueryPosition = 0;
unsigned int mode = MENU;

TimerData td1("Timer 1", 0, 30);
TimerData td2("Timer 2", 1, 60);
TimerData td3("Timer 3", 2, 90);
TimerData td4("Timer 4", 3, 120);

TimerData datas[4]{td1, td2, td3, td4};


LiquidLine line1(2,datas[0].position, datas[0].name);
LiquidLine line2(2,datas[1].position, datas[1].name);
LiquidLine line3(2,datas[2].position, datas[2].name);
LiquidLine line4(2,datas[3].position, datas[3].name);

//Maybe we can use this by allocating enough that we never need to resize it
//create similar large arrays for screens
//LiquidLine llines[5](5, line1);

LiquidLine lines[4]{line1, line2, line3, line4};

//LiquidLine selectorLine(0,0,">");

LiquidScreen screen1;
LiquidScreen screen2;
LiquidScreen screen3;

LiquidScreen screens[3]{screen1, screen2, screen3};
bool arrowIsOnBottom = false;

LiquidMenu menu(lcd, screens[0], screens[1], screens[2]);

Selector selector;


Button upButton(upButtonPin);
Button middleButton(middleButtonPin);
Button downButton(downButtonPin);

void setup() {
  // set up the LCD's number of columns and rows:
  
  Serial.begin(9600);
  for(int i = 0; i < TimerEntries - 1; i++){
    screens[i].add_line(lines[i]);
    screens[i].add_line(lines[i+1]);
  }


  lcd.begin(16, 4);
  menu.update();
  lcd.setCursor(0, 0);
  lcd.print(selector.selector);
  lcd.setCursor(0, 0);
}

void runMenu(){
  lcd.setCursor(selector.column, selector.row);
  lcd.print(selector.selector);
  if(upButton.isPressedOnce() && QueryPosition > 0){
    selector.row--;
    QueryPosition--;
    if(arrowIsOnBottom)arrowIsOnBottom = false;
    else{
      for(int i = 0; i < TimerEntries; i++){
        datas[i].position++;
        lines[i] = LiquidLine(2, datas[i].position, datas[i].name);
        selector.row = 0;
      }
      menu--;
    }
    lcd.clear();
    lcd.setCursor(selector.column, selector.row);
    menu.update();
    
    Serial.println(selector.row);
  }
  if(downButton.isPressedOnce() && QueryPosition < TimerEntries - 1){
    selector.row++;
    QueryPosition++;
    if(!arrowIsOnBottom)arrowIsOnBottom = true;
    else {
      for(int i = 0; i < TimerEntries; i++){
      datas[i].position--;
      lines[i] = LiquidLine(2, datas[i].position, datas[i].name);
      selector.row = 1;
      }
      menu++;
    }

    lcd.clear();
    lcd.setCursor(selector.column, selector.row);
    menu.update();
  
    Serial.println(selector.row);
  }
  if(middleButton.isPressedOnce()){

    timer.setDuration(datas[QueryPosition].duration);
    timer.setName(String(datas[QueryPosition].name));
    lcd.clear();
    mode = TIMER;


  }

}

void runTimer(){

  lcd.setCursor(0,0);
  lcd.print(timer.getName());
  lcd.setCursor(0, 1);
  lcd.print(timer.getDurationAsString());
  timer.manageTimer();
  if(middleButton.isPressedOnce()){
    timer.toggle();
  }
  if(downButton.isPressedOnce()){
    timer.stop();
    lcd.clear();
    menu.update();
    lcd.setCursor(0, selector.row);
    lcd.print(selector.selector);
    lcd.setCursor(0, selector.row);
    mode = MENU;
  }

}

void addTimer(){








}

void loop() {
  upButton.manageButton();
  middleButton.manageButton();
  downButton.manageButton();
  if(mode == MENU)
    runMenu();
  else if(mode == TIMER)
    runTimer();
}