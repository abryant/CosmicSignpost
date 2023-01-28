#include "stepper_motors.h"

#include <cmath>
#include <memory>

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <sys/time.h>

#include "angle_utils.h"
#include "direction_queue.h"
#include "time_utils.h"

const int32_t STEPS_PER_AZIMUTH_360_DEGREES = 200 * 16 * 2;
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
#ifdef ARDUINO
  pinMode(azimuthStepPin, OUTPUT);
  pinMode(azimuthDirectionPin, OUTPUT);
  pinMode(altitudeStepPin, OUTPUT);
  pinMode(altitudeDirectionPin, OUTPUT);
#endif
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

double azimuthDegreesToSteps(double degrees) {
  return degrees * STEPS_PER_AZIMUTH_360_DEGREES / 360.0;
}

double altitudeDegreesToSteps(double degrees) {
  return degrees * STEPS_PER_ALTITUDE_360_DEGREES / 360.0;
}

// Converts a number of steps, speed, or acceleration from azimuth motor units to altitude motor
// units. This is only necessary if the motors are geared differently, e.g. if altitude needs one
// full rotation to compensate for two full azimuth rotations, then this just applies a linear
// conversion factor.
double convertAzimuthToAltitude(double azimuth) {
  return -(azimuth * STEPS_PER_ALTITUDE_360_DEGREES) / STEPS_PER_AZIMUTH_360_DEGREES;
}

