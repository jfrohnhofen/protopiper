#ifndef __DYNAMIXEL_H__
#define __DYNAMIXEL_H__

#include <Arduino.h>

class Dynamixel
{
  public:
    typedef enum {
      MODEL_NUMBER,
      FIRMWARE,
      POSITION,
      VELOCITY,
      LOAD,
      VOLTAGE,
      TEMPERATURE,
      REGISTERED_INSTRUCTION,
      MOVING,
      HARDWARE_ERROR_STATUS,
      ID,
      BAUD_RATE,
      RETURN_DELAY_TIME,
      CW_ANGLE_LIMIT,
      CCW_ANGLE_LIMIT,
      CONTROL_MODE,
      LIMIT_TEMPERATURE,
      LOWER_LIMIT_VOLTAGE,
      HIGHER_LIMIT_VOLTAGE,
      MAX_TORQUE,
      RETURN_LEVEL,
      ALARM_SHUTDOWN,
      TORQUE_ENABLE,
      LED,
      D_GAIN,
      I_GAIN,
      P_GAIN,
      GOAL_POSITION,
      GOAL_VELOCITY,
      GOAL_TORQUE,
      PUNCH
    } param_t;

    enum {
      BAUD9600 = 0,
      BAUD57600 = 1,
      BAUD115200 = 2,
      BAUD1MBPS = 3
    } BAUD_RATE_;

    typedef enum {
      WHEEL = 1,
      JOIN = 2
    } CONTROL_MODE_;

    typedef enum {
      PING_ONLY = 0,
      PING_READ = 1,
      ALL = 2
    } RETURN_LEVEL_;

    typedef enum {
      DISABLED = 0,
      ENABLED = 1
    } TORQUE_ENABLE_;

    typedef enum {
      OFF = 0,
      RED = 1,
      GREEN = 2,
      YELLOW = 3,
      BLUE = 4,
      PINK = 5,
      BLUE_GREEN = 6,
      WHITE = 7
    } LED_;

    typedef enum {
      NONE = 0,
      SOME = 1
    } REGISTERED_INSTRUCTION_;

    typedef enum {
      FALSE = 0,
      TRUE = 1
    } MOVING_;

    Dynamixel(uint8_t id, uint8_t direction_pin, uint8_t baud_rate = BAUD1MBPS);
    
    bool reboot();
    bool reset();
    bool ping();

    uint16_t read(param_t param);
    bool write(param_t param, uint16_t value);

    uint8_t last_error();

  private:
    typedef enum {
      PING = 0x01,
      READ = 0x02,
      WRITE = 0x03,
      RESET = 0x06,
      REBOOT = 0x08,
      STATUS = 0x55
    } instruction_t;

    typedef union {
      #pragma pack(1)
      struct {
        uint32_t signature;
        uint8_t id;
        uint16_t length;
        uint8_t instruction;
        union {
          struct {
            uint16_t param;
            union {
              uint16_t write_value;
              uint16_t read_len;
            };
          };
          struct {
            uint8_t error;
            uint16_t read_value;
          };
        };
      };
      uint8_t data[15];
    } message_t;

    uint16_t calculate_crc();
    void write_msg();
    bool read_msg();

    static const uint8_t param_offsets[];
    static const uint8_t param_lengths[];
    static const uint32_t baud_rates[];
    static const uint16_t crc_table[];
    static const uint32_t signature = 0x00FDFFFF;
    static message_t msg;

    uint8_t _id;
    uint8_t _direction_pin;
    uint32_t _baud_rate;

    uint8_t _last_error;
};

#endif
