#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "Wire.h"
#include "Adafruit_LEDBackpack.h"

class Display
{
  public:
    Display(uint8_t addr);

    void setText(char text[4]);
    void setDec(uint16_t dec);
    void setDec2Digits(uint16_t dec);
    void setDec3Digits(uint16_t dec);
    void setHex(uint16_t hex);

  private:
    void update();

  	Adafruit_AlphaNum4 _display;
    uint8_t _addr;
    char _text[4];
    uint8_t _dot;
};

#endif
