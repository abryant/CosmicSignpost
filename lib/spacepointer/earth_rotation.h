#ifndef SPACEPOINTER_LIB_SPACEPOINTER_EARTH_ROTATION_H_
#define SPACEPOINTER_LIB_SPACEPOINTER_EARTH_ROTATION_H_

#include <utility>

#include "vector.h"

namespace EarthRotation {
  extern const int8_t NUTATION_ALPHA_COEFFICIENTS_MATRIX[106][5];
  extern const double NUTATION_AB_COEFFICIENTS_MATRIX[106][4];

  Vector earthEquatorialToEarthFixed(Vector v, long timeUtcMillis);

  std::pair<double, double> getDeltaPsiAndDeltaEpsilon(double timeJulianCenturiesSinceJ2000);
  Vector applyPrecession(Vector v, double timeJulianCenturiesSinceJ2000);
  Vector applyNutation(
      Vector v,
      std::pair<double, double> deltaPsiAndDeltaEpsilon,
      double timeJulianCenturiesSinceJ2000);
  Vector applySiderealRotation(
    Vector v,
    std::pair<double, double> deltaPsiAndDeltaEpsilon,
    long timeUtcMillis);
};

#endif
