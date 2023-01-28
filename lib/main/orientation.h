#ifndef COSMIC_SIGNPOST_LIB_MAIN_ORIENTATION_H_
#define COSMIC_SIGNPOST_LIB_MAIN_ORIENTATION_H_

#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

namespace orientation {
  extern Adafruit_BNO055 sensor;
  extern bool connected;

  namespace calibration {
    extern adafruit_bno055_offsets_t lastCalibrationData;
    extern uint8_t systemCalibrationStatus;
    extern uint8_t gyroscopeCalibrationStatus;
    extern uint8_t accelerometerCalibrationStatus;
    extern uint8_t magnetometerCalibrationStatus;

    void updateCalibrationStatuses();
  };

  // Initializes the BNO055 sensor over I2C.
  void init();
  // Checks whether the sensor is connected.
  bool isConnected();
};

#endif
