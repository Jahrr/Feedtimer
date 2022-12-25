
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
  bool b_isPressed;
  bool b_wasPressed;
  



};

#endif //CAKETIMERCL_TIMER_H