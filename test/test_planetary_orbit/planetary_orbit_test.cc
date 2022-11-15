#include "planetary_orbit.h"

#include <gtest/gtest.h>

#include "angle_utils.h"
#include "time_utils.h"

TEST(PlanetaryOrbit, EarthMoonBarycentreAtJ2000) {
  CartesianLocation earthMoonBarycentreFromSun =
      PlanetaryOrbit::EARTH_MOON_BARYCENTRE.toCartesian(J2000_UTC_MILLIS);
  // Vector from JPL Horizons:
  // https://ssd.jpl.nasa.gov/horizons/app.html
  EXPECT_NEAR(earthMoonBarycentreFromSun.position.getX(), -2.650257688971310E+10, 1e7);
  EXPECT_NEAR(earthMoonBarycentreFromSun.position.getY(), 1.446939556279910E+11, 1e7);
  EXPECT_NEAR(earthMoonBarycentreFromSun.position.getZ(), -1.704331902042031E+05, 1e7);
  EXPECT_EQ(earthMoonBarycentreFromSun.referenceFrame, ReferenceFrame::SUN_ECLIPTIC);
}

TEST(PlanetaryOrbit, EarthMoonBarycentreOn15Nov2022) {
  CartesianLocation earthMoonBarycentreFromSun =
      PlanetaryOrbit::EARTH_MOON_BARYCENTRE.toCartesian(1668549600000); // 2022-11-15 22:00:00 GMT
  // Vector from JPL Horizons:
  // https://ssd.jpl.nasa.gov/horizons/app.html
  EXPECT_NEAR(earthMoonBarycentreFromSun.position.getX(), 8.863071355519846E+10, 1e8);
  EXPECT_NEAR(earthMoonBarycentreFromSun.position.getY(), 1.184864323351681E+11, 1e8);
  EXPECT_NEAR(earthMoonBarycentreFromSun.position.getZ(), -6.585665129043162E+06, 1e7);
  EXPECT_EQ(earthMoonBarycentreFromSun.referenceFrame, ReferenceFrame::SUN_ECLIPTIC);
}

TEST(PlanetaryOrbit, JupiterAtJ2000) {
  CartesianLocation jupiterFromSun =
      PlanetaryOrbit::JUPITER.toCartesian(J2000_UTC_MILLIS);
  // Vector from JPL Horizons:
  // https://ssd.jpl.nasa.gov/horizons/app.html
  EXPECT_NEAR(jupiterFromSun.position.getX(), 5.985675835979289E+11, 1e10);
  EXPECT_NEAR(jupiterFromSun.position.getY(), 4.396047284920244E+11, 1e10);
  EXPECT_NEAR(jupiterFromSun.position.getZ(), -1.522686065301856E+10, 1e9);
  EXPECT_EQ(jupiterFromSun.referenceFrame, ReferenceFrame::SUN_ECLIPTIC);
}

TEST(PlanetaryOrbit, JupiterOn15Nov2022) {
  CartesianLocation jupiterFromSun =
      PlanetaryOrbit::JUPITER.toCartesian(1668549600000); // 2022-11-15 22:00:00 GMT
  // Vector from JPL Horizons:
  // https://ssd.jpl.nasa.gov/horizons/app.html
  EXPECT_NEAR(jupiterFromSun.position.getX(), 7.335059705687681E+11, 1e10);
  EXPECT_NEAR(jupiterFromSun.position.getY(), 1.026704192447554E+11, 1e10);
  EXPECT_NEAR(jupiterFromSun.position.getZ(), -1.683736152794467E+10, 1e9);
  EXPECT_EQ(jupiterFromSun.referenceFrame, ReferenceFrame::SUN_ECLIPTIC);
}

#include "test_runner.inc"
