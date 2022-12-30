#include "Button.h"

Button::Button(uint8_t pin) : b_isPressed{false}, b_wasPressed{false}{

this->pin = pin;
pinMode(pin, INPUT);

}
Button::~Button(){}



void Button::manageButton(){
  if(digitalRead(pin) == HIGH){
    b_wasPressed = (b_isPressed == false) ? true : false;
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
  return b_wasPressed;
}