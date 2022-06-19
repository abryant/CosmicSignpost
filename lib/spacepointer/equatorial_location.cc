#include "equatorial_location.h"

#include "angle_utils.h"
#include "error_utils.h"
#include "location.h"

EquatorialLocation::EquatorialLocation(double rightAscension, double declination) {
  this->rightAscension = wrapDegrees(rightAscension);
  checkArgumentOrReset(-90.0 <= declination && declination <= 90.0, "declination not in range [-90, 90]");
  this->declination = declination;
}

CartesianLocation EquatorialLocation::farCartesian(double range) {
  CartesianLocation fixed = Location(declination, rightAscension, range).getCartesian();
  return CartesianLocation(fixed.x, fixed.y, fixed.z, ReferenceFrame::EARTH_EQUATORIAL);
}
