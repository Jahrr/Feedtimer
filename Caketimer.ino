/*

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

 This program is a timer that is intended to be connected to a caker/feeder. It can store multiple timers in EEPROM,
 has an interactive menu, and a naming system.

 TODO: Ability to backspace on duration of new timer, 
 simplify and clean up code, make confirmation universal instead of being tied to duration,
 add long press support to the button class, make Button::isPressedOnce better (registers two presses per press often)

 Low Priority TODO: Rename timers, folder system

*/

// include the library code:
#include <LiquidCrystal.h>
#include <LiquidMenu.h>
#include <EEPROM.h>
#include "Timer.h"
#include "Button.h"

void(* resetFunc) (void) = 0;

const bool canWrite = true;

//Initialize pins
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int runningLED = 50;
const int upButtonPin = 30;
const int middleButtonPin = 34;
const int downButtonPin = 38;
const int sideButtonPin = 42;


struct Selector{
  char selector = '>';
  int row = 0;
  int column = 0;
};

struct TimerData{
    TimerData(){}
    TimerData(const char* title, int pos, int length) : position{pos}, duration{length}{
      int it = 0;
      for(int i = 0; i < strlen(title); i++){
        if(it >= 11){
          break;
        }
        name[i] = title[i];
        it++;
        }
        for(;it<11;it++){
          name[it]=' '; 

        }
        name[11] = '\0';
    }
    char name[12];
    int position;
    int duration;  
};




enum mode{
MENU, TIMER, ADDNAME, ADDDURATION, CONFIRM, DELETING
  
};


//variables
Timer displayTimer(false);
Timer timer(true);

const unsigned int displayTimeout = 60;
unsigned int TimerEntries = 0;
unsigned int QueryPosition = 0;
unsigned int mode = MENU;

bool confirmed = false;


TimerData td("NULL", 0, 0);
TimerData addTimerData;

TimerData datas[16](16, td);


LiquidLine line(0,0,"");

LiquidLine lines[16](16, line);

LiquidScreen screen;

LiquidScreen screens[15](15, screen);
bool arrowIsOnBottom = false;

LiquidMenu menu(lcd);

Selector selector;


Button upButton(upButtonPin);
Button middleButton(middleButtonPin);
Button downButton(downButtonPin);
Button sideButton(sideButtonPin);

void setup() {
  // set up the LCD's number of columns and rows:
  pinMode(runningLED, OUTPUT);
  Serial.begin(9600);
  readAndParse();
  datas[TimerEntries] = TimerData("Add", TimerEntries, 0);
  TimerEntries++;
  setupMenu();

  lcd.begin(16, 4);
  menu.update();
  lcd.setCursor(0, 0);
  lcd.print(selector.selector);
  lcd.setCursor(0, 0);
  displayTimer.setDuration(displayTimeout);
  displayTimer.start();
}

void setupMenu(){

  for(int i = 0; i < TimerEntries; i++){
    lines[i] = LiquidLine(2, datas[i].position, datas[i].name);
  }

  for(int i = 0; i < TimerEntries; i++){
    screens[i] = LiquidScreen(lines[i]);
    if(i + 1 < TimerEntries){
      screens[i].add_line(lines[i+1]);
    }
  }
  menu = LiquidMenu(lcd);
  for(int i = 0; i < TimerEntries; i++){
    menu.add_screen(screens[i]);
  }
  //Serial.println(menu.change_screen(&screens[QueryPosition]));
}

int getCharsetIterator(char currentChar){
  if(currentChar > 64 && currentChar < 91) return 0;
  else if(currentChar > 96) return 1;
  else if(currentChar < 58) return 2;
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
  

  }
  
  if(middleButton.isPressedOnce()){

    if(QueryPosition == TimerEntries - 1){
      Serial.println("switching to add mode");
      mode = ADDNAME;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Enter Name");
      lcd.setCursor(0,1);
      lcd.cursor();
    }else{
    timer.setDuration(datas[QueryPosition].duration);
    timer.setName(String(datas[QueryPosition].name));
    lcd.clear();
    mode = TIMER;
    }

  }
  if(sideButton.isPressedOnce() && QueryPosition != TimerEntries - 1){
    Serial.println("sideButton pressed");
    setAdd("Delete?");
    lcd.print("L/n R/y");
    mode = DELETING;
    return;
  }

}


void deleteTimer(){

  if(confirmed){
    TimerData compareData;
    for(int i = 0; i < 272; i += sizeof(TimerData) + 1){
      EEPROM.get(i+1, compareData);
      Serial.println(String(compareData.name) + ":" + String(datas[QueryPosition].name));
      if(String(compareData.name) == String(datas[QueryPosition].name)){
        EEPROM.update(i, 254);
        Serial.println("Deleted.");
        resetFunc();
      }      
    }
    Serial.println("Error: Could not locate memory address of timer; timer not deleted.");
    mode = MENU;
    lcd.clear();
    lcd.setCursor(0, selector.row);
    lcd.print(selector.selector);
    lcd.setCursor(0, selector.row);
    menu.update();
    return;
  }
  else{
    if(upButton.isPressedOnce()){
      confirmed = true;
      Serial.println("Deletion confirmed");
    }
    if(downButton.isPressedOnce()){
      mode = MENU;
      lcd.clear();
      lcd.setCursor(0, selector.row);
      lcd.print(selector.selector);
      lcd.setCursor(0, selector.row);
      menu.update();
      return;
    }
        
  }



}

