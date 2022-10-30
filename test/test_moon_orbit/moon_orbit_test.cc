#include "moon_orbit.h"

#include <gtest/gtest.h>

#include "angle_utils.h"
#include "time_utils.h"

TEST(MoonOrbit, MoonIcrfAtJ2000) {
  CartesianLocation moonFromEarth = MoonOrbit::positionAt(J2000_UTC_MILLIS);
  // Vector from JPL Horizons:
  // https://ssd.jpl.nasa.gov/horizons/app.html
  EXPECT_NEAR(moonFromEarth.position.getX(), -291608384.1877129, 200000);
  EXPECT_NEAR(moonFromEarth.position.getY(), -274979741.6731504, 200000);
  EXPECT_NEAR(moonFromEarth.position.getZ(), 36271196.62699287, 200000);
  EXPECT_EQ(moonFromEarth.referenceFrame, ReferenceFrame::EARTH_ECLIPTIC);
}

TEST(MoonOrbit, EarthMoonBarycenterIcrfAtJ2000) {
  CartesianLocation earthMoonBaryCenter = MoonOrbit::earthMoonBarycentreAt(J2000_UTC_MILLIS);
  // Vector from JPL Horizons:
  // https://ssd.jpl.nasa.gov/horizons/app.html
  EXPECT_NEAR(earthMoonBaryCenter.position.getX(), -3543212.282604189, 3000);
  EXPECT_NEAR(earthMoonBaryCenter.position.getY(), -3341164.558342924, 3000);
  EXPECT_NEAR(earthMoonBaryCenter.position.getZ(), 440716.2357539921, 3000);
  EXPECT_EQ(earthMoonBaryCenter.referenceFrame, ReferenceFrame::EARTH_ECLIPTIC);
}

#include "test_runner.inc"
