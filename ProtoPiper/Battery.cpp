#include "Battery.h"

Battery::Battery(uint8_t pin) :
  _pin(pin),
  _voltage(analogRead(pin))
{
}

void Battery::process()
{
  uint16_t voltage = (_voltage * 7 + analogRead(_pin)) >> 3;

  if(voltage < lower_voltage_thresh && _voltage >= lower_voltage_thresh && _callbacks[HighVoltage]) {
    _callbacks[LowVoltage]();
  }

  if(voltage >= upper_voltage_thresh && _voltage < upper_voltage_thresh && _callbacks[HighVoltage]) {
    _callbacks[HighVoltage]();
  }

  _voltage = voltage;
}

void Battery::on(Event event, Callback callback)
{
  _callbacks[event] = callback;
}

uint16_t Battery::voltage()
{
  process();
  return _voltage;
}
