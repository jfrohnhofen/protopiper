#include "Selector.h"

Selector::Selector(uint8_t min_pin, uint8_t max_pin) :
  _min_pin(min_pin),
  _max_pin(max_pin),
  _pin_count(max_pin - min_pin + 1),
  _status(-1)
{
  for(int i=_min_pin; i<=_max_pin; i++) {
    pinMode(i, INPUT_PULLUP);
  }
}

void Selector::onChanged(Callback callback)
{
  _callback = callback;
}

void Selector::process()
{
  if(!_callback) {
    return;
  }

  for(int i=_min_pin; i<=_max_pin; i++) {
    if(i != _status && i + _pin_count != _status && !digitalRead(i)) {
      _status = abs(_status - i) < abs(_status - i - _pin_count) ? i : i + _pin_count;
      _callback(_status - _min_pin);
      break;
    }
  }
}

void Selector::emit()
{
  if(_callback) {
    _callback(_status - _min_pin);
  }
}