double findSpeedCorrection(double diffSteps) {
  double QUADRATIC_REGION_STEPS = 10.0; // 1.125 degrees
  double scaledSteps = diffSteps / QUADRATIC_REGION_STEPS;
  double result;
  // We need a function that guarantees the acceleration is under a certain constant value,
  // regardless of the input. If x=distance (diff) and t=time, then we need:
  // d^2x/dt^2 = 1
  // Integrating dt, we get:
  // dx/dt = t + c (let's take c=0, since we're making this up)
  // dx/dt is what we need (the speed), but it's in terms of time rather than distance.
  // Integrating again:
  // x = 1/2t^2 + c (let's take c=0 again)
  // giving: t = sqrt(2x)
  // which we can substitute in to the speed equation to get:
  // dx/dt = sqrt(2x)
  // So using a function proportional to sqrt(x) gives us a constant acceleration.
  //
  // Unfortunately, when we're close to zero we don't want a constant acceleration, since that will
  // make us oscillate around zero instead of slowing down to reach it. Instead, we want to
  // decrease the acceleration closer to zero the closer the diff is to zero.
  //
  // To work around this, we can use a piecewise function that uses sqrt(x) for large values of x,
  // and x^2 for small values of x, with some constants to line up the speed and gradient at the
  // boundaries:
  if (scaledSteps > 1) {
    result = std::sqrt(scaledSteps) - 0.75;
  } else if (scaledSteps < -1) {
    result = -std::sqrt(-scaledSteps) + 0.75;
  } else {
    result = scaledSteps * std::abs(scaledSteps) / 4.0;
  }
  // Conversion factor from diff steps to steps per microsecond:
  double SPEED_PER_DIFF_STEP = 10.0 / 1000000.0; // 10 steps per second per step of diff
  return result * QUADRATIC_REGION_STEPS * SPEED_PER_DIFF_STEP;
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
  Direction afterNext = next;
  // Wrapped between -180 and 180 degrees, but measured in steps.
  double currentAzimuthSteps = 0;
  double currentAltitudeSteps = 0;
  // Acceleration in steps per microsecond^2.
  double currentAzimuthAcceleration = 0.0;
  double currentAltitudeAcceleration = 0.0;

  TimeMillisMicros sliceStart = TimeMillisMicros::now();
  TimeMillisMicros nextSliceStart = sliceStart.plusMicros(SLICE_LENGTH_MICROS);
  TimeMillisMicros afterNextSliceStart = nextSliceStart.plusMicros(SLICE_LENGTH_MICROS);
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
    // If the speed is so low that the time until the next step overflows an int32, don't step.
    // (this corresponds to around one step every 35 minutes)
    if (((double) INT32_MAX) * std::abs(currentAzimuthSpeed) > 1.0) {
      // Find the next azimuth step time based on the current speed (after current acceleration).
      TimeMillisMicros nextAzimuthStepTime =
          lastAzimuthStepTime.plusMicros((int64_t) (1.0 / std::abs(currentAzimuthSpeed)));
      if (now >= nextAzimuthStepTime) {
        int8_t stepDirection = currentAzimuthSpeed > 0 ? 1 : -1;
        stepAzimuth(stepDirection > 0);
        currentAzimuthSteps = wrapAzimuthSteps(currentAzimuthSteps + stepDirection);
        // Moving the azimuth motor always moves the altitude too, because of the way the gears are attached.
        // So we need to subtract all azimuth steps from the altitude steps to compensate.
        currentAltitudeSteps -= convertAzimuthToAltitude(stepDirection);
        lastAzimuthStepTime = now;
      }
    }
    // If the speed is so low that the time until the next step overflows an int32, don't step.
    // (this corresponds to around one step every 35 minutes)
    if (((double) INT32_MAX) * std::abs(currentAltitudeSpeed) > 1.0) {
      // Find the next altitude step time based on the current speed (after current acceleration).
      TimeMillisMicros nextAltitudeStepTime =
          lastAltitudeStepTime.plusMicros((int64_t) (1.0 / std::abs(currentAltitudeSpeed)));
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
        nextSliceStart = afterNextSliceStart;
        afterNextSliceStart = afterNextSliceStart.plusMicros(SLICE_LENGTH_MICROS);
      }
      current = next;
      next = afterNext;
      afterNext = getDirectionAt(afterNextSliceStart.millis);
      int64_t lastSliceMicros = timeDeltaMicros;
      int64_t nextSliceMicros = nextSliceStart.deltaMicrosSince(sliceStart);

      // Azimuth
      sliceStartAzimuthSpeed += currentAzimuthAcceleration * lastSliceMicros;

      // Find the average speed from current to afterNext:
      double azimuthEndOfSliceSpeedTarget =
          wrapAzimuthSteps(azimuthDegreesToSteps(afterNext.getAzimuth() - current.getAzimuth()))
          / afterNextSliceStart.deltaMicrosSince(now);
      // Speed correction for converging on the correct position:
      double endAzimuthSteps = azimuthDegreesToSteps(next.getAzimuth());
      double azimuthDiff = wrapAzimuthSteps(endAzimuthSteps - currentAzimuthSteps);
      azimuthEndOfSliceSpeedTarget += findSpeedCorrection(azimuthDiff);
      // Accelerate to match this speed target:
      // v = u + a*t from https://en.wikipedia.org/wiki/Equations_of_motion
      // => a = (v - u) / t
      currentAzimuthAcceleration =
          (azimuthEndOfSliceSpeedTarget - currentAzimuthSpeed) / nextSliceMicros;
      if (currentAzimuthAcceleration > MAX_ACCELERATION) {
        currentAzimuthAcceleration = MAX_ACCELERATION;
      } else if (currentAzimuthAcceleration < -MAX_ACCELERATION) {
        currentAzimuthAcceleration = -MAX_ACCELERATION;
      }

      // Find the azimuth speed at the end of the next slice, so that we can adjust the altitude
      // speed correctly.
      double realAzimuthEndOfSliceSpeed =
          sliceStartAzimuthSpeed + (currentAzimuthAcceleration * nextSliceMicros);

      // Altitude
      sliceStartAltitudeSpeed += currentAltitudeAcceleration * lastSliceMicros;

      // Find the average speed from current to afterNext:
      double altitudeEndOfSliceSpeedTarget =
          (altitudeDegreesToSteps(afterNext.getAltitude() - current.getAltitude()))
          / afterNextSliceStart.deltaMicrosSince(now);
      // Correct for azimuth rotation, which we always need to match:
      altitudeEndOfSliceSpeedTarget += convertAzimuthToAltitude(realAzimuthEndOfSliceSpeed);
      // Speed correction for converging on the correct position:
      double endAltitudeSteps = altitudeDegreesToSteps(next.getAltitude());
      double altitudeDiff = endAltitudeSteps - std::round(currentAltitudeSteps);
      altitudeEndOfSliceSpeedTarget += findSpeedCorrection(altitudeDiff);
      // Accelerate to match this speed target:
      // v = u + a*t from https://en.wikipedia.org/wiki/Equations_of_motion
      // => a = (v - u) / t
      currentAltitudeAcceleration =
          (altitudeEndOfSliceSpeedTarget - currentAltitudeSpeed) / nextSliceMicros;
      if (currentAltitudeAcceleration > MAX_ACCELERATION) {
        currentAltitudeAcceleration = MAX_ACCELERATION;
      } else if (currentAltitudeAcceleration < -MAX_ACCELERATION) {
        currentAltitudeAcceleration = -MAX_ACCELERATION;
      }
    }
  }
}

void StepperMotors::stepAzimuth(bool clockwise) {
#ifdef ARDUINO
  digitalWrite(azimuthDirectionPin, !clockwise);
  delayMicroseconds(2);
  digitalWrite(azimuthStepPin, HIGH);
  delayMicroseconds(2);
  digitalWrite(azimuthStepPin, LOW);
#endif
}

void StepperMotors::stepAltitude(bool north) {
#ifdef ARDUINO
  digitalWrite(altitudeDirectionPin, north);
  delayMicroseconds(2);
  digitalWrite(altitudeStepPin, HIGH);
  delayMicroseconds(2);
  digitalWrite(altitudeStepPin, LOW);
#endif
}
