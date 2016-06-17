#ifndef __EXTRUDER_H__
#define __EXTRUDER_H__

#include "Dynamixel.h"

class Extruder
{
  typedef void (*Callback)(uint16_t);

  static const uint16_t slow_forward_velocity = 0x400 + 0x2FF;
  static const uint16_t forward_velocity = 0x400 + 0x3FF;
  static const uint16_t reverse_velocity = 0x0FF;
  static const uint16_t off_velocity = 0x000;
  static constexpr float rad_to_cm_ratio = 233-0;

  static const uint16_t counter_address = 0x30;

  public:
    Extruder(uint8_t servo_id, uint8_t servo_direction_pin);
    void process();

    void onLengthChanged(Callback callback);

    void start(float length=0, bool busy=false);
    void stop();
    void reset();
    void reverse();
    
    uint16_t length();

    uint16_t totalLength();
    void resetCounter();

  private:
    uint16_t readPosition();

    uint32_t readCounter();
    void writeCounter(uint32_t value);

  	Dynamixel _servo;
  	int16_t _position;
    uint32_t _length;
  	uint32_t _limit;
    bool _waitForZero;
  	bool _extruding;
    bool _silent;
  	Callback _callback;
};

#endif
