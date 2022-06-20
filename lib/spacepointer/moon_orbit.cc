#include "moon_orbit.h"

#include <cmath>
#include <cstdint>

#include "angle_utils.h"
#include "cartesian_location.h"
#include "keplerian_orbit.h"

CartesianLocation MoonOrbit::positionAt(int64_t timeMillis) {
  double semiMajorAxisMetres = 384400000;
  double eccentricity = 0.0554;
  double inclinationRadians = degreesToRadians(5.16);
  double longitudeOfAscendingNodeRadians = degreesToRadians(125.08);
  double argumentOfPeriapsisRadians = degreesToRadians(318.15);
  double meanAnomalyJ2000Radians = degreesToRadians(135.27);
  double periodJulianDays = 27.322;

  return KeplerianOrbit::findPosition(
      timeMillis,
      ReferenceFrame::EARTH_ECLIPTIC,
      semiMajorAxisMetres,
      eccentricity,
      inclinationRadians,
      longitudeOfAscendingNodeRadians,
      argumentOfPeriapsisRadians,
      meanAnomalyJ2000Radians,
      periodJulianDays);
}

CartesianLocation MoonOrbit::earthMoonBarycentreAt(int64_t timeMillis) {
  double earthMass = 5.97217e24; // kg
  double moonMass = 7.342e22; // kg
  double scale = moonMass / (moonMass + earthMass);
  CartesianLocation moonLocationFromEarth = positionAt(timeMillis);
  return CartesianLocation(
      moonLocationFromEarth.x * scale,
      moonLocationFromEarth.y * scale,
      moonLocationFromEarth.z * scale,
      ReferenceFrame::EARTH_ECLIPTIC);
}