
#ifndef CAKETIMERCL_BUTTON_H
#define CAKETIMERCL_BUTTON_H




class Button{

public:
  Button(uint8_t pin);
  ~Button(); 

  void manageButton();
  bool isPressed();
  bool isPressedOnce();

private:
  int pin;
  unsigned long startingMillis;
  unsigned long activationTime;
  bool b_isPressed;
  bool b_wasPressed;
  bool b_delayedWasPressed;
  bool b_delayedEligible;
  



};

#endif //CAKETIMERCL_TIMER_H