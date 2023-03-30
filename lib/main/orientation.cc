#include "orientation.h"

#include <cmath>
#include <optional>
#include <sstream>
#include <iomanip>

#include <Arduino.h>
#include <Adafruit_BNO055.h>

Adafruit_BNO055 orientation::sensor = Adafruit_BNO055();
bool orientation::connected = false;

std::shared_ptr<StepperMotors> orientation::calibration::motors = nullptr;

uint8_t orientation::calibration::systemCalibrationStatus = 0;
uint8_t orientation::calibration::gyroscopeCalibrationStatus = 0;
uint8_t orientation::calibration::accelerometerCalibrationStatus = 0;
uint8_t orientation::calibration::magnetometerCalibrationStatus = 0;

orientation::calibration::CalibrationStage orientation::calibration::stage =
    orientation::calibration::CalibrationStage::NOT_CALIBRATING;
int64_t orientation::calibration::calibrationStartTimeMillis = 0;
int64_t orientation::calibration::lastGyroscopeTimeMillis = 0;
Direction orientation::calibration::zeroAltitudeDirection = Direction(0, 0);

int64_t orientation::calibration::lastMovementTimeMillis = 0;
Quaternion orientation::calibration::lastMovementQuaternion = Quaternion::rotateX(0);

Direction orientation::calibration::lastCompassDirection = Direction(0, 0);

Direction orientation::calibration::zeroAzimuthDirection = Direction(0, 0);

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

bool orientation::calibration::isCalibrating() {
  return stage != CalibrationStage::NOT_CALIBRATING && stage != CalibrationStage::FINISHED_CALIBRATING;
}

std::string orientation::calibration::getStatusString() {
  std::ostringstream orientationStr;
  if (orientation::connected) {
    switch (orientation::calibration::stage) {
      case orientation::calibration::CalibrationStage::NOT_CALIBRATING:
        // We shouldn't usually see this, it only happens when calibration stops early
        // (via this MenuEntry being deactivated).
        orientationStr << "Stopped     ";
        break;
      case orientation::calibration::CalibrationStage::CALIBRATE_GYROSCOPE:
        orientationStr << "Tuning Gyro ";
        break;
      case orientation::calibration::CalibrationStage::CALCULATE_ZERO_ALTITUDE:
      case orientation::calibration::CalibrationStage::WAIT_FOR_ZERO_ALTITUDE:
      case orientation::calibration::CalibrationStage::RESET_MOTORS_TO_ZERO_ALTITUDE:
        orientationStr << "Zeroing Alt ";
        break;
      case orientation::calibration::CalibrationStage::CALIBRATE_MAGNETOMETER:
        orientationStr << "Compass     ";
        break;
      case orientation::calibration::CalibrationStage::CALCULATE_ZERO_AZIMUTH:
      case orientation::calibration::CalibrationStage::WAIT_FOR_ZERO_AZIMUTH:
      case orientation::calibration::CalibrationStage::RESET_MOTORS_TO_ZERO_AZIMUTH:
        orientationStr << "Zeroing Azi ";
        break;
      case orientation::calibration::CalibrationStage::FINISHED_CALIBRATING:
        orientationStr << "Finished    ";
        break;
    }
    orientationStr << std::to_string(orientation::calibration::systemCalibrationStatus);
    orientationStr << std::to_string(orientation::calibration::gyroscopeCalibrationStatus);
    orientationStr << std::to_string(orientation::calibration::accelerometerCalibrationStatus);
    orientationStr << std::to_string(orientation::calibration::magnetometerCalibrationStatus);
  } else {
    orientationStr << "No Compass";
  }
  return orientationStr.str();
}

void orientation::calibration::resetMovementCheck(int64_t timeMillis, Quaternion sensorQuaternion) {
  // If we've already reset the movement with a time later than now, then keep waiting until that
  // time instead of bringing it forward.
  lastMovementTimeMillis = std::max(lastMovementTimeMillis, timeMillis);
  lastMovementQuaternion = sensorQuaternion;
}

