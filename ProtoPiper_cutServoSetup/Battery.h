#ifndef __BATTERY_H__
#define __BATTERY_H__

#include <Arduino.h>

class Battery
{
  static const uint16_t lower_voltage_thresh = 480;
  static const uint16_t upper_voltage_thresh = 500;

  typedef void(*Callback)();

  public:
    typedef enum {LowVoltage, HighVoltage} Event;

    Battery(uint8_t pin);
    void process();
    void on(Event event, Callback callback);

    uint16_t voltage();

  private:
    uint8_t _pin;
  	uint16_t _voltage;
    Callback _callbacks[2];
};

#endif
