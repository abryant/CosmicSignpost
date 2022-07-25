#ifndef SPACEPOINTER_LIB_MENU_INPUT_DEVICES_H_
#define SPACEPOINTER_LIB_MENU_INPUT_DEVICES_H_

#include <stdint.h>

#include <Arduino.h>

#include "menu.h"

const size_t INPUT_BUFFER_SIZE = 50;

namespace InputDevices {
  enum InputButton {
    SELECT,
    BACK,
    CLOCKWISE,
    ANTICLOCKWISE,
  };

  extern InputButton inputQueue[INPUT_BUFFER_SIZE];
  extern size_t inputQueueReadPos;
  extern size_t inputQueueWritePos;
  extern portMUX_TYPE inputQueueMux;

  extern int8_t rotaryEncoderOldState;
  extern int32_t rotaryEncoderCount;
  extern int32_t encoderPinA;
  extern int32_t encoderPinB;

  // Input queue functions, for internal use only.
  // The caller must hold inputQueueMux while calling these.
  void addToInputQueue(InputButton button);
  bool inputQueueIsEmpty();
  InputButton readFromInputQueue();

  void initRotaryEncoder(int32_t pinA, int32_t pinB);
  void initSelectButton(int32_t pin);
  void initBackButton(int32_t pin);

  void IRAM_ATTR handleSelectButtonInterrupt();
  void IRAM_ATTR handleBackButtonInterrupt();
  void IRAM_ATTR handleRotaryEncoderInterrupt();

  void controlMenu(Menu &menu);
  bool processSingleQueueEntry(Menu &menu);
};

#endif
