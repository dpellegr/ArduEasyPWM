# ArduEasyPWM
Flexible, simple and everlasting library for ***software PWM***.

ArduEasyPWM allows for simultaneous output of square waves on the PORTB pins (pin 8 to 13) controlling and eventually varying both the period and the duty cycle on the fly.

## Getting Started:

```
#include "ArduEasyPWM.h"

void setup() {
  // Builtin led
  pinMode(13, OUTPUT);
}

unsigned long period_us = 1000000;
byte duty_cycle = 50;
byte full_duty = 100;

PWM pwm( PWM::P13, period_us, duty_cycle, full_duty);

void loop() {  
  pwm.up();
}
```

This will make the builtin led blink with a period of 1 second, being on 50% of the time. Note that this code is non-blocking, that is you can have as many PWM generators as the MCU supports (depending on their periods), without having to worry about `delays()`.

In general shorter periods will require more frequent calls to the `up` member function inside the loop to maintain a high relative accuracy of the switching times.

At any time you can modify the variables `pwm::period`, `pwm::duty_cycle` and `pwm::full_duty` making the signal a function of time and/or other inputs. The updates to these variables are taken into account twice every period (on the rising and falling edge), this will avoid any kind of glitching that you may get if you call ANALOG_WRITE too frequently, but keep in mind that if you set a period of 1 minute, you might not be able to effectively modify it untill this minute is over.

## Can you make a led breath?

Well, you should already have all the instruments, but I have done the exercise already, so here we go:

```
#include "ArduEasyPWM.h"

void setup() {
  // Builtin led
  pinMode(13, OUTPUT);
}

LED led(PWM::P13);

void loop() {  
  led.up(breath_fast);
}
```

`breath_fast` is a two-column table defined at the bottom of `ArduEasyPWM.h` together with few other examples. The first column indicates time, the second the corresponding duty-cycles. The time in the last row represents the period of the animation, after which the sequence is repeated. The table is linearly interpolated to update the duty cycle.

Note that the period of the animation is not the period of the PWM signal. The latter is set internally by the LED class to 25 ms, while the former is deduced from the table.