void runTimer(){
  lcd.setCursor(0,0);
  lcd.print(timer.getName());
  lcd.setCursor(0, 1);
  lcd.print(timer.getDurationAsString());
  digitalWrite(runningLED, timer.manageTimer());
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
  if(upButton.isPressedOnce()){
    timer.addTime(10);
  }

}


void setAdd(String text){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(text);
  lcd.setCursor(0,1);
}

void addTimerName(){
static String timerName;
static char currentChar = 'A' - 1;
static int pos = 0;
const char charMinArr[3]{'A', 'a', '0'};
const char charMaxArr[3]{'Z', 'z', '9'};
static char charMin = 'A';
static char charMax = 'Z';
static int charArrIt = 0;
if(upButton.isPressedOnce()){
  setAdd("Enter Name");
  lcd.print(timerName);
  if(currentChar + 1 <= charMax && currentChar != ' ' && currentChar != '<' && currentChar != '>'){
    currentChar++;
  }else if(currentChar == ' '){
    currentChar = charMin;
  }else if(currentChar == '>'){
    currentChar = ' ';
  }else if(currentChar == '<'){
    currentChar = '>';
  }
  lcd.print(currentChar);
  lcd.setCursor(pos,1);
}
  if(downButton.isPressedOnce()){
    setAdd("Enter Name");
    lcd.print(timerName);
    if(currentChar - 1 >= charMin && currentChar != ' ' && currentChar != '>' && currentChar != '<'){
      currentChar--;
    }else if(currentChar == charMin){
      currentChar = ' ';
    }else if(currentChar == ' ' || currentChar == charMin-1){
      currentChar = '>';
    }else if(currentChar == '>'){
      currentChar = '<';
    }
    lcd.print(currentChar);
    lcd.setCursor(pos,1);
  }

  if(middleButton.isPressedOnce()){ 
    if(currentChar == '@'){
      timerName += ' ';
      currentChar = 'A' - 1;
      pos++;
      setAdd("Enter Name");
      lcd.print(timerName);
    }else if (currentChar == '>'){
      //move to duration set
      //write to allocated timerdata to prepare for write to eeprom/flash 
      addTimerData = TimerData(timerName.c_str(), datas[TimerEntries - 1].position, 0);
      timerName = "";
      pos = 0;
      currentChar = '@';      
      mode = ADDDURATION;
      setAdd("Enter Duration");
      lcd.print("00:00");
      lcd.setCursor(0,1);    
      return;  
    }else if (currentChar == '<'){ 
      //backspace time baby
      if(pos > 0){
        pos--;
        currentChar = timerName[pos];
        //sync charset
        int charsetIt = getCharsetIterator(currentChar);
        charMin = charMinArr[charsetIt];
        charMax = charMaxArr[charsetIt];                
        timerName = timerName.substring(0, pos);
        setAdd("Enter Name");
        lcd.print(timerName);
        lcd.print(currentChar);
        lcd.setCursor(pos,1);
      }else{
      timerName = "";
      pos = 0;
      currentChar = 'A' - 1;
      mode = MENU;
      lcd.setCursor(0, selector.row);
      lcd.print(selector.selector);
      lcd.setCursor(0, selector.row);
      lcd.noCursor();
      lcd.clear();
      menu.update();
      return;
      }
    }else{
    timerName += currentChar;
    currentChar = ' ';
    pos++;
    setAdd("Enter Name");
    lcd.print(timerName);
    }


  }
  if(sideButton.isPressedOnce()){
    Serial.println(charArrIt);
    charArrIt = (charArrIt < 2) ? charArrIt + 1 : 0;
    Serial.println(charArrIt);
    charMin = charMinArr[charArrIt];
    charMax = charMaxArr[charArrIt];
    currentChar = charMin;
    lcd.print(currentChar);
    lcd.setCursor(pos,1);
  }


}
String currentDuration = "00:00";
void addTimerDuration(){
  
  static int pos = 0;
  if(upButton.isPressedOnce()){
    if(currentDuration[pos] + 1 <= '9'){
      currentDuration[pos]++;
      setAdd("Enter Duration");
      lcd.print(currentDuration);
      lcd.setCursor(pos, 1);      
    }
  }if(downButton.isPressedOnce()){
    if(currentDuration[pos] - 1 >= '0'){
      currentDuration[pos]--;
      setAdd("Enter Duration");
      lcd.print(currentDuration);
      lcd.setCursor(pos, 1);      
    }
  }if(middleButton.isPressedOnce()){
    if(pos == 0 || pos == 3){
      setAdd("Enter Duration");
      lcd.print(currentDuration);
      pos++;
      lcd.setCursor(pos, 1);

    }else if(pos == 1){
      setAdd("Enter Duration");
      lcd.print(currentDuration);
      pos+=2;
      lcd.setCursor(pos, 1);
    }else if(pos == 4){
      pos = 0;
      setAdd("Confirm");
      lcd.noCursor();
      lcd.print("L/n R/y");
      mode = CONFIRM;
      Serial.println("Switching to mode::CONFIRM");
      }
    }
  }


