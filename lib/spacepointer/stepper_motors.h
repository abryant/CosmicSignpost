#ifndef SPACEPOINTER_LIB_SPACEPOINTER_STEPPER_MOTORS_H_
#define SPACEPOINTER_LIB_SPACEPOINTER_STEPPER_MOTORS_H_

#include <functional>
#include <cstdint>

#include "direction.h"

class StepperMotors {
  private:
    std::function<Direction(int64_t)> directionGenerator;
    int32_t azimuthStepPin;
    int32_t azimuthDirectionPin;
    int32_t altitudeStepPin;
    int32_t altitudeDirectionPin;

    void stepAzimuth(bool clockwise);
    void stepAltitude(bool north);
    Direction getDirectionAt(int64_t timeMillis);

  public:
    StepperMotors(
        std::function<Direction(int64_t)> directionGenerator,
        int32_t azimuthStepPin,
        int32_t azimuthDirectionPin,
        int32_t altitudeStepPin,
        int32_t altitudeDirectionPin);
    void control();
};

#endif
