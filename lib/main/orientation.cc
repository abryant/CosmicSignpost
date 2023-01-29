#include "orientation.h"

#include <Arduino.h>
#include <Adafruit_BNO055.h>

Adafruit_BNO055 orientation::sensor = Adafruit_BNO055();
bool orientation::connected = false;

uint8_t orientation::calibration::systemCalibrationStatus = 0;
uint8_t orientation::calibration::gyroscopeCalibrationStatus = 0;
uint8_t orientation::calibration::accelerometerCalibrationStatus = 0;
uint8_t orientation::calibration::magnetometerCalibrationStatus = 0;

orientation::calibration::CalibrationStage orientation::calibration::stage =
    orientation::calibration::CalibrationStage::NOT_CALIBRATING;
int64_t orientation::calibration::calibrationStartTimeMillis = 0;
int64_t orientation::calibration::lastGyroscopeTimeMillis = 0;

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

std::optional<Quaternion> orientation::getQuaternion() {
  if (!connected) {
    return std::nullopt;
  }
  imu::Quaternion imuQuat = sensor.getQuat();
  if (imuQuat.w() == 0 && imuQuat.x() == 0 && imuQuat.y() == 0 && imuQuat.z() == 0) {
    return std::nullopt;
  }
  return Quaternion(imuQuat.w(), imuQuat.x(), imuQuat.y(), imuQuat.z());
}

void orientation::calibration::startCalibration(Tracker &tracker) {
  if (!connected) {
    stopCalibration(tracker);
    return;
  }
  stage = CalibrationStage::CALIBRATE_GYROSCOPE;
  calibrationStartTimeMillis = 0;
  tracker.setDirectionFunction(getCalibrationDirection);
}

void orientation::calibration::stopCalibration(Tracker &tracker) {
  stage = CalibrationStage::NOT_CALIBRATING;
  calibrationStartTimeMillis = 0;
  tracker.setDirectionFunction(std::nullopt);
}

Direction orientation::calibration::getCalibrationDirection(int64_t timeMillis) {
  if (!orientation::connected) {
    stage = CalibrationStage::NOT_CALIBRATING;
    calibrationStartTimeMillis = 0;
    return Direction(0, 0);
  }
  updateCalibrationStatuses();

  switch (stage) {
    case CalibrationStage::CALIBRATE_GYROSCOPE:
      if (calibrationStartTimeMillis == 0) {
        calibrationStartTimeMillis = timeMillis;
      }
      if (gyroscopeCalibrationStatus == 3) {
        stage = CalibrationStage::ZERO_ALTITUDE;
      }
      // Stay at 0,0 and wait for it to become 3.
      lastGyroscopeTimeMillis = timeMillis;
      return Direction(0, 0);
    case CalibrationStage::ZERO_ALTITUDE:
      // TODO: move to where 0,0 should be, wait for it to stabilise, and then tell the motors that
      // this is their new zero and wait for them to accept it, then move on to the next stage.
      return Direction(0, 0);
    default:
    case CalibrationStage::NOT_CALIBRATING:
    case CalibrationStage::FINISHED_CALIBRATING:
      return Direction(0, 0);
  }
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
}
