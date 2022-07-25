#include "input_devices.h"

#include <Arduino.h>

InputDevices::InputButton InputDevices::inputQueue[INPUT_BUFFER_SIZE];
size_t InputDevices::inputQueueReadPos;
size_t InputDevices::inputQueueWritePos;
portMUX_TYPE InputDevices::inputQueueMux = portMUX_INITIALIZER_UNLOCKED;

int8_t InputDevices::rotaryEncoderOldState;
int32_t InputDevices::rotaryEncoderCount;
int32_t InputDevices::encoderPinA;
int32_t InputDevices::encoderPinB;

void InputDevices::addToInputQueue(InputDevices::InputButton button) {
  size_t newWritePos = (inputQueueWritePos + 1) % INPUT_BUFFER_SIZE;
  if (newWritePos == inputQueueReadPos) {
    // Queue is full, fail to write.
    return;
  }
  inputQueue[inputQueueWritePos] = button;
  inputQueueWritePos = newWritePos;
}

bool InputDevices::inputQueueIsEmpty() {
  return inputQueueReadPos == inputQueueWritePos;
}

InputDevices::InputButton InputDevices::readFromInputQueue() {
  InputButton result = inputQueue[inputQueueReadPos];
  inputQueueReadPos = (inputQueueReadPos + 1) % INPUT_BUFFER_SIZE;
  return result;
}


// A lookup table for finding the change in rotary encoder count when a given state change happens.
// Rows represent the old state, columns represent the new state, and values represent the change
// in count. The state is encoded as 2*a + b where a and b are input bits.
// For a clockwise rotation, we expect A to precede B, so for a full rotation we'll get 02310
// For an anticlockwise rotation, we expect B to precede A, so for a full rotation we'll get 01320
// For changes that we don't expect to occur, we record a zero, and hope that the other state
// changes give us more information. In the end we only need to detect whether the count is
// positive or negative when both inputs are low.
const int8_t ROTARY_ENCODER_COUNT_CHANGES[4][4] = {
  { 0, -1,  1,  0},
  { 1,  0,  0, -1},
  {-1,  0,  0,  1},
  { 0,  1, -1,  0}
};

void InputDevices::initRotaryEncoder(int32_t pinA, int32_t pinB) {
  rotaryEncoderCount = 0;
  encoderPinA = pinA;
  encoderPinB = pinB;
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinA), InputDevices::handleRotaryEncoderInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pinB), InputDevices::handleRotaryEncoderInterrupt, CHANGE);
}

void IRAM_ATTR InputDevices::handleRotaryEncoderInterrupt() {
  portENTER_CRITICAL_ISR(&InputDevices::inputQueueMux);
  bool a = digitalRead(encoderPinA) == HIGH;
  bool b = digitalRead(encoderPinB) == HIGH;
  int8_t newState = (a ? 2 : 0) | (b ? 1 : 0);
  int8_t diff = ROTARY_ENCODER_COUNT_CHANGES[rotaryEncoderOldState][newState];
  rotaryEncoderCount += diff;
  if (rotaryEncoderOldState != 0 && newState == 0) {
    addToInputQueue(
        rotaryEncoderCount > 0
        ? InputDevices::InputButton::CLOCKWISE
        : InputDevices::InputButton::ANTICLOCKWISE);
    rotaryEncoderCount = 0;
  }
  rotaryEncoderOldState = newState;
  portEXIT_CRITICAL_ISR(&InputDevices::inputQueueMux);
}

void InputDevices::initSelectButton(int32_t pin) {
  pinMode(pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(pin), InputDevices::handleSelectButtonInterrupt, RISING);
}

void IRAM_ATTR InputDevices::handleSelectButtonInterrupt() {
  portENTER_CRITICAL_ISR(&InputDevices::inputQueueMux);
  addToInputQueue(InputDevices::InputButton::SELECT);
  portEXIT_CRITICAL_ISR(&InputDevices::inputQueueMux);
}

void InputDevices::initBackButton(int32_t pin) {
  pinMode(pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(pin), InputDevices::handleBackButtonInterrupt, FALLING);
}

void IRAM_ATTR InputDevices::handleBackButtonInterrupt() {
  portENTER_CRITICAL_ISR(&InputDevices::inputQueueMux);
  addToInputQueue(InputDevices::InputButton::BACK);
  portEXIT_CRITICAL_ISR(&InputDevices::inputQueueMux);
}

void InputDevices::controlMenu(Menu &menu) {
  bool done = false;
  while (!done) {
    done = processSingleQueueEntry(menu);
  }
}

bool InputDevices::processSingleQueueEntry(Menu &menu) {
  bool empty = true;
  InputDevices::InputButton button = InputDevices::InputButton::SELECT;
  portENTER_CRITICAL(&InputDevices::inputQueueMux);
  empty = inputQueueIsEmpty();
  if (!empty) {
    button = readFromInputQueue();
  }
  portEXIT_CRITICAL(&InputDevices::inputQueueMux);

  if (!empty) {
    switch (button) {
      case SELECT:
        menu.onSelect();
        break;
      case BACK:
        menu.onBack();
        break;
      case CLOCKWISE:
        menu.onRotateClockwise();
        break;
      case ANTICLOCKWISE:
        menu.onRotateAnticlockwise();
        break;
    }
  }
  return empty;
}
