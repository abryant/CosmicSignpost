#include "moon_orbit.h"

#include <gtest/gtest.h>

#include "angle_utils.h"
#include "time_utils.h"

TEST(MoonOrbit, MoonIcrfAtJ2000) {
  CartesianLocation moonFromEarth = MoonOrbit::positionAt(J2000_UTC_MILLIS);
  // Vector from JPL Horizons:
  // https://ssd.jpl.nasa.gov/horizons/app.html
  EXPECT_NEAR(moonFromEarth.position.getX(), -291'608'384.1877129, 0.00001);
  EXPECT_NEAR(moonFromEarth.position.getY(), -274'979'741.6731504, 0.00001);
  EXPECT_NEAR(moonFromEarth.position.getZ(), 36'271'196.62699287, 0.00001);
  EXPECT_EQ(moonFromEarth.referenceFrame, ReferenceFrame::EARTH_ECLIPTIC);
}

TEST(MoonOrbit, EarthMoonBarycenterIcrfAtJ2000) {
  CartesianLocation earthMoonBaryCenter = MoonOrbit::earthMoonBarycentreAt(J2000_UTC_MILLIS);
  // Vector from JPL Horizons:
  // https://ssd.jpl.nasa.gov/horizons/app.html
  EXPECT_NEAR(earthMoonBaryCenter.position.getX(), -3'543'212.282604189, 2000);
  EXPECT_NEAR(earthMoonBaryCenter.position.getY(), -3'341'164.558342924, 2000);
  EXPECT_NEAR(earthMoonBaryCenter.position.getZ(), 440'716.2357539921, 2000);
  EXPECT_EQ(earthMoonBaryCenter.referenceFrame, ReferenceFrame::EARTH_ECLIPTIC);
}

#include "test_runner.inc"
