// Author: Dario Pellegrini <pellegrini dot dario at gmail dot com>
// Written: Aug 2019
#pragma once

using millis_t = decltype(millis());
using micros_t = decltype(micros());

struct PWM {
  enum pin_t: byte { 
    P8 = B00000001,
    P9 = B00000010,
    P10= B00000100,
    P11= B00001000,
    P12= B00010000,
    P13= B00100000 //LED PIN
  };

  // User has RW access to these variables
  pin_t pin; //in the PORTB format
  micros_t period_us = -1;
  byte duty_cycle = 0;
  byte full_duty = 100; //can be increased up to 255 for more granularity on the duty cycle

  // default and proper constructor
  PWM() = default;
  PWM( pin_t pin, micros_t period_us, byte duty_cycle = 0, byte full_duty = 100):
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
      keep_duration = period_us;
      if (status) {
        if ( duty_cycle != full_duty ) {
          //switch off
          status = false;
          PORTB &= ~pin;
          keep_duration *= full_duty-duty_cycle;
          keep_duration /= full_duty;
        }
      } else {
        if ( duty_cycle != 0 ) {
          //switch on
          status = true;
          PORTB |= pin;
          keep_duration *= duty_cycle;
          keep_duration /= full_duty;
        }
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

//////////////////////////////////////////////////

template <typename A, typename B>
struct Pair {
  A a;
  B b;
};

template <typename A, typename B, unsigned MAX_SIZE = 256>
struct Table {
  using data_t = Pair<A,B>;
  using size_t = decltype(MAX_SIZE);

  data_t data[MAX_SIZE];
  size_t size = 0;

  template<typename Ta, typename Tb>
  Table & push_back(Ta a, Tb b) {
    data[size].a = a;
    data[size].b = b;
    ++size;
    return *this;
  }
  template <typename Ta, typename Tb, typename... Ts>
  Table & push_back(Ta a, Tb b, const Ts &... ds) {
    return push_back(a,b).push_back(ds...);
  }

  /// Append the table content in reversed order
  Table & mirror() {
    const auto period = back().a;
    for (size_t i = size-1; i>0; --i) {
      push_back( 2*period - data[i-1].a, data[i-1].b );
    }
    return *this;
  }
  
  /// Append the table content in the same order n-times
  Table & n_plicate(const unsigned n = 1) {
    const size_t s = size;
    const auto period = back().a;
    for ( unsigned j = 1; j <= n; ++j) { 
      for ( size_t i = 0; i < s; ++i) {
        push_back( data[i].a+j*period, data[i].b );
      }
    }
    return *this;
  }

  
  B lin_interp(const A x) const {
    size_t i = 1;
    while ( x >= data[i].a ) {
      i += 1;
      if ( i == size ) return B{};
    }
    const A x0 = data[i-1].a;
    const A x1 = data[i  ].a;
    const B y0 = data[i-1].b;
    const B y1 = data[i  ].b;
    const B y  = map(x, x0,x1, y0,y1);
    return y;
  }

  const data_t & operator[](size_t i) const { return data[i]; }
        data_t & operator[](size_t i)       { return data[i]; }

  const data_t & back() const { return data[size-1]; }
        data_t & back()       { return data[size-1]; }

  data_t pop_back() { return data[--size]; }
};

struct LED {
  float intensity;
  LED(PWM::pin_t pin, const float intensity = 1.): intensity(intensity), pwm(pin, 25e3, 0, 100), period_start(millis()) {}  

  template <typename TABLE>
  void up(const TABLE & t) {
    const millis_t now = millis();
    if (now > last_check + 50) {
      last_check = now;
      const auto period = t.back().a;
      while (now > period_start + period) period_start += period;
      const millis_t period_pos = now - period_start;
      pwm.duty_cycle = t.lin_interp(period_pos);
      if ( intensity != 1. ) pwm.duty_cycle *= intensity;
    }
    pwm.up();
  }

  template <typename TABLE>
  void cycle(const TABLE & t, const unsigned N = 1) {
    const millis_t period = t.back().a;
    const millis_t timeout = millis() + period*N;
    while (millis() < timeout) {
      up(t);
    }
  }

  private:
  PWM pwm;
  millis_t period_start = 0;
  millis_t last_check = 0;
};

Table<unsigned, byte, 2> makeOn(const unsigned period) {
  Table<unsigned, byte, 2> t;
  t.push_back(
    0, 100, 
    period, 100
  );
  return t;
}

Table<unsigned, byte, 2> makeOff(const unsigned period) {
  Table<unsigned, byte, 2> t;
  t.push_back(
    0, 0,
    period, 0
  );
  return t;
}

Table<unsigned, byte, 7> makeBreath(const unsigned period) {
  Table<unsigned, byte, 7> t;
  t.push_back(
    0, 0,
    period/8, 0,
    period/3, 30,
    period/2, 100
  ).mirror();
  return t;
}

template <unsigned N=1>
Table<unsigned, byte, N*4> makeBlink(const millis_t on, const millis_t off, const millis_t pause = 0) {
  Table<unsigned, byte, N*4> t;
  t.push_back(
    0,  100,
    on, 100,
    on, 0,
    on+off, 0
  ).n_plicate(N-1);
  t.back().a += pause;  
  return t;
}

/// Some premade tables
static const auto on100  = makeOn(100);
static const auto off100 = makeOff(100);
static const auto breath_slow = makeBreath(6000);
static const auto breath_fast = makeBreath(1500);
static const auto blink2 = makeBlink<2>(100, 150, 1000);
static const auto blink3 = makeBlink<3>(100, 150, 1000);
