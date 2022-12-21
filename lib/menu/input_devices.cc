#include "input_devices.h"

#include <Arduino.h>

InputDevices::InputButton InputDevices::inputQueue[INPUT_BUFFER_SIZE];
size_t InputDevices::inputQueueReadPos;
size_t InputDevices::inputQueueWritePos;
portMUX_TYPE InputDevices::inputQueueMux = portMUX_INITIALIZER_UNLOCKED;

int32_t InputDevices::rotaryEncoderCount;
int32_t InputDevices::encoderPinA;
int32_t InputDevices::encoderPinB;
volatile int8_t InputDevices::encoderState;
volatile uint64_t InputDevices::lastEncoderTimeMicros;

int32_t InputDevices::selectPin;
volatile bool InputDevices::selectState;
volatile uint64_t InputDevices::lastSelectTimeMicros;

int32_t InputDevices::backPin;
volatile bool InputDevices::backState;
volatile uint64_t InputDevices::lastBackTimeMicros;

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
  InputDevices::InputButton result = inputQueue[inputQueueReadPos];
  inputQueueReadPos = (inputQueueReadPos + 1) % INPUT_BUFFER_SIZE;
  return result;
}


// A lookup table for finding the change in rotary encoder count when a given state change happens.
// Rows represent the old state, columns represent the new state, and values represent the change
// in count. The state is encoded as 2*a + b where a and b are input bits.
// For a clockwise rotation, we expect B to precede A, so for a full rotation we'll get 01320
// For an anticlockwise rotation, we expect A to precede B, so for a full rotation we'll get 02310
// For changes that we don't expect to occur, we record a zero, and hope that the other state
// changes give us more information. In the end we only need to detect whether the count is
// positive or negative when both inputs are low.
const int8_t ROTARY_ENCODER_COUNT_CHANGES[4][4] = {
  { 0,  1, -1,  0},
  {-1,  0,  0,  1},
  { 1,  0,  0, -1},
  { 0, -1,  1,  0}
};

void InputDevices::initRotaryEncoder(int32_t pinA, int32_t pinB) {
  rotaryEncoderCount = 0;
  lastEncoderTimeMicros = 0;
  encoderPinA = pinA;
  encoderPinB = pinB;
  encoderState = 0;
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  // Trigger interrupts on HIGH/LOW instead of RISING/FALLING/CHANGE, as the latter sometimes miss
  // edges, see this ESP32 issue: https://github.com/espressif/esp-idf/issues/7602 and section 3.14 in:
  // https://www.espressif.com/sites/default/files/documentation/eco_and_workarounds_for_bugs_in_esp32_en.pdf
  //
  // Rotary encoder signals are inverted, and pulled up to high. To begin with, we want to trigger
  // whenever they are low, but the interrupt will toggle the type to track changes.
  attachInterrupt(digitalPinToInterrupt(pinA), InputDevices::handleRotaryEncoderAInterrupt, ONLOW);
  attachInterrupt(digitalPinToInterrupt(pinB), InputDevices::handleRotaryEncoderBInterrupt, ONLOW);
}

void IRAM_ATTR InputDevices::handleRotaryEncoderInterrupt(int32_t pin, int8_t bitMask) {
  portENTER_CRITICAL_ISR(&InputDevices::inputQueueMux);
  bool toggleInterrupt = false;
  uint64_t time = micros();
  // Debounce with 10 microseconds. It seems that maybe toggling the interrupt type from low to
  // high or vice versa can cause another interrupt. We don't want an infinite loop of interrupts,
  // as it makes the ESP32 reset itself with the WDT (watchdog timer), so we need to wait between
  // interrupt triggers.
  if (time - lastEncoderTimeMicros >= 10) {
    int8_t newState = encoderState ^ bitMask;
    toggleInterrupt = true;
    int8_t diff = ROTARY_ENCODER_COUNT_CHANGES[encoderState][newState];
    rotaryEncoderCount += diff;
    if (newState == 0 && rotaryEncoderCount != 0) {
      addToInputQueue(rotaryEncoderCount > 0
                      ? InputDevices::InputButton::CLOCKWISE
                      : InputDevices::InputButton::ANTICLOCKWISE);
      rotaryEncoderCount = 0;
    }
    encoderState = newState;
    lastEncoderTimeMicros = time;
  }
  portEXIT_CRITICAL_ISR(&InputDevices::inputQueueMux);

  if (toggleInterrupt) {
    // We want to flip the type of interrupt, so that if we're currently seeing a low signal we
    // trigger on a high one. The encoderState uses high bits to represent low GPIO pin values,
    // so when the bit is high we expect the pin to be low, and want to trigger when it gets to
    // high again.
    gpio_set_intr_type(
        (gpio_num_t) pin,
        (encoderState & bitMask) ? GPIO_INTR_HIGH_LEVEL : GPIO_INTR_LOW_LEVEL);
  }
}

void IRAM_ATTR InputDevices::handleRotaryEncoderAInterrupt() {
  handleRotaryEncoderInterrupt(encoderPinA, 0x2);
}

void IRAM_ATTR InputDevices::handleRotaryEncoderBInterrupt() {
  handleRotaryEncoderInterrupt(encoderPinB, 0x1);
}