bool orientation::calibration::hasStoppedMoving(int64_t timeMillis, Quaternion sensorQuaternion) {
  Vector pointerDirection = Vector(-1, 0, 0);
  Vector lastPointerDirection = lastMovementQuaternion.rotate(pointerDirection);
  Vector currentPointerDirection = sensorQuaternion.rotate(pointerDirection);
  if ((currentPointerDirection - lastPointerDirection).getLength() > 0.0001) {
    resetMovementCheck(timeMillis, sensorQuaternion);
  }
  // Wait until 1 second after the last movement.
  return (timeMillis - lastMovementTimeMillis) > 1000;
}

double findAngleToHorizontalDegrees(Quaternion sensorQuaternion) {
  Vector pointerDirection = Vector(-1, 0, 0);
  pointerDirection = sensorQuaternion.rotate(pointerDirection);
  Vector down = Vector(0, 0, -1);
  double angleToDownDegrees = pointerDirection.angleDegrees(down);
  // If we're above the horizontal, the result is negative (meaning we should rotate down).
  // If we're below the horizontal, the result is positive (meaning we should rotate up).
  return 90 - angleToDownDegrees;
}

double findAngleToNorthDegrees(Quaternion sensorQuaternion) {
  Vector pointerDirection = Vector(-1, 0, 0);
  pointerDirection = sensorQuaternion.rotate(pointerDirection);

  // The azimuth is defined as the angle clockwise from north.
  // From the sensor's perspective, the Y axis points north and the X axis points east.
  // We subtract this from atan2(1, 0) to convert from an anticlockwise angle from east into a
  // clockwise angle from north.
  double azimuth = atan2(1, 0) - atan2(pointerDirection.getY(), pointerDirection.getX());
  double azimuthDegrees = azimuth * 180.0 / M_PI;
  // Azimuth is our current azimuth, negate it to find the azimuth we need to return to north.
  return -azimuthDegrees;
}

