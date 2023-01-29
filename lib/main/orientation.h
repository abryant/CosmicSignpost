#ifndef COSMIC_SIGNPOST_LIB_MAIN_ORIENTATION_H_
#define COSMIC_SIGNPOST_LIB_MAIN_ORIENTATION_H_

#include <memory>
#include <optional>

#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

// Without an explicit path, the LDF uses BNO055's quaternion.h
#include "tracking/quaternion.h"

#include "stepper_motors.h"
#include "time_utils.h"
#include "tracker.h"

namespace orientation {
  extern Adafruit_BNO055 sensor;
  extern bool connected;

  namespace calibration {
    // To calibrate the motors, we need to be able to reset their coordinates.
    extern std::shared_ptr<StepperMotors> motors;

    extern uint8_t systemCalibrationStatus;
    extern uint8_t gyroscopeCalibrationStatus;
    extern uint8_t accelerometerCalibrationStatus;
    extern uint8_t magnetometerCalibrationStatus;
    extern std::string debug;

    enum class CalibrationStage {
      NOT_CALIBRATING,
      CALIBRATE_GYROSCOPE,
      CALCULATE_ZERO_ALTITUDE,
      WAIT_FOR_ZERO_ALTITUDE,
      RESET_MOTORS_TO_ZERO_ALTITUDE,
      FINISHED_CALIBRATING,
    };

    extern CalibrationStage stage;
    extern int64_t calibrationStartTimeMillis;
    extern int64_t lastGyroscopeTimeMillis;
    extern Direction zeroAltitudeDirection;
    extern int64_t waitForZeroAltitudeLastMovementTimeMillis;
    extern Quaternion waitForZeroAltitudeLastQuaternion;

    // Starts calibration, by telling the tracker to use the calibration direction function.
    void startCalibration(Tracker &tracker);
    // Stops calibration and cleans up (resets to the previous tracking target).
    void stopCalibration(Tracker &tracker);
    bool isCalibrating();

    void updateCalibrationStatuses();

    // Finds the direction that the calibration routine currently wants us to point in.
    Direction getCalibrationDirection(int64_t timeMillis);
  };

  // Initializes the BNO055 sensor over I2C.
  void init();
  // Checks whether the sensor is connected.
  bool isConnected();

  std::optional<Quaternion> getQuaternion();
};

#endif
