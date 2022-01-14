#pragma once

#include <Arduino.h>

class ButtonState
{
  uint8_t pinNum;
  uint8_t state;
  uint32_t newStateCount;
  uint32_t errCount;
  uint32_t btnCount[4];
  uint16_t adcRefs[5];
  uint16_t lookUpLongKey;
  uint16_t lookUpValue;
  uint16_t lookUpOffset;
  bool cruise;
  uint32_t prevMillis;
  void (*handler)(uint8_t, uint8_t);

  public:
  ButtonState(uint8_t pinNum, uint16_t adcRefs[5], uint16_t longKeyTime, uint16_t offset, bool cruise, void (*handler)(uint8_t, uint8_t));

  void update();
};