#include "Button.h"

Button::Button(uint8_t pin) : b_isPressed{false}, b_wasPressed{false}, b_delayedWasPressed{false}, startingMillis{0}, activationTime{100}{

this->pin = pin;
pinMode(pin, INPUT);

}
Button::~Button(){}



void Button::manageButton(){
  if(digitalRead(pin) == HIGH){
    if(!b_isPressed){
      b_wasPressed = true;
      startingMillis = millis();
    }else{
      b_wasPressed = false;
    }
    if(millis() > startingMillis + activationTime && !b_delayedWasPressed){
      b_delayedWasPressed = true;
    }else{
      b_delayedWasPressed = false;
    }
    b_isPressed = true; 
  }else{
    b_isPressed = false;
    b_wasPressed = false;
  }
}
bool Button::isPressed(){
  return b_isPressed;
}
bool Button::isPressedOnce(){
  return b_delayedWasPressed;
}