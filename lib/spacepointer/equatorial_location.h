#ifndef SPACEPOINTER_SRC_SPACEPOINTER_EQUATORIAL_LOCATION_H_
#define SPACEPOINTER_SRC_SPACEPOINTER_EQUATORIAL_LOCATION_H_

#include "cartesian_location.h"

const double DEFAULT_CARTESIAN_RANGE = 1e200;

class EquatorialLocation {
  private:
    // Right ascension in degrees, J2000.
    double rightAscension;
    // Declination in degrees, J2000.
    double declination;

  public:
    EquatorialLocation(double rightAscension, double declination);
    // Finds the cartesian location of a far-away equatorial location.
    CartesianLocation farCartesian(double range = DEFAULT_CARTESIAN_RANGE);
};

#endif
