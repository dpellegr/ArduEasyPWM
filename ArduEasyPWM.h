// Author: Dario Pellegrini <pellegrini dot dario at gmail dot com>
// Written: Aug 2019


struct PWM {
  using micros_t = decltype(micros());
  static constexpr char P8 = B00000001;
  static constexpr char P9 = B00000010;
  static constexpr char P10= B00000100;
  static constexpr char P11= B00001000;
  static constexpr char P12= B00010000;
  static constexpr char P13= B00100000; //LED PIN

  // User has RW access to these variables
  char pin = 0; //in the PORTB format
  micros_t period_us = -1;
  char duty_cycle = 0;
  char full_duty = 100; //can be increased up to 255 for more granularity on the duty cycle

  // default and proper constructor
  PWM() = default;
  PWM(const char pin, const micros_t period_us, const char duty_cycle = 50, const char full_duty = 100):
    pin(pin),
    period_us(period_us),
    duty_cycle(duty_cycle),
    full_duty(full_duty),
    last_switch_time(micros())
  {}

  // copy and move ctor and assignment
  PWM(const PWM & other) = default;
  PWM(PWM && other) = default;
  PWM& operator=(const PWM & other) = default;
  PWM& operator=(PWM && other) = default;

  // update function, to be called in the loop()
  void up() {
    const micros_t tnow = micros();
    if (tnow > last_switch_time + keep_duration) {
      if (status) {
        //switch off
        status = false;
        PORTB &= ~pin;
        keep_duration = period_us * (full_duty-duty_cycle) / full_duty;
      } else {
        //switch on
        status = true;
        PORTB |= pin;
        keep_duration = period_us * duty_cycle / full_duty;
      }
      last_switch_time = tnow;
    }
  }

  protected:
  // User should not mess with these variables
  bool status = false;
  micros_t last_switch_time = 0;
  micros_t keep_duration = 0;
};
