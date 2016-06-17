

#include "Adafruit_LEDBackpack.h"
#include "Battery.h"
#include "Button.h"
#include "Cutter.h"
#include "Display.h"
#include "Dynamixel.h"
#include "EEPROM.h"
#include "Extruder.h"
#include "Selector.h"
#include "Wire.h"

typedef enum {STANDBY, TUBE_A, TUBE_B, TUBE_C, DIAGONAL_AA, DIAGONAL_AB, DIAGONAL_AC, DIAGONAL_BC, DIAGONAL_ABC, COUNTER_TAPE, COUNTER_STRAIGHT, COUNTER_CONNECTOR, BATTERY} Tool;
char *texts[13] = {"IDLE", "A   ", "B   ", "C   ", "AA  ", "AB  ", "AC  ", "BC  ", "D3  ", "0000", "S   ", "C   ", "BATT"};
float memory[9] = {0, 30, 50, 100, 42, 58, 104, 112, 116};
Tool tool = STANDBY;

Battery battery(A0);
Button cutButton(A3);
Button extrudeButton(4);
Cutter cutter(2, 2, 3, 5, &battery);
Display display(0x70);
Extruder extruder(3, 2);
Selector selector(6, 11);

void setup()
{
  selector.onChanged(selectorChange);

  battery.on(Battery::LowVoltage, batteryLowVoltage);
  battery.on(Battery::HighVoltage, batteryHighVoltage);

  cutButton.on(Button::Push, cutPush);
  cutButton.on(Button::DoublePush, cutDoublePush);
  cutButton.on(Button::TriplePush, cutTriplePush);

  extrudeButton.on(Button::Push, extrudePush);
  extrudeButton.on(Button::DoublePush, extrudeDoublePush);
  extrudeButton.on(Button::LongPushBegin, extrudeLongPushBegin);
  extrudeButton.on(Button::LongPushEnd, extrudeLongPushEnd);
}

void loop()
{
  battery.process();
  
  if(tool != BATTERY) {
    cutButton.process();
    extrudeButton.process();
    extruder.process();
    selector.process();
  }
}

// CALLBACKS //

void selectorChange(uint8_t status)
{
  tool = (Tool)status;
  display.setText(texts[tool]);
  enter();
}

void batteryLowVoltage()
{
  selectorChange(BATTERY);
}

void batteryHighVoltage()
{
  selector.emit();
}

void extruderLengthChangeTube(uint16_t length)
{
  display.setDec3Digits(length + 8);
}

void extruderLengthChangeDiagonal(uint16_t length)
{
  display.setDec2Digits(length + 8);
}

void enter()
{
  switch(tool) {
    case STANDBY:
    case BATTERY:
      cutter.suspend();
      extruder.onLengthChanged(NULL);
      break;
    case TUBE_A:
    case TUBE_B:
    case TUBE_C:
      cutter.prepare();
      extruder.reset();
      extruder.onLengthChanged(extruderLengthChangeTube);
      display.setDec3Digits(memory[tool]);
      break;
    case DIAGONAL_AA:
    case DIAGONAL_AB:
    case DIAGONAL_AC:
    case DIAGONAL_BC:
    case DIAGONAL_ABC:
      cutter.prepare();
      extruder.reset();
      extruder.onLengthChanged(extruderLengthChangeDiagonal);
      display.setDec2Digits(memory[tool]);
      break;
    case COUNTER_STRAIGHT:
    case COUNTER_CONNECTOR:
      cutter.suspend();
      extruder.onLengthChanged(NULL);
      display.setDec3Digits(cutter.readCounter(Cutter::Cut(tool - COUNTER_STRAIGHT)));
      break;
    case COUNTER_TAPE:
      cutter.suspend();
      extruder.onLengthChanged(NULL);
      display.setDec(extruder.totalLength());
      break;
  }
}

void cutPush()
{
  switch(tool) {
    case STANDBY:
      cutter.clean();
      break;
    case TUBE_A:
    case TUBE_B:
    case TUBE_C:
    case DIAGONAL_AA:
    case DIAGONAL_AB:
    case DIAGONAL_AC:
    case DIAGONAL_BC:
    case DIAGONAL_ABC:
      extruder.start(8.0, true);
      break;
  }
}

void cutDoublePush()
{
  switch(tool) {
    case TUBE_A:
    case TUBE_B:
    case TUBE_C:
    case DIAGONAL_AA:
    case DIAGONAL_AB:
    case DIAGONAL_AC:
    case DIAGONAL_BC:
    case DIAGONAL_ABC:
      extruder.start(2.5, true);
      cutter.cut(Cutter::Straight);
      extruder.start(5.5, true);
      break;
  }
}

void cutTriplePush()
{
  switch(tool) {
    case TUBE_A:
    case TUBE_B:
    case TUBE_C:
    case DIAGONAL_AA:
    case DIAGONAL_AB:
    case DIAGONAL_AC:
    case DIAGONAL_BC:
    case DIAGONAL_ABC:
      extruder.start(5.0, true);
      cutter.cut(Cutter::Connector);
      extruder.start(8.0, true);
      break;
    case COUNTER_STRAIGHT:
    case COUNTER_CONNECTOR:
      cutter.resetCounters();
      display.setDec3Digits(0);
      break;
    case COUNTER_TAPE:
      extruder.resetCounter();
      display.setDec3Digits(0);
      break;
  }
}

void extrudePush()
{
  extruder.stop();
}

void extrudeDoublePush()
{
  switch(tool) {
    case TUBE_A:
    case TUBE_B:
    case TUBE_C:
    case DIAGONAL_AA:
    case DIAGONAL_AB:
    case DIAGONAL_AC:
    case DIAGONAL_BC:
    case DIAGONAL_ABC:
      extruder.reset();
      extruder.start(memory[tool] - 8);
      break;
  }
}

void extrudeLongPushBegin()
{
  switch(tool) {
    case STANDBY:
      extruder.reverse();
      break;
    case TUBE_A:
    case TUBE_B:
    case TUBE_C:
      extruder.start(0);
      break;
  }
}

void extrudeLongPushEnd()
{
  extruder.stop();
  
  switch(tool) {
    case TUBE_A:
    case TUBE_B:
    case TUBE_C:
      memory[tool] = extruder.length() + 8;
      memory[DIAGONAL_AA] = sqrt(pow(memory[TUBE_A], 2) * 2) - 1;
      memory[DIAGONAL_AB] = sqrt(pow(memory[TUBE_A], 2) + pow(memory[TUBE_B], 2)) - 1;
      memory[DIAGONAL_AC] = sqrt(pow(memory[TUBE_A], 2) + pow(memory[TUBE_C], 2)) - 1;
      memory[DIAGONAL_BC] = sqrt(pow(memory[TUBE_B], 2) + pow(memory[TUBE_C], 2)) - 1;
      memory[DIAGONAL_ABC] = sqrt(pow(memory[TUBE_A], 2) + pow(memory[TUBE_B], 2) + pow(memory[TUBE_C], 2)) - 1;
      break;
  }
}

