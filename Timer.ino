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
    running = true;
}

void Timer::reset() {
    seconds = duration;
}

void Timer::stop() {
    running = false;
}

void Timer::count() {

}

bool Timer::manageTimer() {
    if(running){
      delay(1000);
      if(seconds <= 0){
        running = false;
      }else
        seconds--;
    }
    return running;
}

void Timer::setDuration(int durationInSeconds) {
    duration = durationInSeconds;
    seconds = duration;
}

int Timer::getDurationInSeconds() const {
    return duration;
}

String Timer::getDurationAsString(){
    return String(seconds/60) + ":" + (((seconds % 60 < 10) && (seconds % 60 > 0)) ? "0" : "") + String(seconds - (seconds/60) * 60) + (seconds % 60 == 0 ? "0" : "");
}

