#include "Display.h"

#define DEC_CHAR(digit) '0' + (digit)
#define HEX_CHAR(digit) (digit) >= 10 ? 'A' - 10 + (digit) : '0' + (digit)

Display::Display(uint8_t addr) :
  _addr(addr),
  _dot(-1)
{
}

void Display::setText(char text[4])
{
  memcpy(_text, text, 4);
  _dot = -1;

  update();
}

void Display::setDec(uint16_t dec)
{
  _text[0] = DEC_CHAR((dec / 1000) % 10);
  _text[1] = DEC_CHAR((dec / 100) % 10);
  _text[2] = DEC_CHAR((dec / 10) % 10);
  _text[3] = DEC_CHAR(dec % 10);
  _dot = -1;

  update();
}

void Display::setDec2Digits(uint16_t dec)
{
  if(dec >= 100) {
    _text[2] = DEC_CHAR((dec / 100) % 10);
    _text[3] = DEC_CHAR((dec / 10) % 10);
    _dot = 2;
  } else {
    _text[2] = DEC_CHAR((dec / 10) % 10);
    _text[3] = DEC_CHAR(dec % 10);
    _dot = 1;
  }

  update();
}

void Display::setDec3Digits(uint16_t dec)
{
  _text[1] = DEC_CHAR((dec / 100) % 10);
  _text[2] = DEC_CHAR((dec / 10) % 10);
  _text[3] = DEC_CHAR(dec % 10);
  _dot = 1;

  update();
}

void Display::setHex(uint16_t hex)
{
  if(hex <= 0xFF) {
    _text[0] = '0';
    _text[1] = 'x';
  } else {
    _text[0] = HEX_CHAR(hex >> 12);
    _text[1] = HEX_CHAR((hex >> 8) % 16);
  }

  _text[2] = HEX_CHAR((hex >> 4) % 16);
  _text[3] = HEX_CHAR(hex % 16);

  _dot = -1;

  update();
}

void Display::update()
{
  _display.begin(_addr);

  for(int i=0; i<4; i++) {
    _display.writeDigitAscii(i, _text[i], _dot == i);
  }
 
  _display.writeDisplay();
}
