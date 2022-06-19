#include "planetary_orbit.h"

#include <cmath>

#include "angle_utils.h"
#include "cartesian_location.h"
#include "error_utils.h"
#include "keplerian_orbit.h"
#include "reference_frame.h"
#include "time_utils.h"

PlanetaryOrbit::PlanetaryOrbit(
    double semiMajorAxisAuJ2000,
    double semiMajorAxisAuDelta,
    double eccentricityJ2000,
    double eccentricityDelta,
    double inclinationJ2000,
    double inclinationDelta,
    double meanLongitudeJ2000,
    double meanLongitudeDelta,
    double longitudeOfPerihelionJ2000,
    double longitudeOfPerihelionDelta,
    double longitudeOfAscendingNodeJ2000,
    double longitudeOfAscendingNodeDelta,
    double bTimeSquaredCoefficient,
    double cCosineCoefficient,
    double sSineCoefficient,
    double fFrequencyMultiplier) {
  this->semiMajorAxisJ2000 = semiMajorAxisAuJ2000 * METRES_PER_AU;
  this->semiMajorAxisDelta = semiMajorAxisAuDelta * METRES_PER_AU;
  this->eccentricityJ2000 = eccentricityJ2000;
  this->eccentricityDelta = eccentricityDelta;
  this->inclinationJ2000 = degreesToRadians(inclinationJ2000);
  this->inclinationDelta = degreesToRadians(inclinationDelta);
  this->meanLongitudeJ2000 = degreesToRadians(meanLongitudeJ2000);
  this->meanLongitudeDelta = degreesToRadians(meanLongitudeDelta);
  this->longitudeOfPerihelionJ2000 = degreesToRadians(longitudeOfPerihelionJ2000);
  this->longitudeOfPerihelionDelta = degreesToRadians(longitudeOfPerihelionDelta);
  this->longitudeOfAscendingNodeJ2000 = degreesToRadians(longitudeOfAscendingNodeJ2000);
  this->longitudeOfAscendingNodeDelta = degreesToRadians(longitudeOfAscendingNodeDelta);
  this->bTimeSquaredCoefficient = degreesToRadians(bTimeSquaredCoefficient);
  this->cCosineCoefficient = degreesToRadians(cCosineCoefficient);
  this->sSineCoefficient = degreesToRadians(sSineCoefficient);
  this->fFrequencyMultiplier = degreesToRadians(fFrequencyMultiplier);
}

double PlanetaryOrbit::findEccentricAnomaly(double meanAnomaly, double eccentricity) {
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

CartesianLocation PlanetaryOrbit::toCartesian(long timeMillis) {
  // Steps taken from: https://ssd.jpl.nasa.gov/planets/approx_pos.html
  long timeSinceJ2000Millis = timeMillis - J2000_UTC_MILLIS;
  double timeSinceJ2000Centuries = timeSinceJ2000Millis / 1000.0 / 60.0 / 60.0 / 24.0 / 36525.0;

  double semiMajorAxis = semiMajorAxisJ2000 + (timeSinceJ2000Centuries * semiMajorAxisDelta);
  double eccentricity = eccentricityJ2000 + (timeSinceJ2000Centuries * eccentricityDelta);
  double inclination = inclinationJ2000 + (timeSinceJ2000Centuries * inclinationDelta);
  double meanLongitude = meanLongitudeJ2000 + (timeSinceJ2000Centuries * meanLongitudeDelta);
  double longitudeOfPerihelion =
      longitudeOfPerihelionJ2000 + (timeSinceJ2000Centuries * longitudeOfPerihelionDelta);
  double longitudeOfAscendingNode =
      longitudeOfAscendingNodeJ2000 + (timeSinceJ2000Centuries * longitudeOfAscendingNodeDelta);

  double argumentOfPerihelion = longitudeOfPerihelion - longitudeOfAscendingNode;
  double meanAnomaly =
      meanLongitude
      - longitudeOfPerihelion
      + (bTimeSquaredCoefficient * timeSinceJ2000Centuries * timeSinceJ2000Centuries)
      + (cCosineCoefficient * std::cos(fFrequencyMultiplier * timeSinceJ2000Centuries))
      + (sSineCoefficient * std::sin(fFrequencyMultiplier * timeSinceJ2000Centuries));

  return KeplerianOrbit::findPosition(
      ReferenceFrame::SUN_ECLIPTIC,
      semiMajorAxis,
      eccentricity,
      inclination,
      longitudeOfAscendingNode,
      argumentOfPerihelion,
      meanAnomaly);
}
