#include "direction.h"

#include "angle_utils.h"
#include "error_utils.h"

Direction::Direction(double azimuth, double altitude) {
  this->azimuth = wrapDegrees(azimuth);
  checkArgument(-90.0 <= altitude && altitude <= 90.0, "altitude not in range [-90, 90]");
  this->altitude = altitude;
}

double Direction::getAzimuth() {
  return this->azimuth;
}

double Direction::getAltitude() {
  return this->altitude;
}
