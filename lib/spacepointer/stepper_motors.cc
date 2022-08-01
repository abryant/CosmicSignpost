#include "stepper_motors.h"

#include <cmath>
#include <memory>

#include "Arduino.h"
#include <sys/time.h>

#include "angle_utils.h"
#include "direction_queue.h"
#include "time_utils.h"

const int32_t STEPS_PER_AZIMUTH_360_DEGREES = 200 * 16;
const int32_t STEPS_PER_ALTITUDE_360_DEGREES = 200 * 16;

StepperMotors::StepperMotors(
    std::shared_ptr<DirectionQueue> directionQueue,
    int32_t azimuthStepPin,
    int32_t azimuthDirectionPin,
    int32_t altitudeStepPin,
    int32_t altitudeDirectionPin)
    : directionQueue(directionQueue),
      azimuthStepPin(azimuthStepPin),
      azimuthDirectionPin(azimuthDirectionPin),
      altitudeStepPin(altitudeStepPin),
      altitudeDirectionPin(altitudeDirectionPin) {
  pinMode(azimuthStepPin, OUTPUT);
  pinMode(azimuthDirectionPin, OUTPUT);
  pinMode(altitudeStepPin, OUTPUT);
  pinMode(altitudeDirectionPin, OUTPUT);
}

Direction StepperMotors::getDirectionAt(int64_t timeMillis) {
  int64_t lowerTime = (timeMillis / 5) * 5;
  int64_t upperTime = lowerTime + 5;
  std::pair<int64_t, Direction> lower = directionQueue->getDirectionAtOrAfter(lowerTime);
  std::pair<int64_t, Direction> upper = directionQueue->peekDirectionAtOrAfter(upperTime);
  int64_t timeDiff = upper.first - lower.first;
  if (timeDiff <= 0) {
    return lower.second;
  }
  Direction lowerDir = lower.second;
  Direction upperDir = upper.second;
  double interpolation = (timeMillis - lower.first) / timeDiff;
  double azimuthChange = wrapDegrees(upperDir.getAzimuth() - lowerDir.getAzimuth());
  double altitudeChange = upperDir.getAltitude() - lowerDir.getAltitude();
  return Direction(
      lowerDir.getAzimuth() + (interpolation * azimuthChange),
      lowerDir.getAltitude() + (interpolation * altitudeChange));
}

// Wraps an azimuth in steps to the range [-180, 180]
int32_t wrapAzimuthSteps(int32_t azimuthSteps) {
  azimuthSteps = azimuthSteps % STEPS_PER_AZIMUTH_360_DEGREES;
  if (azimuthSteps > STEPS_PER_AZIMUTH_360_DEGREES / 2) {
    return azimuthSteps - STEPS_PER_AZIMUTH_360_DEGREES;
  } else if (azimuthSteps < -STEPS_PER_AZIMUTH_360_DEGREES / 2) {
    return azimuthSteps + STEPS_PER_AZIMUTH_360_DEGREES;
  } else {
    return azimuthSteps;
  }
}

