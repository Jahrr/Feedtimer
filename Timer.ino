//
// Created by minri on 12/23/2022.
//

#include "Timer.h"

Timer::Timer() : duration{0}, seconds{duration}, running{false}{

}

Timer::Timer(int durationInSeconds) {
    duration = durationInSeconds;
}


Timer::~Timer() {

}

void Timer::start() {
    startingMillis = millis();
    running = true;
}

void Timer::reset() {
    seconds = duration;
}

void Timer::stop() {
    running = false;
}

void Timer::toggle(){
  if(!running)start();
  else stop();
}

void Timer::addTime(int amountOfSeconds){
  seconds += amountOfSeconds;
}

bool Timer::manageTimer() {
    if(running){
      if(millis() > startingMillis + 1000){
        seconds--;
        startingMillis = millis();
      }
      if(seconds <= 0){
        running = false;
      }
    }
    return running;
}

void Timer::setDuration(int durationInSeconds) {
    duration = durationInSeconds;
    seconds = duration;
}

void Timer::setName(String n){
  name = n;
}

int Timer::getDurationInSeconds() const {
    return seconds;
}

String Timer::getDurationAsString(){
    return String(seconds/60) + ":" + (((seconds % 60 < 10) && (seconds % 60 > 0)) ? "0" : "") + String(seconds - (seconds/60) * 60) + (seconds % 60 == 0 ? "0" : "");
}

String Timer::getName(){
  return name;
}

