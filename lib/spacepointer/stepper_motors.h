#ifndef SPACEPOINTER_LIB_SPACEPOINTER_STEPPER_MOTORS_H_
#define SPACEPOINTER_LIB_SPACEPOINTER_STEPPER_MOTORS_H_

#include <functional>
#include <cstdint>
#include <memory>

#include "direction.h"
#include "direction_queue.h"

class StepperMotors {
  private:
    std::shared_ptr<DirectionQueue> directionQueue;
    int32_t azimuthStepPin;
    int32_t azimuthDirectionPin;
    int32_t altitudeStepPin;
    int32_t altitudeDirectionPin;

    void stepAzimuth(bool clockwise);
    void stepAltitude(bool north);
    Direction getDirectionAt(int64_t timeMillis);

  public:
    StepperMotors(
        std::shared_ptr<DirectionQueue> directionQueue,
        int32_t azimuthStepPin,
        int32_t azimuthDirectionPin,
        int32_t altitudeStepPin,
        int32_t altitudeDirectionPin);
    void control();
};

#endif
