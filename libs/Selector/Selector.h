#ifndef __SELECTOR_H__
#define __SELECTOR_H__

#include "Arduino.h"

class Selector
{
  typedef void(*Callback)(uint8_t);

  static const uint16_t status_address = 0x00;

  public:
    Selector(uint8_t min_pins, uint8_t max_pin);
    void process();
    void onChanged(Callback callback);
    void emit();

  private:
  	uint8_t _min_pin;
  	uint8_t _max_pin;
    uint8_t _pin_count;
  	int8_t _status;
  	Callback _callback;
};

#endif
