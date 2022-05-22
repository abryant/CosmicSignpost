#include "ota.h"

#include <stdlib.h>

#include <string>

#include <ArduinoOTA.h>

void otaStart() {
  String command = ArduinoOTA.getCommand() == U_FLASH ? "flash" : "filesystem";
  Serial.println("Starting update: " + ArduinoOTA.getCommand());
}

void otaEnd() {
  Serial.println("Finished update");
}

void otaProgress(unsigned int progress, unsigned int total) {
  Serial.printf("Progress: %u%%\n", (int) floor(100.0 * progress / total));
}

void otaError(ota_error_t error) {
  Serial.printf("OTA Error[%u]: ", error);
  switch (error) {
    case OTA_AUTH_ERROR: Serial.println("Auth Failed");
    case OTA_BEGIN_ERROR: Serial.println("Begin Failed");
    case OTA_CONNECT_ERROR: Serial.println("Connect Failed");
    case OTA_RECEIVE_ERROR: Serial.println("Receive Failed");
    case OTA_END_ERROR: Serial.println("End Failed");
    default: Serial.println("Unknown");
  }
}

void ota::setUp(int32_t port, std::string password) {
  ArduinoOTA
      .onStart(otaStart)
      .onEnd(otaEnd)
      .onProgress(otaProgress)
      .onError(otaError)
      .setHostname(WiFi.getHostname())
      .setPort(port)
      .setPassword(password.c_str())
      .begin();
}

void ota::checkForOta() {
  ArduinoOTA.handle();
}