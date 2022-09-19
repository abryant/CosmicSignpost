#include "keplerian_orbit.h"

#include <gtest/gtest.h>

#include "angle_utils.h"
#include "time_utils.h"

TEST(KeplerianOrbit, MoonIcrfAtJ2000) {
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
  // Vector from JPL Horizons:
  // https://ssd.jpl.nasa.gov/horizons/app.html
  EXPECT_NEAR(moonFromEarth.position.getX(), -291'608'384.1877129, 0.00001);
  EXPECT_NEAR(moonFromEarth.position.getY(), -274'979'741.6731504, 0.00001);
  EXPECT_NEAR(moonFromEarth.position.getZ(), 36'271'196.62699287, 0.00001);
  EXPECT_EQ(moonFromEarth.referenceFrame, ReferenceFrame::EARTH_ECLIPTIC);
}

TEST(KeplerianOrbit, JupiterAtJ2000) {
  // Osculating Orbital Elements from JPL Horizons:
  // https://ssd.jpl.nasa.gov/horizons/app.html
  // Target Body: Jupiter Barycenter
  // Coordinate Center: Solar System Barycenter (SSB) [500@0]
  // Time Specification: Start=2000-01-01 TDB, Stop=2000-01-02, Step=6 (hours)
  double semiMajorAxisMetres = 776248284430.2663;
  double eccentricity = 0.04760644163583022;
  double inclinationRadians = degreesToRadians(1.304245062486530);
  double longitudeOfAscendingNodeRadians = degreesToRadians(100.4855062837313);
  double argumentOfPeriapsisRadians = degreesToRadians(274.0367214870651);
  double meanAnomalyRadians = degreesToRadians(19.82968662680455);

  CartesianLocation jupiterFromSun =
      KeplerianOrbit::findPosition(
          ReferenceFrame::SUN_ECLIPTIC,
          semiMajorAxisMetres,
          eccentricity,
          inclinationRadians,
          longitudeOfAscendingNodeRadians,
          argumentOfPeriapsisRadians,
          meanAnomalyRadians);
  // Vector from JPL Horizons:
  // https://ssd.jpl.nasa.gov/horizons/app.html
  EXPECT_NEAR(jupiterFromSun.position.getX(), 597499876792.5479, 0.001);
  EXPECT_NEAR(jupiterFromSun.position.getY(), 439186453220.2049, 0.001);
  EXPECT_NEAR(jupiterFromSun.position.getZ(), -15195998835.76381, 0.001);
  EXPECT_EQ(jupiterFromSun.referenceFrame, ReferenceFrame::SUN_ECLIPTIC);
}

#include "test_runner.inc"
