#include "equatorial_location.h"

#include "angle_utils.h"
#include "error_utils.h"
#include "location.h"

EquatorialLocation::EquatorialLocation(double rightAscension, double declination) {
  this->rightAscension = wrapDegrees(rightAscension);
  checkArgument(-90.0 <= declination && declination <= 90.0, "declination not in range [-90, 90]");
  this->declination = declination;
}

EquatorialLocation::EquatorialLocation(
    int32_t raHour, int32_t raMinute, double raSecond,
    int32_t decDegrees, int32_t decArcminute, double decArcsecond)
    : EquatorialLocation(((((raSecond / 60.0) + raMinute) / 60.0) + raHour) * 15.0,
                          (((decArcsecond / 60.0) + decArcminute) / 60.0) + decDegrees) {
}

CartesianLocation EquatorialLocation::farCartesian(double range) {
  CartesianLocation fixed = Location(declination, rightAscension, range).getCartesian();
  return CartesianLocation(fixed.position, ReferenceFrame::EARTH_EQUATORIAL);
}
