#if defined(ARDUINO)

#include <Arduino.h>
#include <WiFiManager.h>

#include "ota.h"
#include "secrets.h"

WiFiManager wifiManager;

void setup() {
  // To match test_speed in platformio.ini
  Serial.begin(115200);
  wifiManager.setHostname("cosmicsignpost");
  wifiManager.autoConnect("Cosmic Signpost", WIFI_MANAGER_PASSWORD.c_str());
  ota::setUp(1420, OTA_PASSWORD);

  // Set the timezone to UTC, as some time parsing functions depend on that.
  setenv("TZ", "", 1);

  // Give 2 seconds for the test runner to connect to the board.
  delay(2000);

  ::testing::InitGoogleTest();
  if (RUN_ALL_TESTS());
}

void loop() {
  ota::checkForOta();
}

#else

int main(int argc, char **argv) {
  // Set the timezone to UTC, as some time parsing functions depend on that.
  setenv("TZ", "", 1);

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

#endif
