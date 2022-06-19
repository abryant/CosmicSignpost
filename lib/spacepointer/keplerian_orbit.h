#ifndef SPACEPOINTER_LIB_SPACEPOINTER_KEPLERIAN_ORBIT_H_
#define SPACEPOINTER_LIB_SPACEPOINTER_KEPLERIAN_ORBIT_H_

#include <cstdint>

#include "cartesian_location.h"

namespace KeplerianOrbit {

  // Finds the eccentric anomaly in radians, given mean anomaly in radians.
  double findEccentricAnomaly(double meanAnomalyRadians, double eccentricity);

  // Finds the position of a given object in its orbit, given the current orbital parameters,
  // in an inertial reference frame.
  // The mean anomaly must be the current mean anomaly, and is not interpolated over time.
  CartesianLocation findPosition(
    ReferenceFrame referenceFrame,
    double semiMajorAxisMetres,
    double eccentricity,
    double inclinationRadians,
    double longitudeOfAscendingNodeRadians,
    double argumentOfPeriapsisRadians,
    double meanAnomalyRadians);

  // Finds the position of a given object in its orbit, given the orbital parameters,
  // in an inertial reference frame.
  CartesianLocation findPosition(
    int64_t timeMillis,
    ReferenceFrame referenceFrame,
    double semiMajorAxisMetres,
    double eccentricity,
    double inclinationRadians,
    double longitudeOfAscendingNodeRadians,
    double argumentOfPeriapsisRadians,
    double meanAnomalyJ2000Radians,
    double periodJulianDays);
};

#endif
