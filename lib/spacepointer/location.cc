#include "location.h"

#include <cmath>

#include "angle_utils.h"
#include "cartesian_location.h"
#include "direction.h"
#include "error_utils.h"
#include "quaternion.h"

// From https://en.wikipedia.org/wiki/Geodetic_Reference_System_1980
const double earthEquatorialRadius = 6378137; // semi-major axis
const double earthPolarRadius = 6356752.314; // semi-minor axis

Location::Location(double latitude, double longitude, double elevation) {
  checkArgument(-90.0 <= latitude && latitude <= 90.0, "latitude not in range [-90, 90]");
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

CartesianLocation Location::getCartesian() {
  double latitudeRadians = this->latitude * M_PI / 180.0;
  double longitudeRadians = this->longitude * M_PI / 180.0;
  double equatorialPart = earthEquatorialRadius * std::cos(latitudeRadians);
  double polarPart = earthPolarRadius * std::sin(latitudeRadians);
  double n = (earthEquatorialRadius * earthEquatorialRadius) /
      std::sqrt((equatorialPart * equatorialPart) + (polarPart * polarPart));
  double x = (n + this->elevation) * std::cos(latitudeRadians) * std::cos(longitudeRadians);
  double y = (n + this->elevation) * std::cos(latitudeRadians) * std::sin(longitudeRadians);
  double z = (
        (n * (earthPolarRadius * earthPolarRadius) / (earthEquatorialRadius * earthEquatorialRadius))
        + this->elevation)
    * std::sin(latitudeRadians);
  return CartesianLocation(x, y, z, ReferenceFrame::EARTH_FIXED);
}

Vector Location::getNormal() {
  double latitudeRadians = this->latitude * M_PI / 180.0;
  double longitudeRadians = this->longitude * M_PI / 180.0;
  return Vector(
      std::cos(latitudeRadians) * std::cos(longitudeRadians),
      std::cos(latitudeRadians) * std::sin(longitudeRadians),
      std::sin(latitudeRadians)
    ).normalized();
}

Direction Location::directionTo(Location other) {
  return getCartesian().directionTowards(other.getCartesian(), getNormal());
}