void StepperMotors::control() {
  // TODO: see how high we can set the acceleration
  const int32_t MAX_ACCELERATION_STEPS_PER_SECOND_PER_SECOND = 1000;
  const double MAX_ACCELERATION = MAX_ACCELERATION_STEPS_PER_SECOND_PER_SECOND / 1.0e6 / 1.0e6;
  const int32_t SLICE_LENGTH_MICROS = 5000;
  Direction current = Direction(0.0, 0.0);
  Direction next = current;
  // Wrapped between -180 and 180 degrees, but measured in steps.
  int32_t currentAzimuthSteps = 0;
  int32_t currentAltitudeSteps = 0;
  // Acceleration in steps per microsecond^2.
  double currentAzimuthAcceleration = 0.0;
  double currentAltitudeAcceleration = 0.0;

  TimeMillisMicros sliceStart = TimeMillisMicros::now();
  TimeMillisMicros nextSliceStart = sliceStart.plusMicros(SLICE_LENGTH_MICROS);
  TimeMillisMicros lastAzimuthStepTime = sliceStart;
  TimeMillisMicros lastAltitudeStepTime = sliceStart;
  // Speed in steps per microsecond.
  double sliceStartAzimuthSpeed = 0.0;
  double sliceStartAltitudeSpeed = 0.0;
  while (true) {
    TimeMillisMicros now = TimeMillisMicros::now();
    int64_t timeDeltaMicros = now.deltaMicrosSince(sliceStart);
    // Calculate the current speeds in steps per microsecond.
    double currentAzimuthSpeed = sliceStartAzimuthSpeed + (currentAzimuthAcceleration * timeDeltaMicros);
    double currentAltitudeSpeed = sliceStartAltitudeSpeed + (currentAltitudeAcceleration * timeDeltaMicros);
    if (currentAzimuthSpeed != 0) {
      // Find the next azimuth step time based on the current speed (after current acceleration).
      TimeMillisMicros nextAzimuthStepTime =
          lastAzimuthStepTime.plusMicros((int32_t) (1.0 / std::abs(currentAzimuthSpeed)));
      if (now >= nextAzimuthStepTime) {
        int8_t stepDirection = currentAzimuthSpeed > 0 ? 1 : -1;
        stepAzimuth(stepDirection > 0);
        currentAzimuthSteps = wrapAzimuthSteps(currentAzimuthSteps + stepDirection);
        // Moving the azimuth motor always moves the altitude too, because of the way the gears are attached.
        // So we need to subtract all azimuth steps from the altitude steps to compensate.
        currentAltitudeSteps -= stepDirection;
        lastAzimuthStepTime = now;
      }
    }
    if (currentAltitudeSpeed != 0) {
      // Find the next altitude step time based on the current speed (after current acceleration).
      TimeMillisMicros nextAltitudeStepTime =
          lastAltitudeStepTime.plusMicros((int32_t) (1.0 / std::abs(currentAltitudeSpeed)));
      if (now >= nextAltitudeStepTime) {
        int8_t stepDirection = currentAltitudeSpeed > 0 ? 1 : -1;
        stepAltitude(stepDirection > 0);
        currentAltitudeSteps += stepDirection;
        lastAltitudeStepTime = now;
      }
    }


    if (now >= nextSliceStart) {
      sliceStart = nextSliceStart;
      nextSliceStart = nextSliceStart.plusMicros(SLICE_LENGTH_MICROS);
      current = next;
      next = getDirectionAt(nextSliceStart.millis);

      // Azimuth
      sliceStartAzimuthSpeed += currentAzimuthAcceleration * SLICE_LENGTH_MICROS;
      double endAzimuth = next.getAzimuth();
      int32_t endAzimuthSteps = (int32_t) std::round(endAzimuth * STEPS_PER_AZIMUTH_360_DEGREES / 360.0);
      int32_t azimuthDiff = wrapAzimuthSteps(endAzimuthSteps - currentAzimuthSteps);
      int32_t maxAzimuthDiff = (int32_t)
          // s = ut + 1/2*a*t^2 from https://en.wikipedia.org/wiki/Equations_of_motion
          ((currentAzimuthSpeed * SLICE_LENGTH_MICROS)
          + (MAX_ACCELERATION * SLICE_LENGTH_MICROS * SLICE_LENGTH_MICROS / 2.0));
      int32_t minAzimuthDiff = (int32_t)
          // s = ut + 1/2*a*t^2 from https://en.wikipedia.org/wiki/Equations_of_motion
          ((currentAzimuthSpeed * SLICE_LENGTH_MICROS)
          - (MAX_ACCELERATION * SLICE_LENGTH_MICROS * SLICE_LENGTH_MICROS / 2.0));

      int32_t realAzimuthDiff;
      if (minAzimuthDiff <= azimuthDiff && azimuthDiff <= maxAzimuthDiff) {
        // The azimuth is reachable, set the acceleration to reach it.
        // s = u*t + 1/2*a*t^2
        // => a = 2(s/t^2 - u/t)
        currentAzimuthAcceleration =
            2 * (
              (((double) azimuthDiff) / (SLICE_LENGTH_MICROS * SLICE_LENGTH_MICROS))
              - (currentAzimuthSpeed / SLICE_LENGTH_MICROS));
        realAzimuthDiff = azimuthDiff;
      } else {
        // The azimuth is unreachable this time slice. Check whether we're going to overshoot it at
        // the current speed, regardless of how long it takes to get there.
        // v^2 = u^2 + 2*a*s from https://en.wikipedia.org/wiki/Equations_of_motion
        // Stopping distance: s = (v^2 - u^2) / 2*a
        // v = 0, so s = -u^2 / 2*a, and a is negative for deceleration, so:
        int32_t azimuthStoppingDistanceSteps =
            (int32_t) (currentAzimuthSpeed * currentAzimuthSpeed / (2.0 * MAX_ACCELERATION));
        if (azimuthStoppingDistanceSteps > std::abs(azimuthDiff)) {
          // Decelerate so that we can stop at the position.
          currentAzimuthAcceleration = -MAX_ACCELERATION * (currentAzimuthSpeed > 0 ? 1 : -1);
        } else {
          // Accelerate towards the position.
          currentAzimuthAcceleration = MAX_ACCELERATION * (currentAzimuthSpeed > 0 ? 1 : -1);
        }
        realAzimuthDiff = (int32_t)
            // s = ut + 1/2*a*t^2 from https://en.wikipedia.org/wiki/Equations_of_motion
            ((currentAzimuthSpeed * SLICE_LENGTH_MICROS)
            + (currentAzimuthAcceleration * SLICE_LENGTH_MICROS * SLICE_LENGTH_MICROS / 2.0));
      }

      // Altitude
      sliceStartAltitudeSpeed += currentAltitudeAcceleration * SLICE_LENGTH_MICROS;
      double endAltitude = next.getAltitude();
      int32_t endAltitudeSteps =
          (int32_t) std::round(endAltitude * STEPS_PER_ALTITUDE_360_DEGREES / 360.0);
      int32_t altitudeDiff = endAltitudeSteps - currentAltitudeSteps;
      // Moving the azimuth motor always moves the altitude too, because of the way the gears are
      // attached. So we need to subtract the real azimuth steps from the next slice from the
      // altitude steps to compensate.
      altitudeDiff -= realAzimuthDiff;
      int32_t maxAltitudeDiff = (int32_t)
          // s = ut + 1/2*a*t^2 from https://en.wikipedia.org/wiki/Equations_of_motion
          ((currentAltitudeSpeed * SLICE_LENGTH_MICROS)
          + (MAX_ACCELERATION * SLICE_LENGTH_MICROS * SLICE_LENGTH_MICROS / 2.0));
      int32_t minAltitudeDiff = (int32_t)
          // s = ut + 1/2*a*t^2 from https://en.wikipedia.org/wiki/Equations_of_motion
          ((currentAltitudeSpeed * SLICE_LENGTH_MICROS)
          - (MAX_ACCELERATION * SLICE_LENGTH_MICROS * SLICE_LENGTH_MICROS / 2.0));

      if (minAltitudeDiff <= altitudeDiff && altitudeDiff <= maxAltitudeDiff) {
        // The altitude is reachable, set the acceleration to reach it.
        // s = u*t + 1/2*a*t^2
        // => a = 2(s/t^2 - u/t)
        currentAltitudeAcceleration =
            2 * (
              (((double) altitudeDiff) / (SLICE_LENGTH_MICROS * SLICE_LENGTH_MICROS))
              - (currentAltitudeSpeed / SLICE_LENGTH_MICROS));
      } else {
        // The altitude is unreachable this time slice. Check whether we're going to overshoot it at
        // the current speed, regardless of how long it takes to get there.
        // v^2 = u^2 + 2*a*s from https://en.wikipedia.org/wiki/Equations_of_motion
        // Stopping distance: s = (v^2 - u^2) / 2*a
        // v = 0, so s = -u^2 / 2*a, and a is negative for deceleration, so:
        int32_t altitudeStoppingDistanceSteps =
            (int32_t) (currentAltitudeSpeed * currentAltitudeSpeed / (2.0 * MAX_ACCELERATION));
        if (altitudeStoppingDistanceSteps > std::abs(altitudeDiff)) {
          // Decelerate so that we can stop at the position.
          currentAltitudeAcceleration = -MAX_ACCELERATION * (currentAltitudeSpeed > 0 ? 1 : -1);
        } else {
          // Accelerate towards the position.
          currentAltitudeAcceleration = MAX_ACCELERATION * (currentAltitudeSpeed > 0 ? 1 : -1);
        }
      }
    }
  }
}

void StepperMotors::stepAzimuth(bool clockwise) {
  digitalWrite(azimuthDirectionPin, clockwise);
  delayMicroseconds(2);
  digitalWrite(azimuthStepPin, HIGH);
  delayMicroseconds(2);
  digitalWrite(azimuthStepPin, LOW);
}

void StepperMotors::stepAltitude(bool north) {
  digitalWrite(altitudeDirectionPin, north);
  delayMicroseconds(2);
  digitalWrite(altitudeStepPin, HIGH);
  delayMicroseconds(2);
  digitalWrite(altitudeStepPin, LOW);
}
