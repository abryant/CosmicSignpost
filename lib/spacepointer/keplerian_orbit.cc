#include "keplerian_orbit.h"

#include <cmath>
#include <cstdint>

#include "angle_utils.h"
#include "cartesian_location.h"
#include "error_utils.h"
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
  } while (std::abs(deltaE) < 1e-6);
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

  double sinArgumentOfPeriapsis = std::sin(argumentOfPeriapsisRadians);
  double cosArgumentOfPeriapsis = std::cos(argumentOfPeriapsisRadians);
  double sinLongitudeOfAscendingNode = std::sin(longitudeOfAscendingNodeRadians);
  double cosLongitudeOfAscendingNode = std::cos(longitudeOfAscendingNodeRadians);
  double sinInclination = std::sin(inclinationRadians);
  double cosInclination = std::cos(inclinationRadians);

  // ecliptic coordinates =
  // Rz(-longitudeOfAscendingNode)
  // * Rx(-inclination)
  // * Rz(-argumentOfPeriapsis)
  // * orbital plane coordinates

  // The following computes all three rotations at once:
  // (it would be neater as three rotation matrices)
  double x =
    orbitalPlaneX * (
      cosArgumentOfPeriapsis * cosLongitudeOfAscendingNode
      - sinArgumentOfPeriapsis * sinLongitudeOfAscendingNode * cosInclination)
    + orbitalPlaneY * (
      -sinArgumentOfPeriapsis * cosLongitudeOfAscendingNode
      - cosArgumentOfPeriapsis * sinLongitudeOfAscendingNode * cosInclination);
  double y =
    orbitalPlaneX * (
      cosArgumentOfPeriapsis * sinLongitudeOfAscendingNode
      + sinArgumentOfPeriapsis * cosLongitudeOfAscendingNode * cosInclination)
    + orbitalPlaneY * (
      -sinArgumentOfPeriapsis * sinLongitudeOfAscendingNode
      + cosArgumentOfPeriapsis * cosLongitudeOfAscendingNode * cosInclination);
  double z =
    orbitalPlaneX * (sinArgumentOfPeriapsis * sinInclination)
    + orbitalPlaneY * (cosArgumentOfPeriapsis * sinInclination);

  return CartesianLocation(x, y, z, referenceFrame);
}

CartesianLocation KeplerianOrbit::findPosition(
    int64_t timeMillis,
    ReferenceFrame referenceFrame,
    double semiMajorAxisMetres,
    double eccentricity,
    double inclinationRadians,
    double longitudeOfAscendingNodeRadians,
    double argumentOfPeriapsisRadians,
    double meanAnomalyJ2000Radians,
    double periodJulianDays) {
  double meanMotion = 2 * M_PI / periodJulianDays;
  double meanAnomaly = meanAnomalyJ2000Radians + daysSinceJ2000(timeMillis) * meanMotion;
  return findPosition(
    referenceFrame,
    semiMajorAxisMetres,
    eccentricity,
    inclinationRadians,
    longitudeOfAscendingNodeRadians,
    argumentOfPeriapsisRadians,
    meanAnomaly);
}
