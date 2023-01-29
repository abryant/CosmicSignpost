#ifndef COSMIC_SIGNPOST_LIB_TRACKING_STEPPER_MOTORS_H_
#define COSMIC_SIGNPOST_LIB_TRACKING_STEPPER_MOTORS_H_

#include <atomic>
#include <functional>
#include <cstdint>
#include <memory>
#include <optional>

#include "direction.h"
#include "direction_queue.h"

class StepperMotors {
  private:
    std::shared_ptr<DirectionQueue> directionQueue;
    std::atomic<bool> shouldResetCoordinates;
    int32_t azimuthStepPin;
    int32_t azimuthDirectionPin;
    int32_t altitudeStepPin;
    int32_t altitudeDirectionPin;

    void stepAzimuth(bool clockwise);
    void stepAltitude(bool north);
    std::optional<Direction> getDirectionAt(int64_t timeMillis);

  public:
    StepperMotors(
        std::shared_ptr<DirectionQueue> directionQueue,
        int32_t azimuthStepPin,
        int32_t azimuthDirectionPin,
        int32_t altitudeStepPin,
        int32_t altitudeDirectionPin);

    // Requests that the motors reset their coordinate frame to be zero at the current location.
    // This is thread-safe.
    void requestCoordinateReset();

    // Controls the motors. This should be run on a dedicated core.
    void control();
};

#endif
