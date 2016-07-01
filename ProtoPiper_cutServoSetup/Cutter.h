#ifndef __CUTTER_H__
#define __CUTTER_H__

#include "Battery.h"
#include "Dynamixel.h"

class Cutter
{
  static const uint16_t cutting_position = 280;
  static const uint16_t open_position = 680;
  static const uint16_t standby_position = 450;

  static const uint16_t counter_addresses[2];

  public:
    typedef enum {Straight, Connector} Cut;

    Cutter(uint8_t servo_id, uint8_t servo_direstion_pin, uint8_t center_wire_pin, uint8_t wire_pin, Battery *battery);

    void cut(Cut type);
    void clean();

    void prepare();
    void suspend();

    void resetCounters();
    uint16_t readCounter(Cut type);

  private:
    void cutStraight();
    void cutConnector();

    void heatControl(uint32_t center_wire_temp, uint32_t wire_temp, uint16_t duration);

    void writeCounter(Cut type, uint16_t value);

  	Battery *_battery;
    Dynamixel _servo;
    uint8_t _voltage_measure_pin;
    uint8_t _center_wire_pin;
    uint8_t _wire_pin;
};

#endif
