#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <Arduino.h>

class Button
{
  static const uint8_t debounce_count = 3;
  static const uint16_t multi_push_timeout = 500;
  static const uint16_t long_push_timeout = 300;

  typedef void(*Callback)();
  typedef enum {Normal, Down, Pushed, PushedTwice, Long, Double, Triple} State;

  public:
    typedef enum {Push, DoublePush, TriplePush, LongPushBegin, LongPushEnd} Event;

    Button(uint8_t pin);
    void process();
    void on(Event event, Callback callback);

  private:
    void emit(Event event);

    uint8_t _pin;
    uint8_t _pin_state;
    uint8_t _pin_count;
    State _state;
    uint32_t _down_millis;
    Callback _callbacks[5];
};

#endif
