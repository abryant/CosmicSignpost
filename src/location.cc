#include "location.h"

#include "angle_utils.h"
#include "error_utils.h"

Location::Location(double latitude, double longitude, double elevation) {
  checkArgumentOrReset(-90.0 <= latitude && latitude <= 90.0, "latitude not in range [-90, 90]");
  this->latitude = latitude;
  this->longitude = wrapDegrees(longitude);
  this->elevation = elevation;
}

double Location::getLatitude() {
  return this->latitude;
}

double Location::getLongitude() {
  return this->longitude;
}

double Location::getElevation() {
  return this->elevation;
}
