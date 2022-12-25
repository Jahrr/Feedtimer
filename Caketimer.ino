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

//variables
Timer timer;


unsigned int TimerEntries = 4;
unsigned int QueryPosition = 0;

const char names[4][8]{"Timer 1", "Timer 2", "Timer 3", "Timer 4"};
int pos[4]{0,1,2,3};

LiquidLine line1(2,0, "Timer 1");
LiquidLine line2(2,1, "Timer 2");
LiquidLine line3(2,2, "Timer 3");
LiquidLine line4(2,3, "Timer 4");

LiquidLine lines[4]{line1, line2, line3, line4};





int offset = 0;
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

void loop() {
  lcd.setCursor(selector.column, selector.row);
  lcd.print(selector.selector);
  upButton.manageButton();
  middleButton.manageButton();
  downButton.manageButton();
  if(upButton.isPressedOnce() && QueryPosition > 0){
    selector.row--;
    QueryPosition--;
    if(arrowIsOnBottom)arrowIsOnBottom = false;
    else{
      for(int i = 0; i < TimerEntries; i++){
        pos[i]++;
        lines[i] = LiquidLine(2, pos[i], names[i]);
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
      pos[i]--;
      lines[i] = LiquidLine(2, pos[i], names[i]);
      selector.row = 1;
      }
      menu++;
    }
    lcd.clear();
    lcd.setCursor(selector.column, selector.row);
    menu.update();
  
    Serial.println(selector.row);
  }

  //lcd.print("Hello");
}