#ifndef UNIT_TEST

#include <memory>
#include <Arduino.h>
#include <Wire.h>
#include <HTTPClient.h>

#include <WiFiManager.h>
#include "freertos/task.h"

#include "async_queue.h"
#include "input_devices.h"
#include "output_devices.h"
#include "menu.h"
#include "main_menu.h"

#include "cartesian_location.h"
#include "direction_queue.h"
#include "equatorial_location.h"
#include "satellite_orbit.h"
#include "moon_orbit.h"
#include "stepper_motors.h"
#include "time_utils.h"
#include "tracker.h"
#include "vector.h"

#include "config.h"
#include "gps.h"
#include "orientation.h"
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
Tracker tracker(
  Location(51.500804, -0.124340, 10),
  Direction(0, 0),
  [](int64_t timeMillis) {
      return CartesianLocation::fixed(Vector(0, 0, 0));
  });

TaskHandle_t motorControlTaskHandle;
std::shared_ptr<DirectionQueue> directionQueue;
TimeMillisMicros lastAddedTime;

std::shared_ptr<StepperMotors> motors;

void waitForTime() {
  int year = 0;
  while (year < 2000) {
    std::time_t time = std::time(nullptr);
    std::tm *utcTime = std::gmtime(&time);
    year = utcTime->tm_year + 1900;
    delay(10);
  }
}

// Required for fetching satellite information from Celestrak.
std::optional<std::string> fetchUrl(std::string url) {
  Serial.println(("Fetching " + url).c_str());
  HTTPClient http;
  http.begin(url.c_str());
  http.GET();
  std::string payload = std::string(http.getString().c_str());
  return std::optional(payload);
}

void initSerialComms() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
}

void initInputDevices() {
  InputDevices::initRotaryEncoder(ENCODER_A_PIN, ENCODER_B_PIN);
  InputDevices::initSelectButton(ENCODER_BUTTON_PIN);
  InputDevices::initBackButton(BUTTON_PIN);
}

void initOutputDevices() {
  OutputDevices::initLcd(LCD_ADDRESS);
  OutputDevices::displayAndSetSplashScreen("Cosmic Signpost");
  delay(500);
}

void initWifi() {
  OutputDevices::display("Connecting to\nWIFI...");
  wifiManager.setHostname("cosmicsignpost");
  wifiManager.autoConnect("Cosmic Signpost", WIFI_MANAGER_PASSWORD.c_str());
}

void initOta() {
  ota::setUp(1420, OTA_PASSWORD);
}

void initNetworkTime() {
  // Set the timezone to UTC, as some time parsing functions depend on that.
  setenv("TZ", "", 1);
  // Get the current time from an NTP server.
  configTime(0, 0, "pool.ntp.org");
  waitForTime();
}

void initGps() {
  gps::initGps(
    [](Location location) {
      Serial.printf("GPS update: lat=%f, long=%f, alt=%f\n", location.getLatitude(), location.getLongitude(), location.getElevation());
      tracker.setCurrentLocation(location);
    },
    main_menu::updateGpsMenuEntry);
}

void initTracking() {
  tracker.setCurrentLocation(config::readDefaultLocation());

  OutputDevices::display("Downloading ISS\norbit data...");
  Serial.println("Initializing ISS orbit...");
  SatelliteOrbit &issOrbit = TrackableObjects::getSatelliteOrbit("ISS");
  bool initialized = issOrbit.fetchElements(fetchUrl);
  if (initialized) {
    tracker.setTrackingFunction(TrackableObjects::getTrackingFunction("ISS"));
    Serial.println("Done.");
  } else {
    Serial.println("Failed to get ISS satellite data.");
  }

  directionQueue = std::make_shared<DirectionQueue>();
  lastAddedTime = TimeMillisMicros::now();
}

void controlStepperMotors(void *param) {
  (void) param;

  while (true) {
    motors->control();
  }
}

void initMotors() {
  motors = std::make_shared<StepperMotors>(
      directionQueue,
      AZIMUTH_STEP_PIN,
      AZIMUTH_DIR_PIN,
      ALTITUDE_STEP_PIN,
      ALTITUDE_DIR_PIN);
  orientation::calibration::motors = motors;

  xTaskCreatePinnedToCore(
      &controlStepperMotors,
      "ControlMotors",
      /* stackSize= */ 10000,
      /* parameter= */ NULL,
      /* priority= */ 0,
      &motorControlTaskHandle,
      /* core= */ 0);
}

void calibrateOrientation() {
  orientation::init();
  orientation::calibration::startCalibration(tracker);
  OutputDevices::display("Calibrating...");

  while (orientation::calibration::isCalibrating()) {
    if (!directionQueue->isFull()) {
      lastAddedTime = lastAddedTime.plusMicros(50000);
      int64_t timeMillis = (lastAddedTime.millis / 50) * 50;
      directionQueue->addDirection(timeMillis, tracker.getDirectionAt(timeMillis));
    }

    ota::checkForOta();
  }

  orientation::calibration::stopCalibration(tracker);
}

void initMenu() {
  menu = main_menu::buildMainMenu(tracker, fetchUrl);
}

void setup() {
  initSerialComms();
  initInputDevices();
  initOutputDevices();
  initWifi();
  initOta();
  initNetworkTime();
  initTracking();
  initMotors();
  calibrateOrientation();
  initMenu();
  initGps();
}

void loop() {
  InputDevices::controlMenu(menu);
  std::string menuText = menu->getDisplayedText();
  // OutputDevices won't send it to the LCD again unless it has changed.
  OutputDevices::display(menuText);
  AsyncQueue::runQueue();

  if (!directionQueue->isFull()) {
    lastAddedTime = lastAddedTime.plusMicros(50000);
    int64_t timeMillis = (lastAddedTime.millis / 50) * 50;
    directionQueue->addDirection(timeMillis, tracker.getDirectionAt(timeMillis));
  }

  gps::checkForUpdates();

  ota::checkForOta();
}

#endif