void confirmTimer(){


      if(upButton.isPressedOnce()){
                
        String cleanDurationString;
        cleanDurationString += currentDuration[0];
        cleanDurationString += currentDuration[1];
        cleanDurationString += currentDuration[3];
        cleanDurationString += currentDuration[4];
        int durationStringToInt = cleanDurationString.toInt();
        addTimerData.duration = ((durationStringToInt/100) * 60 + (durationStringToInt - ((durationStringToInt/100) * 100)));
        Serial.println("Confirmed... duration is " + String(addTimerData.duration));
        currentDuration = "00:00";
        write();
        mode = MENU;
        lcd.clear();
        lcd.setCursor(0, selector.row);
        lcd.print(selector.selector);
        lcd.setCursor(0, selector.row);
        menu.update();
        return;
      }
      if(downButton.isPressedOnce()){
        Serial.println("Confirmation denied... returning to mode::MENU");
        currentDuration = "00:00";
        mode = MENU;
        lcd.clear();
        lcd.setCursor(0, selector.row);
        lcd.print(selector.selector);
        lcd.setCursor(0, selector.row);
        menu.update();
        return;
      }

}



void readAndParse(){
  int it = 0;
  for(int i = 0; (EEPROM.read(i) == 128 || EEPROM.read(i) == 254) && it < 16; i += sizeof(TimerData) + 1){
    if(EEPROM.read(i) == 254){
      continue;
    }
    EEPROM.get(i+1, datas[it]);
    datas[it].position = it;
    it++;
  }
  TimerEntries = it;
  Serial.println(datas[0].name);
  Serial.println(datas[0].position);
  Serial.println(datas[0].duration);

}

void write(){
  /*
  Serial.println("Writing timerdata: Name = " + String(addTimerData.name) + ", Position = " + String(addTimerData.position) + ", Duration = " + String(addTimerData.duration));
  //set Add data to the next position
  datas[TimerEntries] = datas[TimerEntries-1];
  //insert new timerdata into the previous position where Add was
  datas[TimerEntries-1] = addTimerData;
  //increment the new add data's position to reflect its new placement
  datas[TimerEntries].position++;
  //redo the menu to reflect the new changes
  TimerEntries++;
    for(int i = 0; i < TimerEntries; i++){
      datas[i].position+=TimerEntries-3;
    Serial.println("Name = " + String(datas[i].name) + ", Position = " + String(datas[i].position) + ", Duration = " + String(datas[i].duration));
  }
  
  QueryPosition = 0;
  selector.row = 0;
  arrowIsOnBottom = false;
  setupMenu();
  //selector.row = 0;
  //arrowIsOnBottom = false;
*/  
  if(canWrite){
    int writableByte = 0;
    while(EEPROM.read(writableByte) == 128){
      writableByte += sizeof(TimerData) + 1;
    }
    EEPROM.update(writableByte, 128);
    EEPROM.put(writableByte + 1, addTimerData);
    Serial.println("Write Complete. new Timerdata written to " + String(writableByte) + "; written block ends at " + String(writableByte + 1 + sizeof(TimerData)));
  }else{
    Serial.println("New Timerdata not written to EEPROM: Access denied; check canWrite variable");
  }
  resetFunc();
  //screens[addTimerData.position] = LiquidScreen(lines[addTimerData.position], lines[addTimerData.position+1]);
  //menu.add_screen(screens[addTimerData.position]);
}

void loop() {
  upButton.manageButton();
  middleButton.manageButton();
  downButton.manageButton();
  sideButton.manageButton();
  displayTimer.manageTimer();
  if(upButton.isPressed() || middleButton.isPressed() || downButton.isPressed()){
    displayTimer.reset();
    displayTimer.start();
    lcd.display();
  }
  if(displayTimer.getDurationInSeconds() == 0 && mode != TIMER){
    lcd.noDisplay();
  }

  switch(mode){
    case MENU:
      runMenu();
      break;
    case TIMER:
      runTimer();
      break;
    case ADDNAME:
      addTimerName();
      break;
    case ADDDURATION:
      addTimerDuration();
      break;  
    case CONFIRM:
      confirmTimer();
      break;
    case DELETING:
      deleteTimer();
      break;
  }
}