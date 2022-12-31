#include "Button.h"

Button::Button(uint8_t pin) : b_isPressed{false}, b_wasPressed{false}, b_delayedWasPressed{false}, b_delayedEligible{true}, startingMillis{0}, activationTime{50}{

this->pin = pin;
pinMode(pin, INPUT);

}
Button::~Button(){}



void Button::manageButton(){
  if(digitalRead(pin) == HIGH){
    b_wasPressed = (!b_isPressed) ? true : false;
    if((millis() > startingMillis + activationTime) && b_delayedEligible){
      b_delayedWasPressed = true;
      b_delayedEligible = false;
    }else{
      b_delayedWasPressed = false;
    }
    b_isPressed = true; 
  }else{
    b_isPressed = false;
    b_wasPressed = false;
    startingMillis = millis();
    b_delayedEligible = true;
    b_delayedWasPressed = false;
  }
}
bool Button::isPressed(){
  return b_isPressed;
}
bool Button::isPressedOnce(){
  return b_delayedWasPressed;
}