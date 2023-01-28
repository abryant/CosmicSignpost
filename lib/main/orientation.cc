#include "orientation.h"

#include <Arduino.h>

Adafruit_BNO055 orientation::sensor = Adafruit_BNO055();
bool orientation::connected = false;

adafruit_bno055_offsets_t orientation::calibration::lastCalibrationData = {};
uint8_t orientation::calibration::systemCalibrationStatus = 0;
uint8_t orientation::calibration::gyroscopeCalibrationStatus = 0;
uint8_t orientation::calibration::accelerometerCalibrationStatus = 0;
uint8_t orientation::calibration::magnetometerCalibrationStatus = 0;

void orientation::init() {
  connected = sensor.begin(OPERATION_MODE_NDOF);
  if (!connected) {
    return;
  }
  sensor.setExtCrystalUse(true);
  do {
    sensor.setMode(OPERATION_MODE_NDOF);
    connected = isConnected();
    delay(10);
  } while (connected && sensor.getMode() != OPERATION_MODE_NDOF);
}

bool orientation::isConnected() {
  Wire.beginTransmission(BNO055_ADDRESS_A);
  // Query a real register, otherwise the connectivity check causes a BNO055 system error when it
  // succeeds.
  bool written = Wire.write(Adafruit_BNO055::BNO055_SYS_ERR_ADDR) == 1;
  if (!written) {
    return false;
  }
  uint8_t result = Wire.endTransmission(true);
  return result == 0;
}

void orientation::calibration::updateCalibrationStatuses() {
  if (!orientation::connected) {
    return;
  }
  orientation::sensor.getCalibration(
      &systemCalibrationStatus,
      &gyroscopeCalibrationStatus,
      &accelerometerCalibrationStatus,
      &magnetometerCalibrationStatus);
  if (systemCalibrationStatus == 3
      && gyroscopeCalibrationStatus == 3
      && accelerometerCalibrationStatus == 3
      && magnetometerCalibrationStatus == 3) {
    orientation::sensor.getSensorOffsets(lastCalibrationData);
  }
}

