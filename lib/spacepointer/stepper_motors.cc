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
  int64_t lowerTime = (timeMillis / 50) * 50;
  int64_t upperTime = lowerTime + 50;
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
double wrapAzimuthSteps(double azimuthSteps) {
  azimuthSteps = std::fmod(azimuthSteps, STEPS_PER_AZIMUTH_360_DEGREES);
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
  const int32_t MAX_ACCELERATION_STEPS_PER_SECOND_PER_SECOND = 5000;
  const double MAX_ACCELERATION = MAX_ACCELERATION_STEPS_PER_SECOND_PER_SECOND / 1.0e6 / 1.0e6;
  const int32_t SLICE_LENGTH_MICROS = 50000;
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
      sliceStart = now;
      while (now >= nextSliceStart) {
        nextSliceStart = nextSliceStart.plusMicros(SLICE_LENGTH_MICROS);
      }
      current = next;
      next = getDirectionAt(nextSliceStart.millis);
      int64_t lastSliceMicros = timeDeltaMicros;
      int64_t nextSliceMicros = nextSliceStart.deltaMicrosSince(sliceStart);

      // Azimuth
      sliceStartAzimuthSpeed += currentAzimuthAcceleration * lastSliceMicros;
      double endAzimuth = next.getAzimuth();
      double endAzimuthSteps = endAzimuth * STEPS_PER_AZIMUTH_360_DEGREES / 360.0;
      double azimuthDiff = wrapAzimuthSteps(endAzimuthSteps - currentAzimuthSteps);
      double maxAzimuthDiff =
          // s = ut + 1/2*a*t^2 from https://en.wikipedia.org/wiki/Equations_of_motion
          ((currentAzimuthSpeed * nextSliceMicros)
          + (MAX_ACCELERATION * nextSliceMicros * nextSliceMicros / 2.0));
      double minAzimuthDiff =
          // s = ut + 1/2*a*t^2 from https://en.wikipedia.org/wiki/Equations_of_motion
          ((currentAzimuthSpeed * nextSliceMicros)
          - (MAX_ACCELERATION * nextSliceMicros * nextSliceMicros / 2.0));

      double realAzimuthDiff;
      if (std::abs(azimuthDiff) <= 0.5 && std::abs(currentAzimuthSpeed * nextSliceMicros) <= 10) {
        // We're moving really slowly and don't need to get anywhere, so zero out the speed for
        // the next slice.
        sliceStartAzimuthSpeed = 0;
        currentAzimuthAcceleration = 0;
        realAzimuthDiff = azimuthDiff;
      } else if (minAzimuthDiff <= azimuthDiff && azimuthDiff <= maxAzimuthDiff) {
        // The azimuth is reachable, set the acceleration to reach it.
        // s = u*t + 1/2*a*t^2
        // => a = 2(s/t^2 - u/t)
        currentAzimuthAcceleration =
            2 * (
              (((double) azimuthDiff) / (nextSliceMicros * nextSliceMicros))
              - (currentAzimuthSpeed / nextSliceMicros));
        realAzimuthDiff = azimuthDiff;
      } else {
        // The azimuth is unreachable this time slice. We need to decide whether to accelerate or
        // decelerate so that we don't overshoot the target, based on our stopping distance.
        //
        // If we base the calculation on our current stopping distance, then we might decide that
        // we don't need to decelerate yet and so decide to accelerate instead. This could mean
        // that our speed at the end of the next slice is too high to decelerate in time to reach
        // our target, causing an overshoot.
        //
        // To avoid this problem, we will simulate one slice worth of movement and find the
        // stopping distance after that. To begin with, we make a naive decision to accelerate
        // towards the positive or the negative for one more time slice, based on which way we need
        // to go to reach azimuthDiff.
        double naiveAcceleration = (azimuthDiff > 0) ? MAX_ACCELERATION : -MAX_ACCELERATION;
        // Simulate the next time slice:
        // s = u*t + 1/2*a*t^2
        double nextSliceAzimuthDiff =
            (currentAzimuthSpeed * nextSliceMicros)
            + (naiveAcceleration * nextSliceMicros * nextSliceMicros / 2.0);
        // v = u + a*t
        double nextSliceAzimuthSpeed =
            currentAzimuthSpeed + (nextSliceMicros * naiveAcceleration);
        if ((nextSliceAzimuthSpeed < 0) != (currentAzimuthSpeed < 0)) {
          // We're about to change directions, which means our stopping distance is within the next
          // time slice. Disable overshoot protection and choose the naive approach, because
          // (a) overshoot protection would make us accelerate away from the desired location, and
          // (b) our speed will at least be in the right direction after we reverse.
          currentAzimuthAcceleration = naiveAcceleration;
        } else {
          // Calculate the stopping distance after the next time slice:
          // v^2 = u^2 + 2*a*s from https://en.wikipedia.org/wiki/Equations_of_motion
          // Stopping distance: s = (v^2 - u^2) / 2*a
          // v = 0, so s = -u^2 / 2*a, and a is negative for deceleration, so:
          double nextSliceAzimuthStoppingDistanceSteps =
              nextSliceAzimuthSpeed * nextSliceAzimuthSpeed / (2.0 * MAX_ACCELERATION);
          // This equation does not tell us the direction that we'll stop in, so:
          double nextSliceAzimuthStopDiffSteps =
              (nextSliceAzimuthSpeed > 0 ? 1 : -1) * nextSliceAzimuthStoppingDistanceSteps;
          double naiveAzimuthStopDiffSteps =
              nextSliceAzimuthDiff + nextSliceAzimuthStopDiffSteps;

          // We want to accelerate/decelerate based on the direction we're aiming for and whether the
          // stopping distance is before or after our destination. It turns out that the direction of
          // acceleration/deceleration cancels out with the orientation of before/after for the
          // stopping diff. Here are the two cases:
          // azimuthDiff is positive: accelerate(+) iff naiveAzimuthStopDiffSteps is lower than it.
          // azimuthDiff is negative: accelerate(-) iff naiveAzimuthStopDiffSteps is higher than it.
          currentAzimuthAcceleration =
              (naiveAzimuthStopDiffSteps < azimuthDiff) ? MAX_ACCELERATION : -MAX_ACCELERATION;
        }

        realAzimuthDiff =
            // s = ut + 1/2*a*t^2 from https://en.wikipedia.org/wiki/Equations_of_motion
            ((currentAzimuthSpeed * nextSliceMicros)
            + (currentAzimuthAcceleration * nextSliceMicros * nextSliceMicros / 2.0));
      }

      // Altitude
      sliceStartAltitudeSpeed += currentAltitudeAcceleration * lastSliceMicros;
      double endAltitude = next.getAltitude();
      double endAltitudeSteps =
          endAltitude * STEPS_PER_ALTITUDE_360_DEGREES / 360.0;
      double altitudeDiff = endAltitudeSteps - currentAltitudeSteps;
      // Moving the azimuth motor always moves the altitude too, because of the way the gears are
      // attached. So we need to subtract the real azimuth steps from the next slice from the
      // altitude steps to compensate.
      altitudeDiff -= realAzimuthDiff;
      double maxAltitudeDiff =
          // s = ut + 1/2*a*t^2 from https://en.wikipedia.org/wiki/Equations_of_motion
          ((currentAltitudeSpeed * nextSliceMicros)
          + (MAX_ACCELERATION * nextSliceMicros * nextSliceMicros / 2.0));
      double minAltitudeDiff =
          // s = ut + 1/2*a*t^2 from https://en.wikipedia.org/wiki/Equations_of_motion
          ((currentAltitudeSpeed * nextSliceMicros)
          - (MAX_ACCELERATION * nextSliceMicros * nextSliceMicros / 2.0));

      if (std::abs(altitudeDiff) <= 0.5 && std::abs(currentAltitudeSpeed * nextSliceMicros) <= 10) {
          // We're moving really slowly and don't need to get anywhere, so zero out the speed for
        // the next slice.
        sliceStartAltitudeSpeed = 0;
        currentAltitudeAcceleration = 0;
      } else if (minAltitudeDiff <= altitudeDiff && altitudeDiff <= maxAltitudeDiff) {
        // The altitude is reachable, set the acceleration to reach it.
        // s = u*t + 1/2*a*t^2
        // => a = 2(s/t^2 - u/t)
        currentAltitudeAcceleration =
            2 * (
              (((double) altitudeDiff) / (nextSliceMicros * nextSliceMicros))
              - (currentAltitudeSpeed / nextSliceMicros));
      } else {
        // The altitude is unreachable this time slice. We need to decide whether to accelerate or
        // decelerate so that we don't overshoot the target, based on our stopping distance.
        //
        // If we base the calculation on our current stopping distance, then we might decide that
        // we don't need to decelerate yet and so decide to accelerate instead. This could mean
        // that our speed at the end of the next slice is too high to decelerate in time to reach
        // our target, causing an overshoot.
        //
        // To avoid this problem, we will simulate one slice worth of movement and find the
        // stopping distance after that. To begin with, we make a naive decision to accelerate
        // towards the positive or the negative for one more time slice, based on which way we need
        // to go to reach altitudeDiff.
        double naiveAcceleration = (azimuthDiff > 0) ? MAX_ACCELERATION : -MAX_ACCELERATION;
        // Simulate the next time slice:
        // s = u*t + 1/2*a*t^2
        double nextSliceAltitudeDiff =
            (currentAltitudeSpeed * nextSliceMicros)
            + (naiveAcceleration * nextSliceMicros * nextSliceMicros / 2.0);
        // v = u + a*t
        double nextSliceAltitudeSpeed =
            currentAltitudeSpeed + (nextSliceMicros * naiveAcceleration);
        if ((nextSliceAltitudeSpeed < 0) != (currentAltitudeSpeed < 0)) {
          // We're about to change directions, which means our stopping distance is within the next
          // time slice. Disable overshoot protection and choose the naive approach, because
          // (a) overshoot protection would make us accelerate away from the desired location, and
          // (b) our speed will at least be in the right direction after we reverse.
          currentAltitudeAcceleration = naiveAcceleration;
        } else {
          // Calculate the stopping distance after the next time slice:
          // v^2 = u^2 + 2*a*s from https://en.wikipedia.org/wiki/Equations_of_motion
          // Stopping distance: s = (v^2 - u^2) / 2*a
          // v = 0, so s = -u^2 / 2*a, and a is negative for deceleration, so:
          double nextSliceAltitudeStoppingDistanceSteps =
              nextSliceAltitudeSpeed * nextSliceAltitudeSpeed / (2.0 * MAX_ACCELERATION);
          // This equation does not tell us the direction that we'll stop in, so:
          double nextSliceAltitudeStopDiffSteps =
              (nextSliceAltitudeSpeed > 0 ? 1 : -1) * nextSliceAltitudeStoppingDistanceSteps;
          double naiveAltitudeStopDiffSteps =
              nextSliceAltitudeDiff + nextSliceAltitudeStopDiffSteps;

          // We want to accelerate/decelerate based on the direction we're aiming for and whether the
          // stopping distance is before or after our destination. It turns out that the direction of
          // acceleration/deceleration cancels out with the orientation of before/after for the
          // stopping diff. Here are the two cases:
          // altitudeDiff is positive: accelerate(+) iff naiveAltitudeStopDiffSteps is lower than it.
          // altitudeDiff is negative: accelerate(-) iff naiveAltitudeStopDiffSteps is higher than it.
          currentAltitudeAcceleration =
              (naiveAltitudeStopDiffSteps < altitudeDiff) ? MAX_ACCELERATION : -MAX_ACCELERATION;
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
