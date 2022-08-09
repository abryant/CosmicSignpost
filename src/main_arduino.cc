#ifndef UNIT_TEST

#include <memory>
#include <Arduino.h>
#include <Wire.h>

#include <AccelStepper.h>
#include <WiFiManager.h>
#include "freertos/task.h"

#include "input_devices.h"
#include "output_devices.h"
#include "menu.h"
#include "space_pointer_menu.h"

#include "direction_queue.h"
#include "stepper_motors.h"
#include "time_utils.h"

#include "ota.h"
#include "secrets.h"

const int32_t AZIMUTH_STEP_PIN = 2;
const int32_t AZIMUTH_DIR_PIN = 4;
const int32_t ALTITUDE_STEP_PIN = 18;
const int32_t ALTITUDE_DIR_PIN = 19;

const int32_t STEPS_PER_ROTATION = 200*16;

const int32_t I2C_SDA_PIN = 21;
const int32_t I2C_SCL_PIN = 22;

const uint8_t LCD_ADDRESS = 0x72;

const int32_t BUTTON_PIN = 27;

const int32_t ENCODER_A_PIN = 12;
const int32_t ENCODER_B_PIN = 14;
const int32_t ENCODER_BUTTON_PIN = 13;

WiFiManager wifiManager;
std::shared_ptr<Menu> menu;

TaskHandle_t motorControlTaskHandle;
Direction currentDirection;
std::shared_ptr<DirectionQueue> directionQueue;
TimeMillisMicros lastAddedTime;

std::shared_ptr<StepperMotors> motors;

void controlStepperMotors(void *param) {
  (void) param;

  while (true) {
    motors->control();
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

  InputDevices::initRotaryEncoder(ENCODER_A_PIN, ENCODER_B_PIN);
  InputDevices::initSelectButton(ENCODER_BUTTON_PIN);
  InputDevices::initBackButton(BUTTON_PIN);
  OutputDevices::initLcd(LCD_ADDRESS);

  wifiManager.setHostname("spacepointer");
  wifiManager.autoConnect("Space Pointer", WIFI_MANAGER_PASSWORD.c_str());
  ota::setUp(1420, OTA_PASSWORD);

  directionQueue = std::make_shared<DirectionQueue>();

  motors = std::make_shared<StepperMotors>(
      directionQueue,
      AZIMUTH_STEP_PIN,
      AZIMUTH_DIR_PIN,
      ALTITUDE_STEP_PIN,
      ALTITUDE_DIR_PIN);

  xTaskCreatePinnedToCore(
      &controlStepperMotors,
      "ControlMotors",
      /* stackSize= */ 10000,
      /* parameter= */ NULL,
      /* priority= */ 0,
      &motorControlTaskHandle,
      /* core= */ 0);

  currentDirection = Direction(90, 0);
  menu = buildSpacePointerMenu([](int32_t newAngle) {
    currentDirection = Direction(newAngle, 0);
  });
  lastAddedTime = TimeMillisMicros::now();
}

void loop() {
  InputDevices::controlMenu(menu);
  std::string menuText = menu->getDisplayedText();
  // OutputDevices won't send it to the LCD again unless it has changed.
  OutputDevices::display(menuText);

  if (!directionQueue->isFull()) {
    lastAddedTime = lastAddedTime.plusMicros(50000);
    int64_t timeMillis = (lastAddedTime.millis / 50) * 50;
    directionQueue->addDirection(timeMillis, currentDirection);
  }

  ota::checkForOta();
}

#endif
