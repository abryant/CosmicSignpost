#ifndef COSMIC_SIGNPOST_LIB_TRACKING_EARTH_ROTATION_H_
#define COSMIC_SIGNPOST_LIB_TRACKING_EARTH_ROTATION_H_

#include <utility>

#include "vector.h"

namespace EarthRotation {
  extern const int8_t NUTATION_ALPHA_COEFFICIENTS_MATRIX[106][5];
  extern const double NUTATION_AB_COEFFICIENTS_MATRIX[106][4];

  // Applies all conversions between the equatorial coordinate system (defined in
  // ReferenceFrame::EARTH_EQUATORIAL) and the fixed coordinate system (defined in
  // ReferenceFrame::EARTH_FIXED).
  Vector earthEquatorialToEarthFixed(Vector v, int64_t timeUtcMillis);

  // Finds the parameters delta-psi and delta-epsilon, used for nutation and sidereal rotation.
  // Both values are measured in arcseconds.
  std::pair<double, double> getDeltaPsiAndDeltaEpsilon(double timeJulianCenturiesSinceJ2000);
  // Applies precession to the given vector, based on the given time since J2000.
  Vector applyPrecession(Vector v, double timeJulianCenturiesSinceJ2000);
  // Applies nutation to the given vector, based on the given time since J2000 and parameters.
  Vector applyNutation(
      Vector v,
      std::pair<double, double> deltaPsiAndDeltaEpsilon,
      double timeJulianCenturiesSinceJ2000);
  // Applies sidereal rotation to the given vector, based on the given unix time and parameters.
  Vector applySiderealRotation(
    Vector v,
    std::pair<double, double> deltaPsiAndDeltaEpsilon,
    int64_t timeUtcMillis);
};

#endif
