// Author: Dario Pellegrini <pellegrini dot dario at gmail dot com>
// Written: Aug 2019

constexpr char P8 = B00000001;
constexpr char P9 = B00000010;
constexpr char P10= B00000100;
constexpr char P11= B00001000;
constexpr char P12= B00010000;
constexpr char P13= B00100000; //LED PIN

struct PWM {
  // User has RW access to these variables
  char pin = 0; //in the PORTB format
  unsigned long period_us = -1;
  float duty_cycle = 0;

  // default and proper constructor
  PWM() = default;
  PWM(const char pin, const unsigned long period_us, const float duty_cycle):
    pin(pin), period_us(period_us), duty_cycle(duty_cycle)
  {
    last_switch_time = micros();
  }

  // copy and move ctor and assignment
  PWM(const PWM & other) = default;
  PWM(PWM && other) = default;
  PWM& operator=(const PWM & other) = default;
  PWM& operator=(PWM && other) = default;

  // update function, to be called in the loop()
  void up() {
    const long unsigned tnow = micros();
    if (tnow - last_switch_time > keep_duration) {
      if (status) {
        //switch off
        status = false;
        PORTB &= ~pin;
        keep_duration = period_us*(1.-duty_cycle);
      } else {
        //switch on
        status = true;
        PORTB |= pin;
        keep_duration = period_us*duty_cycle;
      }
      last_switch_time = tnow;
    }
  }

  private:
  // User should not mess with these variables
  bool status = false;
  unsigned long last_switch_time = 0;
  unsigned long keep_duration = 0;
};

PWM PWM9; // allocate the PWM object
const int potpin = 7; // analog pin used to connect the potentiometer

void setup() {
  pinMode(potpin, INPUT);
  pinMode(9, OUTPUT);
  PWM9 = PWM(P9, 20e3, 0.5);
}

void loop() {
  PWM9.duty_cycle = map(analogRead(potpin), 0, 1023, 25, 75)/1000.;
  PWM9.up();
}
