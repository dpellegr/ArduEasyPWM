// Author: Dario Pellegrini <pellegrini dot dario at gmail dot com>
// Written: Aug 2019
#include "ArduEasyPWM.h"

PWM PWM9; // allocate the PWM object
const int potpin = 7; // analog pin used to connect the potentiometer

void setup() {
  pinMode(potpin, INPUT);
  pinMode(9, OUTPUT);
          //(     Pin, period [us], duty cycle [0:full_duty], full duty (max 255)
  PWM9 = PWM( PWM::P9,        20e3,                       50,                100);
}

void loop() {
  PWM9.duty_cycle = map(analogRead(potpin), 0, 1023, 25, 75);
  PWM9.up();
}
