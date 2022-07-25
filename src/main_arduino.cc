#ifndef UNIT_TEST

#include <memory>
#include <Arduino.h>
#include <Wire.h>

#include <AccelStepper.h>
#include <WiFiManager.h>
#include <ESP32Encoder.h>

#include "input_devices.h"
#include "output_devices.h"
#include "menu.h"
#include "space_pointer_menu.h"

#include "ota.h"
#include "secrets.h"

const int32_t AZIMUTH_STEP_PIN = 2;
const int32_t AZIMUTH_DIR_PIN = 4;
const int32_t ALTITUDE_DIR_PIN = 19;
const int32_t ALTITUDE_STEP_PIN = 18;

const int32_t STEPS_PER_ROTATION = 200*16;

const int32_t I2C_SDA_PIN = 21;
const int32_t I2C_SCL_PIN = 22;

const uint8_t LCD_ADDRESS = 0x72;

const int32_t BUTTON_PIN = 27;

const int32_t ENCODER_A_PIN = 12;
const int32_t ENCODER_B_PIN = 14;
const int32_t ENCODER_BUTTON_PIN = 13;

WiFiManager wifiManager;
AccelStepper stepper = AccelStepper(AccelStepper::DRIVER, AZIMUTH_STEP_PIN, AZIMUTH_DIR_PIN);
std::shared_ptr<Menu> menu;
std::string lastMenuText = "";

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

  menu = buildSpacePointerMenu();
  InputDevices::initRotaryEncoder(ENCODER_A_PIN, ENCODER_B_PIN);
  InputDevices::initSelectButton(ENCODER_BUTTON_PIN);
  InputDevices::initBackButton(BUTTON_PIN);

  OutputDevices::initLcd(LCD_ADDRESS);

  wifiManager.setHostname("spacepointer");
  wifiManager.autoConnect("Space Pointer", WIFI_MANAGER_PASSWORD.c_str());
  ota::setUp(1420, OTA_PASSWORD);

  stepper.setMaxSpeed(5000.0);
  stepper.setAcceleration(1000.0);
}

void loop() {
  if (!stepper.isRunning()) {
    stepper.moveTo(stepper.currentPosition() + (STEPS_PER_ROTATION * 5));
    delay(1000);
  }
  InputDevices::controlMenu(*menu);
  std::string menuText = menu->getDisplayedText();
  if (menuText != lastMenuText) {
    lastMenuText = menuText;
    OutputDevices::display(menuText);
  }
  stepper.run();
  ota::checkForOta();
}

#endif
