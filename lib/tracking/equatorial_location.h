#ifndef COSMIC_SIGNPOST_LIB_TRACKING_EQUATORIAL_LOCATION_H_
#define COSMIC_SIGNPOST_LIB_TRACKING_EQUATORIAL_LOCATION_H_

#include "cartesian_location.h"

const double DEFAULT_CARTESIAN_RANGE = 1e100;

class EquatorialLocation {
  private:
    // Right ascension in degrees, J2000.
    double rightAscension;
    // Declination in degrees, J2000.
    double declination;

  public:
    EquatorialLocation(double rightAscension, double declination);
    EquatorialLocation(
        int32_t raHour, int32_t raMinute, double raSecond,
        int32_t decDegrees, int32_t decArcminute, double decArcsecond);
    // Finds the cartesian location of a far-away equatorial location.
    CartesianLocation farCartesian(double range = DEFAULT_CARTESIAN_RANGE);
};

#endif
