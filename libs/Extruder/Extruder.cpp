#include "Extruder.h"

#include <Dynamixel.h>
#include <EEPROM.h>
#include <Display.h>

Extruder::Extruder(uint8_t servo_id, uint8_t servo_direction_pin) :
  _servo(Dynamixel(servo_id, servo_direction_pin)),
  _waitForZero(false)
{
}

void Extruder::process()
{
  if(!_extruding) {
    return;
  }

  int16_t position = readPosition();
  uint32_t length = _length;

  if(_waitForZero) {
    if(position == 0) {
      _waitForZero = false;
      length += 205;
    }
  } else {
    if(position == 1023) {
      _waitForZero = true;
    }
    length += position - _position;
  }

  if(_callback && !_silent && _length != length) {
    _callback(length / rad_to_cm_ratio);
  }

  _position = position;
  _length = length;

  if(_limit && _length >= _limit) {
    stop();
  }
}

void Extruder::onLengthChanged(Callback callback)
{
  _callback = callback;
}

void Extruder::start(float length, bool busy)
{
  if(busy) {
    reset();
  }

  _limit = length * rad_to_cm_ratio;
  _extruding = true;
  _silent = busy;
  _position = readPosition();

  _servo.write(Dynamixel::GOAL_VELOCITY, busy ? slow_forward_velocity : forward_velocity);
  _servo.write(Dynamixel::LED, Dynamixel::BLUE);

  while(_silent && _extruding) {
    process();
  }

  if(busy) {
    reset();
  }
}

void Extruder::reverse()
{
  _extruding = false;
  _servo.write(Dynamixel::GOAL_VELOCITY, reverse_velocity);
  _servo.write(Dynamixel::LED, Dynamixel::GREEN);
}

void Extruder::stop()
{
  _limit = 0;
  process();
  _extruding = false;
  _servo.write(Dynamixel::GOAL_VELOCITY, off_velocity);
  _servo.write(Dynamixel::LED, Dynamixel::OFF);
}

void Extruder::reset()
{
  writeCounter(readCounter() + _length);
  _length = 0;
}

uint16_t Extruder::length()
{
	return _length / rad_to_cm_ratio;
}

uint16_t Extruder::totalLength()
{
  return readCounter() / 100 / rad_to_cm_ratio;
}

void Extruder::resetCounter()
{
  writeCounter(0);
}


uint16_t Extruder::readPosition()
{
  return 1023 - _servo.read(Dynamixel::POSITION);
}

uint32_t Extruder::readCounter()
{
  uint32_t value = 0;

  for(uint8_t i=0; i<4; i++) {
    value = (value << 8) + EEPROM.read(counter_address + i);
  }

  return value;
}

void Extruder::writeCounter(uint32_t value)
{
  for(uint8_t i=0; i<4; i++) {
    EEPROM.write(counter_address + 3 - i, value & 0xFF);
    value >>= 8;
  }
}

