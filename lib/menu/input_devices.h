#ifndef COSMIC_SIGNPOST_LIB_MENU_INPUT_DEVICES_H_
#define COSMIC_SIGNPOST_LIB_MENU_INPUT_DEVICES_H_

#include <memory>
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

  extern int32_t rotaryEncoderCount;
  extern int32_t encoderPinA;
  extern int32_t encoderPinB;
  // A 2-bit number representing the state.
  // pin A uses the high bit (0x2), and pin B uses the low bit (0x1).
  extern volatile int8_t encoderState;
  extern volatile uint64_t lastEncoderTimeMicros;

  extern int32_t selectPin;
  extern volatile bool selectState;
  extern volatile uint64_t lastSelectTimeMicros;

  extern int32_t backPin;
  extern volatile bool backState;
  extern volatile uint64_t lastBackTimeMicros;

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
  void IRAM_ATTR handleRotaryEncoderInterrupt(int32_t pin, int8_t bitMask);
  void IRAM_ATTR handleRotaryEncoderAInterrupt();
  void IRAM_ATTR handleRotaryEncoderBInterrupt();

  void controlMenu(std::shared_ptr<Menu> menu);
  bool processSingleQueueEntry(std::shared_ptr<Menu> menu);
};

#endif
