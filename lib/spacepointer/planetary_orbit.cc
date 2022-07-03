#include "planetary_orbit.h"

#include <cmath>

#include "angle_utils.h"
#include "cartesian_location.h"
#include "error_utils.h"
#include "keplerian_orbit.h"
#include "reference_frame.h"
#include "time_utils.h"

const double METRES_PER_AU = 149597870700.0;

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

double PlanetaryOrbit::findEccentricAnomaly(double meanAnomaly, double eccentricity) const {
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

CartesianLocation PlanetaryOrbit::toCartesian(long timeMillis) const {
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


// Orbital elements taken from: https://ssd.jpl.nasa.gov/planets/approx_pos.html
const PlanetaryOrbit PlanetaryOrbit::MERCURY =
  PlanetaryOrbit(
      // J2000    delta per century
      0.38709843, 0.00000000, // semi-major axis
      0.20563661, 0.00002123, // eccentricity
      7.00559432, -0.00590158, // inclination
      252.25166724, 149472.67486623, // mean longitude
      77.45771895, 0.15940013, // longitude of perihelion
      48.33961819, -0.12214182, // longitude of ascending node
      // extra terms for outer planets
      0, 0, 0, 0
  );

const PlanetaryOrbit PlanetaryOrbit::VENUS =
  PlanetaryOrbit(
      // J2000    delta per century
      0.72332102, -0.00000026, // semi-major axis
      0.00676399, -0.00005107, // eccentricity
      3.39777545, 0.00043494, // inclination
      181.97970850, 58517.81560260, // mean longitude
      131.76755713, 0.05679648, // longitude of perihelion
      76.67261496, -0.27274174, // longitude of ascending node
      // extra terms for outer planets
      0, 0, 0, 0
  );

const PlanetaryOrbit PlanetaryOrbit::EARTH_MOON_BARYCENTRE =
  PlanetaryOrbit(
      // J2000    delta per century
      1.00000018, -0.00000003, // semi-major axis
      0.01673163, -0.00003661, // eccentricity
      -0.00054346, -0.01337178, // inclination
      100.46691572, 35999.37306329, // mean longitude
      102.93005885, 0.31795260, // longitude of perihelion
      -5.11260389, -0.24123856, // longitude of ascending node
      // extra terms for outer planets
      0, 0, 0, 0
  );

const PlanetaryOrbit PlanetaryOrbit::MARS =
  PlanetaryOrbit(
      // J2000    delta per century
      1.52371243, 0.00000097, // semi-major axis
      0.09336511, 0.00009149, // eccentricity
      1.85181869, -0.00724757, // inclination
      -4.56813164, 19140.29934243, // mean longitude
      -23.91744784, 0.45223625, // longitude of perihelion
      49.71320984, -0.26852431, // longitude of ascending node
      // extra terms for outer planets
      0, 0, 0, 0
  );

const PlanetaryOrbit PlanetaryOrbit::JUPITER =
  PlanetaryOrbit(
      // J2000    delta per century
      5.20248019, -0.00002864, // semi-major axis
      0.04853590, 0.00018026, // eccentricity
      1.29861416, -0.00322699, // inclination
      34.33479152, 3034.90371757, // mean longitude
      14.27495244, 0.18199196, // longitude of perihelion
      100.29282654, 0.13024619, // longitude of ascending node
      // extra terms for outer planets
      -0.00012452, 0.06064060, -0.35635438, 38.35125000
  );

const PlanetaryOrbit PlanetaryOrbit::SATURN =
  PlanetaryOrbit(
      // J2000    delta per century
      9.54149883, -0.00003065, // semi-major axis
      0.05550825, -0.00032044, // eccentricity
      2.49424102, 0.00451969, // inclination
      50.07571329, 1222.11494724, // mean longitude
      92.86136063, 0.54179478, // longitude of perihelion
      113.63998702, -0.25015002, // longitude of ascending node
      // extra terms for outer planets
      0.00025899, -0.13434469, 0.87320147, 38.35125000
  );

const PlanetaryOrbit PlanetaryOrbit::URANUS =
  PlanetaryOrbit(
      // J2000    delta per century
      19.18797948, -0.00020455, // semi-major axis
      0.04685740, -0.00001550, // eccentricity
      0.77298127, -0.00180155, // inclination
      314.20276625, 428.49512595, // mean longitude
      172.43404441, 0.09266985, // longitude of perihelion
      73.96250215, 0.05739699, // longitude of ascending node
      // extra terms for outer planets
      0.00058331, -0.97731848, 0.17689245, 7.67025000
  );

const PlanetaryOrbit PlanetaryOrbit::NEPTUNE =
  PlanetaryOrbit(
      // J2000    delta per century
      30.06952752, 0.00006447, // semi-major axis
      0.00895439, 0.00000818, // eccentricity
      1.77005520, 0.00022400, // inclination
      304.22289287, 218.46515314, // mean longitude
      46.68158724, 0.01009938, // longitude of perihelion
      131.78635853, -0.00606302, // longitude of ascending node
      // extra terms for outer planets
      -0.00041348, 0.68346318, -0.10162547, 7.67025000
  );