void InputDevices::initSelectButton(int32_t pin) {
  selectPin = pin;
  selectState = false;
  lastSelectTimeMicros = 0;
  pinMode(selectPin, INPUT);
  // Trigger interrupts on HIGH/LOW instead of RISING/FALLING/CHANGE, as the latter sometimes miss
  // edges, see this ESP32 issue: https://github.com/espressif/esp-idf/issues/7602 and section 3.14 in:
  // https://www.espressif.com/sites/default/files/documentation/eco_and_workarounds_for_bugs_in_esp32_en.pdf
  attachInterrupt(digitalPinToInterrupt(selectPin), InputDevices::handleSelectButtonInterrupt, ONHIGH);
}

void IRAM_ATTR InputDevices::handleSelectButtonInterrupt() {
  portENTER_CRITICAL_ISR(&InputDevices::inputQueueMux);
  uint64_t time = micros();
  bool toggleInterrupt = false;
  // We need to use a software debounce even if there is a hardware debounce circuit, because the
  // ESP32 treats intermediate voltages (between VIL and VIH) as undefined, and sees random digital
  // values when the voltage is in that range. This causes both interrupts and digitalRead() to
  // misbehave. See https://github.com/espressif/esp-idf/issues/7602.
  if (time - lastSelectTimeMicros >= 10000) {
    selectState = !selectState;
    toggleInterrupt = true;
    // Trigger the select button on press, not release.
    if (selectState) {
      addToInputQueue(InputDevices::InputButton::SELECT);
    }
    lastSelectTimeMicros = time;
  }
  portEXIT_CRITICAL_ISR(&InputDevices::inputQueueMux);

  // gpio_set_intr_type has its own critical section, so we can't toggle interrupts during the
  // critical section above. However, this should work even if the interrupt gets interrupted by
  // another instance of itself, because the debounce will prevent the second one from toggling
  // anything.
  // Even if it did, the state and the interrupt type would still be in sync unless we got
  // interrupted between reading selectState and calling gpio_set_intr_type, and in that rare case
  // it would correct itself after the next interrupt.
  if (toggleInterrupt) {
    gpio_set_intr_type((gpio_num_t) selectPin, selectState ? GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL);
  }
}

void InputDevices::initBackButton(int32_t pin) {
  backPin = pin;
  backState = false;
  lastBackTimeMicros = 0;
  pinMode(backPin, INPUT);
  // Trigger interrupts on HIGH/LOW instead of RISING/FALLING/CHANGE, as the latter sometimes miss
  // edges, see this ESP32 issue: https://github.com/espressif/esp-idf/issues/7602 and section 3.14 in:
  // https://www.espressif.com/sites/default/files/documentation/eco_and_workarounds_for_bugs_in_esp32_en.pdf
  attachInterrupt(digitalPinToInterrupt(backPin), InputDevices::handleBackButtonInterrupt, ONHIGH);
}

void IRAM_ATTR InputDevices::handleBackButtonInterrupt() {
  portENTER_CRITICAL_ISR(&InputDevices::inputQueueMux);
  uint64_t time = micros();
  bool toggleInterrupt = false;
  // We need to use a software debounce even if there is a hardware debounce circuit, because the
  // ESP32 treats intermediate voltages (between VIL and VIH) as undefined, and sees random digital
  // values when the voltage is in that range. This causes both interrupts and digitalRead() to
  // misbehave. See https://github.com/espressif/esp-idf/issues/7602.
  if (time - lastBackTimeMicros >= 10000) {
    backState = !backState;
    toggleInterrupt = true;
    // Trigger the back button on release, not press.
    if (!backState) {
      addToInputQueue(InputDevices::InputButton::BACK);
    }
    lastBackTimeMicros = time;
  }
  portEXIT_CRITICAL_ISR(&InputDevices::inputQueueMux);

  // gpio_set_intr_type has its own critical section, so we can't toggle interrupts during the
  // critical section above. However, this should work even if the interrupt gets interrupted by
  // another instance of itself, because the debounce will prevent the second one from toggling
  // anything.
  // Even if it did, the state and the interrupt type would still be in sync unless we got
  // interrupted between reading selectState and calling gpio_set_intr_type, and in that rare case
  // it would correct itself after the next interrupt.
  if (toggleInterrupt) {
    gpio_set_intr_type((gpio_num_t) backPin, backState ? GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL);
  }
}

void InputDevices::controlMenu(std::shared_ptr<Menu> menu) {
  bool done = false;
  while (!done) {
    done = processSingleQueueEntry(menu);
  }
}

bool InputDevices::processSingleQueueEntry(std::shared_ptr<Menu> menu) {
  bool empty = true;
  InputDevices::InputButton button = InputDevices::InputButton::BACK;
  portENTER_CRITICAL(&InputDevices::inputQueueMux);
  empty = inputQueueIsEmpty();
  if (!empty) {
    button = readFromInputQueue();
  }
  portEXIT_CRITICAL(&InputDevices::inputQueueMux);

  if (!empty) {
    switch (button) {
      case SELECT:
        menu->onSelect();
        break;
      case BACK:
        menu->onBack();
        break;
      case CLOCKWISE:
        menu->onRotateClockwise();
        break;
      case ANTICLOCKWISE:
        menu->onRotateAnticlockwise();
        break;
    }
  }
  return empty;
}