Direction orientation::calibration::getCalibrationDirection(int64_t timeMillis) {
  if (!orientation::connected) {
    stage = CalibrationStage::NOT_CALIBRATING;
    calibrationStartTimeMillis = 0;
    return Direction(0, 0);
  }

  updateCalibrationStatuses();
  std::optional<Quaternion> sensorQuaternion = std::nullopt;
  if (stage != CalibrationStage::CALIBRATE_GYROSCOPE) {
    sensorQuaternion = getQuaternion();
    if (!sensorQuaternion.has_value()) {
      stage = CalibrationStage::NOT_CALIBRATING;
      return Direction(0, 0);
    }
  }

  switch (stage) {
    case CalibrationStage::CALIBRATE_GYROSCOPE:
      {
        if (calibrationStartTimeMillis == 0) {
          calibrationStartTimeMillis = timeMillis;
        }
        if (gyroscopeCalibrationStatus == 3) {
          stage = CalibrationStage::CALCULATE_ZERO_ALTITUDE;
          zeroAltitudeDirection = Direction(0, 0);
        }
        // Stay at 0,0 and wait for it to become 3.
        lastGyroscopeTimeMillis = timeMillis;
        return Direction(0, 0);
      }
    case CalibrationStage::CALCULATE_ZERO_ALTITUDE:
      {
        double angleToHorizontalDegrees = findAngleToHorizontalDegrees(sensorQuaternion.value());
        zeroAltitudeDirection = Direction(0, angleToHorizontalDegrees);

        stage = CalibrationStage::WAIT_FOR_ZERO_ALTITUDE;
        resetMovementCheck(timeMillis, sensorQuaternion.value());
        return zeroAltitudeDirection;
      }
    case CalibrationStage::WAIT_FOR_ZERO_ALTITUDE:
      {
        if (hasStoppedMoving(timeMillis, sensorQuaternion.value())) {
          stage = CalibrationStage::RESET_MOTORS_TO_ZERO_ALTITUDE;
        }
        return zeroAltitudeDirection;
      }
    case CalibrationStage::RESET_MOTORS_TO_ZERO_ALTITUDE:
      {
        motors->requestCoordinateReset();
        zeroAltitudeDirection = Direction(0, 0);

        // Resetting the motors clears the direction queue. The way we measure time is the
        // timestamp that we're adding to the queue, so our time measurement will be off for as
        // many time intervals as the queue can contain.
        // This is usually only half a second, so we're going to reset the movement check with a
        // time that's half a second later than "now".
        resetMovementCheck(timeMillis + 500, sensorQuaternion.value());

        double angleToHorizontalDegrees = findAngleToHorizontalDegrees(sensorQuaternion.value());
        if (std::fabs(angleToHorizontalDegrees) < 0.25) {
          if (magnetometerCalibrationStatus == 3) {
            // We're already calibrated and stopped at 0, 0.
            // Skip calibration, and move to finding zero azimuth.
            stage = CalibrationStage::CALCULATE_ZERO_AZIMUTH;
          } else {
            stage = CalibrationStage::CALIBRATE_MAGNETOMETER;
            lastCompassDirection = Direction(-90, 0);
          }
        } else {
          stage = CalibrationStage::CALCULATE_ZERO_ALTITUDE;
        }
        return Direction(0, 0);
      }
    case CalibrationStage::CALIBRATE_MAGNETOMETER:
      {
        if (hasStoppedMoving(timeMillis, sensorQuaternion.value())) {
          resetMovementCheck(timeMillis, sensorQuaternion.value());
          if (magnetometerCalibrationStatus != 3) {
            // Rotate anticlockwise and stop every 90 degrees, swapping between up and down every time.
            double newAzimuth = lastCompassDirection.getAzimuth() - 90;
            double newAltitude = lastCompassDirection.getAltitude() > 0 ? -90 : 90;
            lastCompassDirection = Direction(newAzimuth, newAltitude);
          } else if (lastCompassDirection.getAzimuth() != 0 || lastCompassDirection.getAltitude() != 0) {
            // Calibrated, go back to 0, 0 so we can move on.
            lastCompassDirection = Direction(0, 0);
          } else {
            // Calibrated and stopped at 0, 0.
            stage = CalibrationStage::CALCULATE_ZERO_AZIMUTH;
          }
        }
        return lastCompassDirection;
      }
    case CalibrationStage::CALCULATE_ZERO_AZIMUTH:
      {
        double angleToNorthDegrees = findAngleToNorthDegrees(sensorQuaternion.value());
        zeroAzimuthDirection = Direction(angleToNorthDegrees, 0);
        stage = CalibrationStage::WAIT_FOR_ZERO_AZIMUTH;
        resetMovementCheck(timeMillis, sensorQuaternion.value());
        return zeroAzimuthDirection;
      }
    case CalibrationStage::WAIT_FOR_ZERO_AZIMUTH:
      {
        if (hasStoppedMoving(timeMillis, sensorQuaternion.value())) {
          stage = CalibrationStage::RESET_MOTORS_TO_ZERO_AZIMUTH;
        }
        return zeroAzimuthDirection;
      }
    case CalibrationStage::RESET_MOTORS_TO_ZERO_AZIMUTH:
      {
        motors->requestCoordinateReset();
        zeroAzimuthDirection = Direction(0, 0);

        // Resetting the motors clears the direction queue. The way we measure time is the
        // timestamp that we're adding to the queue, so our time measurement will be off for as
        // many time intervals as the queue can contain.
        // This is usually only half a second, so we're going to reset the movement check with a
        // time that's half a second later than "now".
        resetMovementCheck(timeMillis + 500, sensorQuaternion.value());

        // Make sure the altitude is zero before we check whether we're pointing north.
        double angleToHorizontalDegrees = findAngleToHorizontalDegrees(sensorQuaternion.value());
        if (std::fabs(angleToHorizontalDegrees) >= 0.25) {
          stage = CalibrationStage::CALCULATE_ZERO_ALTITUDE;
        }

        double angleToNorthDegrees = findAngleToNorthDegrees(sensorQuaternion.value());
        if (std::fabs(angleToNorthDegrees) < 0.25) {
          stage = CalibrationStage::FINISHED_CALIBRATING;
        } else {
          stage = CalibrationStage::CALCULATE_ZERO_AZIMUTH;
        }
        return Direction(0, 0);
      }
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
