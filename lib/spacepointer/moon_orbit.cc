#include "moon_orbit.h"

#include <cmath>
#include <cstdint>

#include "angle_utils.h"
#include "cartesian_location.h"
#include "keplerian_orbit.h"
#include "time_utils.h"

double EARTH_MASS = 5.97219e24; // kg
double MOON_MASS = 7.349e22; // kg
double EARTH_DISTANCE_SCALE = MOON_MASS / (EARTH_MASS + MOON_MASS);
double MOON_DISTANCE_SCALE = EARTH_MASS / (EARTH_MASS + MOON_MASS);

CartesianLocation MoonOrbit::positionAt(int64_t timeMillis) {
  double timeJulianDaysSinceJ2000 = daysSinceJ2000(timeMillis);
  double d = timeJulianDaysSinceJ2000;
  // TODO: find better time-based values for all of these orbital elements, based on d
  (void) d;
  // Osculating Orbital Elements from JPL Horizons:
  // https://ssd.jpl.nasa.gov/horizons/app.html
  // Target Body: Moon [Luna]
  // Coordinate Center: Geocentric [code: 500]
  // Time Specification: Start=2000-01-01 TDB, Stop=2000-01-02, Step=6 (hours)
  double semiMajorAxisMetres = 381'874'524.8499886;
  double eccentricity = 0.06314721685094304;
  double inclinationRadians = degreesToRadians(5.240273000309245);
  double longitudeOfAscendingNodeRadians = degreesToRadians(123.9580554371928);
  double argumentOfPeriapsisRadians = degreesToRadians(308.9226727206595);
  double meanAnomalyRadians = degreesToRadians(146.6732747774804);

  CartesianLocation moonFromEarth =
      KeplerianOrbit::findPosition(
          ReferenceFrame::EARTH_ECLIPTIC,
          semiMajorAxisMetres,
          eccentricity,
          inclinationRadians,
          longitudeOfAscendingNodeRadians,
          argumentOfPeriapsisRadians,
          meanAnomalyRadians);
  return moonFromEarth;
}

CartesianLocation MoonOrbit::earthMoonBarycentreAt(int64_t timeMillis) {
  CartesianLocation moonLocationFromEarth = positionAt(timeMillis);
  return CartesianLocation(
      moonLocationFromEarth.position * EARTH_DISTANCE_SCALE,
      ReferenceFrame::EARTH_ECLIPTIC);
}
