#include "angle_utils.h"

#include <cmath>

double wrapDegrees(double degrees) {
  degrees = std::fmod(degrees, 360.0);
   // [-360, 360)
  if (degrees <= 0.0) {
    degrees += 360.0;
  }
  // [0, 360)
  if (degrees >= 180.0) {
    degrees -= 360.0;
  }
  // [-180.0, 180.0)
  return degrees;
}

double degreesToRadians(double degrees) {
  return degrees * M_PI / 180.0;
}
