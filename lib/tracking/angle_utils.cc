#include "angle_utils.h"

#include <cmath>

double wrapDegrees(double degrees) {
  degrees = std::fmod(degrees, 360.0);
   // [-360, 360)
  if (degrees < 0.0) {
    degrees += 360.0;
  }
  // [0, 360)
  if (degrees >= 180.0) {
    degrees -= 360.0;
  }
  // [-180.0, 180.0)
  return degrees;
}

double wrapRadians(double radians) {
  radians = std::fmod(radians, 2 * M_PI);
   // [-2*pi, 2*pi)
  if (radians < 0.0) {
    radians += 2 * M_PI;
  }
  // [0, 2*pi)
  if (radians >= M_PI) {
    radians -= 2 * M_PI;
  }
  // [-pi, pi)
  return radians;
}

double degreesToRadians(double degrees) {
  return degrees * M_PI / 180.0;
}
