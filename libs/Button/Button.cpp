#include "Arduino.h"
#include "Button.h"

Button::Button(uint8_t pin) :
  _pin(pin),
  _pin_state(HIGH),
  _pin_count(0),
  _state(Normal),
  _down_millis(millis())
{
  pinMode(_pin, INPUT_PULLUP);
}

void Button::process()
{
  // read input and debounce pin
  char read = digitalRead(_pin);
  if(read == _pin_state) {
    _pin_count = 0;
  } else {
    _pin_count++;
  }

  // update button state, if pin changed
  if(_pin_count > debounce_count) {
    _pin_state = read;
    _pin_count = 0;
    if(_pin_state) {
      // button is now up
      switch(_state) {
        case Down:
          if(_callbacks[DoublePush] || _callbacks[TriplePush]) {
            _state = Pushed;
          } else {
            _state = Normal;
            emit(Push);
          }
          break;
        case Long:
          _state = Normal;
          emit(LongPushEnd);
          break;
        case Double:
          if(_callbacks[TriplePush]) {
            _state = PushedTwice;
          } else {
            _state = Normal;
            emit(DoublePush);
          }
          break;
        case Triple:
          _state = Normal;
          emit(TriplePush);
          break;
      }
    } else {
      // button is now down
      _down_millis = millis();
      switch(_state) {
        case Normal:
          // no need to wait for long_push_timeout if no callback on regular pushs is registered
          if(_callbacks[Push] || _callbacks[DoublePush] || _callbacks[TriplePush]) {
            _state = Down;
          } else {
            _state = Long;
            emit(LongPushBegin);
          }
          break;
        case Pushed:
          _state = Double;
          break;
        case PushedTwice:
          _state = Triple;
          break;
      }
    }
  }

  // check if button has been pushed long enough for long push
  if((_callbacks[LongPushBegin] || _callbacks[LongPushEnd]) && _state == Down && millis() - _down_millis >= long_push_timeout) {
    _state = Long;
    emit(LongPushBegin);
  }

  // check if timout for double push has elapsed
  if(millis() - _down_millis >= multi_push_timeout) {
    switch(_state) {
      case Pushed:
        _state = Normal;
        emit(Push);
        break;
      case PushedTwice:
        _state = Normal;
        emit(DoublePush);
        break;
    }
  }
}

void Button::on(Event event, Callback callback)
{
  _callbacks[event] = callback;
}

void Button::emit(Event event)
{
  if(_callbacks[event]) {
    _callbacks[event]();
  }
}
