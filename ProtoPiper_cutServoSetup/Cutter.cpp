#include "Cutter.h"

#include <EEPROM.h>

const uint16_t Cutter::counter_addresses[] = {0x10, 0x20};

Cutter::Cutter(uint8_t servo_id, uint8_t servo_direction_pin, uint8_t center_wire_pin, uint8_t wire_pin, Battery *battery) :
  _servo(Dynamixel(servo_id, servo_direction_pin)),
  _center_wire_pin(center_wire_pin),
  _wire_pin(wire_pin),
  _battery(battery)
{
  pinMode(_center_wire_pin, OUTPUT);
  pinMode(_wire_pin, OUTPUT);

  digitalWrite(_center_wire_pin, LOW);
  digitalWrite(_wire_pin, LOW);
}

void Cutter::cut(Cut type)
{
  writeCounter(type, readCounter(type) + 1);

  switch(type) {
    case Straight:
      cutStraight();
      break;
    case Connector:
      cutConnector();
      break;
  }
}

void Cutter::clean()
{
  prepare();
  delay(1000);

//  heatControl(15000, 75000, 4000); // toaster wire
  heatControl(5000, 35000, 4000); // old wire

  suspend();
  delay(1000);
}

void Cutter::prepare()
{
  _servo.write(Dynamixel::GOAL_VELOCITY, 0x2FF);
  _servo.write(Dynamixel::GOAL_POSITION, open_position);
}

void Cutter::suspend()
{
  _servo.write(Dynamixel::GOAL_VELOCITY, 0x2FF);
  _servo.write(Dynamixel::GOAL_POSITION, standby_position);  
}

void Cutter::resetCounters()
{
  writeCounter(Straight, 0);
  writeCounter(Connector, 0);
}

uint16_t Cutter::readCounter(Cut type)
{
  return (EEPROM.read(counter_addresses[type]) << 8) + EEPROM.read(counter_addresses[type] + 1);
}

void Cutter::cutStraight()
{
  _servo.write(Dynamixel::LED, Dynamixel::RED);

//  heatControl(7000, 0, 1500); // toaster wire
  heatControl(5000, 0, 1500); // old wire

  _servo.write(Dynamixel::GOAL_VELOCITY, 0x12F);
  _servo.write(Dynamixel::GOAL_POSITION, cutting_position - 30);

//  heatControl(7000, 0, 1000); // toaster wire
  heatControl(5000, 0, 1500); // old wire

  _servo.write(Dynamixel::GOAL_VELOCITY, 0x2FF);
  _servo.write(Dynamixel::GOAL_POSITION, open_position);

  delay(1000);

  _servo.write(Dynamixel::LED, Dynamixel::OFF);
}

void Cutter::cutConnector()
{
  _servo.write(Dynamixel::LED, Dynamixel::RED);
  
//  heatControl(9000, 70000, 800); // toaster wire
  heatControl(5000, 30000, 1000); // old wire

  _servo.write(Dynamixel::GOAL_VELOCITY, 0x1FF);
  _servo.write(Dynamixel::GOAL_POSITION, cutting_position);

//  heatControl(9000, 70000, 1000); // toaster wire
  heatControl(5000, 30000, 1300); // old wire

  _servo.write(Dynamixel::GOAL_VELOCITY, 0x3FF);
  _servo.write(Dynamixel::GOAL_POSITION, open_position);
  
//  heatControl(7000, 70000, 500); // toaster wire
  heatControl(5000, 30000, 500); // old wire
  delay(500);

  _servo.write(Dynamixel::LED, Dynamixel::OFF);
}

void Cutter::heatControl(uint32_t center_wire_temp, uint32_t wire_temp, uint16_t duration)
{
  uint32_t end = millis() + duration;
  while(millis() < end) {
    uint16_t voltage = _battery->voltage();
    analogWrite(_center_wire_pin, center_wire_temp / voltage);
    analogWrite(_wire_pin, wire_temp / voltage);
  }

  digitalWrite(_center_wire_pin, LOW);
  digitalWrite(_wire_pin, LOW);
}

void Cutter::writeCounter(Cut type, uint16_t value)
{
  EEPROM.write(counter_addresses[type] + 1, value & 0xFF);
  EEPROM.write(counter_addresses[type], value >> 8);
}
