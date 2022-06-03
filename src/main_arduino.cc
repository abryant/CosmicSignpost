#ifndef UNIT_TEST

#include <Arduino.h>

#include <AccelStepper.h>
#include <WiFiManager.h>

#include "ota.h"
#include "secrets.h"

const int32_t STEP_PIN = 2;
const int32_t DIR_PIN = 4;
const int32_t STEPS_PER_ROTATION = 200*16;

WiFiManager wifiManager;
AccelStepper stepper = AccelStepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

int64_t lastTime;

void setup() {
  Serial.begin(115200);
  wifiManager.setHostname("spacepointer");
  wifiManager.autoConnect("Space Pointer", WIFI_MANAGER_PASSWORD.c_str());
  ota::setUp(1420, OTA_PASSWORD);

  stepper.setMaxSpeed(5000.0);
  stepper.setAcceleration(1000.0);
  lastTime = millis();
}

void loop() {
  if (!stepper.isRunning()) {
    stepper.moveTo(stepper.currentPosition() + (STEPS_PER_ROTATION * 5));
    delay(1000);
  }
  stepper.run();
  ota::checkForOta();
}

#endif
