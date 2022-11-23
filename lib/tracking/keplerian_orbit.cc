#include "keplerian_orbit.h"

#include <cmath>
#include <cstdint>

#include "angle_utils.h"
#include "cartesian_location.h"
#include "error_utils.h"
#include "quaternion.h"
#include "time_utils.h"

double KeplerianOrbit::findEccentricAnomaly(double meanAnomaly, double eccentricity) {
  // Solve Kepler's equation using the Newton-Raphson method.
  double eccentricAnomaly = meanAnomaly + (eccentricity * std::sin(meanAnomaly));
  double deltaE;
  do {
    // F(En) = M - En + e * sin(En)
    double fE = meanAnomaly - eccentricAnomaly + (eccentricity * std::sin(eccentricAnomaly));
    // F'(En) = -1 + e * cos(En)
    double fPrimeE = -1.0 + eccentricity * std::cos(eccentricAnomaly);
    // En+1 = En - F(En) / F'(En)
    deltaE = fE / fPrimeE;
    eccentricAnomaly = eccentricAnomaly - deltaE;
  } while (std::abs(deltaE) > 1e-6);
  return eccentricAnomaly;
}

CartesianLocation KeplerianOrbit::findPosition(
    ReferenceFrame referenceFrame,
    double semiMajorAxisMetres,
    double eccentricity,
    double inclinationRadians,
    double longitudeOfAscendingNodeRadians,
    double argumentOfPeriapsisRadians,
    double meanAnomalyRadians) {
  checkArgument(
    referenceFrame != ReferenceFrame::EARTH_FIXED,
    "Cannot use the non-inertial reference frame EARTH_FIXED to calculate a keplerian position");

  meanAnomalyRadians = wrapRadians(meanAnomalyRadians);

  // Steps taken from: https://ssd.jpl.nasa.gov/planets/approx_pos.html
  double eccentricAnomaly = findEccentricAnomaly(meanAnomalyRadians, eccentricity);

  double orbitalPlaneX = semiMajorAxisMetres * (std::cos(eccentricAnomaly) - eccentricity);
  double orbitalPlaneY =
      semiMajorAxisMetres * std::sqrt(1.0 - (eccentricity * eccentricity)) * std::sin(eccentricAnomaly);

  // ecliptic coordinates =
  // Rz(-longitudeOfAscendingNode)
  // * Rx(-inclination)
  // * Rz(-argumentOfPeriapsis)
  // * orbital plane coordinates

  Quaternion ascendingNodeRotation = Quaternion::rotateZ(longitudeOfAscendingNodeRadians);
  Quaternion inclinationRotation = Quaternion::rotateX(inclinationRadians);
  Quaternion argumentOfPeriapsisRotation = Quaternion::rotateZ(argumentOfPeriapsisRadians);
  Quaternion fullRotation =
      ascendingNodeRotation * inclinationRotation * argumentOfPeriapsisRotation;
  Vector locationInOrbitalPlane(orbitalPlaneX, orbitalPlaneY, 0);
  Vector result = fullRotation.rotate(locationInOrbitalPlane);
  return CartesianLocation(result, referenceFrame);
}
