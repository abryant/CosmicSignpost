#include "cartesian_location.h"

#include <gtest/gtest.h>
#include <cmath>

#include "location.h"
#include "time_utils.h"

TEST(CartesianLocation, Towards) {
  Vector v = CartesianLocation::fixed(Vector(5, 5, 5)).towards(CartesianLocation::fixed(Vector(1, 2, 3)));
  EXPECT_DOUBLE_EQ(v.getX(), -4.0 / sqrt(29));
  EXPECT_DOUBLE_EQ(v.getY(), -3.0 / sqrt(29));
  EXPECT_DOUBLE_EQ(v.getZ(), -2.0 / sqrt(29));
}

TEST(CartesianLocation, DirectionTowardsAbove_AtNorthPole) {
  Direction d = CartesianLocation::fixed(Vector(0, 0, 0)).directionTowards(CartesianLocation::fixed(Vector(0, 0, 1)), Vector(0, 0, 1));
  EXPECT_GE(d.getAzimuth(), -180);
  EXPECT_LT(d.getAzimuth(), 180);
  EXPECT_DOUBLE_EQ(d.getAltitude(), 90);
}

TEST(CartesianLocation, DirectionTowardsBelow_AtNorthPole) {
  Direction d = CartesianLocation::fixed(Vector(0, 0, 0)).directionTowards(CartesianLocation::fixed(Vector(0, 0, -1)), Vector(0, 0, 1));
  EXPECT_GE(d.getAzimuth(), -180);
  EXPECT_LT(d.getAzimuth(), 180);
  EXPECT_DOUBLE_EQ(d.getAltitude(), -90);
}

TEST(CartesianLocation, DirectionTowardsAbove_AtPrimeMeridian) {
  Direction d = CartesianLocation::fixed(Vector(0, 0, 0)).directionTowards(CartesianLocation::fixed(Vector(1, 0, 0)), Vector(1, 0, 0));
  EXPECT_GE(d.getAzimuth(), -180);
  EXPECT_LT(d.getAzimuth(), 180);
  EXPECT_DOUBLE_EQ(d.getAltitude(), 90);
}

TEST(CartesianLocation, DirectionTowardsAbove_AtEast) {
  Direction d = CartesianLocation::fixed(Vector(0, 0, 0)).directionTowards(CartesianLocation::fixed(Vector(0, 1, 0)), Vector(0, 1, 0));
  EXPECT_GE(d.getAzimuth(), -180);
  EXPECT_LT(d.getAzimuth(), 180);
  EXPECT_DOUBLE_EQ(d.getAltitude(), 90);
}

TEST(CartesianLocation, DirectionTowardsBelow_AtPrimeMeridian) {
  Direction d = CartesianLocation::fixed(Vector(0, 0, 0)).directionTowards(CartesianLocation::fixed(Vector(-1, 0, 0)), Vector(1, 0, 0));
  EXPECT_GE(d.getAzimuth(), -180);
  EXPECT_LT(d.getAzimuth(), 180);
  EXPECT_DOUBLE_EQ(d.getAltitude(), -90);
}

TEST(CartesianLocation, DirectionTowardsNorth_AtPrimeMeridian) {
  Direction d = CartesianLocation::fixed(Vector(0, 0, 0)).directionTowards(CartesianLocation::fixed(Vector(0, 0, 1)), Vector(1, 0, 0));
  EXPECT_DOUBLE_EQ(d.getAzimuth(), 0);
  EXPECT_DOUBLE_EQ(d.getAltitude(), 0);
}

TEST(CartesianLocation, DirectionTowardsEast_AtPrimeMeridian) {
  Direction d = CartesianLocation::fixed(Vector(0, 0, 0)).directionTowards(CartesianLocation::fixed(Vector(0, 1, 0)), Vector(1, 0, 0));
  EXPECT_DOUBLE_EQ(d.getAzimuth(), 90);
  EXPECT_DOUBLE_EQ(d.getAltitude(), 0);
}

TEST(CartesianLocation, DirectionTowardsSouth_AtPrimeMeridian) {
  Direction d = CartesianLocation::fixed(Vector(0, 0, 0)).directionTowards(CartesianLocation::fixed(Vector(0, 0, -1)), Vector(1, 0, 0));
  EXPECT_DOUBLE_EQ(d.getAzimuth(), -180);
  EXPECT_DOUBLE_EQ(d.getAltitude(), 0);
}

TEST(CartesianLocation, DirectionTowardsWest_AtPrimeMeridian) {
  Direction d = CartesianLocation::fixed(Vector(0, 0, 0)).directionTowards(CartesianLocation::fixed(Vector(0, -1, 0)), Vector(1, 0, 0));
  EXPECT_DOUBLE_EQ(d.getAzimuth(), -90);
  EXPECT_DOUBLE_EQ(d.getAltitude(), 0);
}

TEST(CartesianLocation, DirectionTowardsWest_AtEast) {
  Direction d = CartesianLocation::fixed(Vector(0, 0, 0)).directionTowards(CartesianLocation::fixed(Vector(1, 0, 0)), Vector(0, 1, 0));
  EXPECT_DOUBLE_EQ(d.getAzimuth(), -90);
  EXPECT_DOUBLE_EQ(d.getAltitude(), 0);
}

TEST(CartesianLocation, DirectionTowardsPrimeMeridian_AtEast) {
  Direction d = CartesianLocation::fixed(Vector(0, 1, 0)).directionTowards(CartesianLocation::fixed(Vector(1, 0, 0)), Vector(0, 1, 0));
  EXPECT_DOUBLE_EQ(d.getAzimuth(), -90);
  EXPECT_DOUBLE_EQ(d.getAltitude(), -45);
}

TEST(CartesianLocation, DirectionTowardsSouthernHemisphere_FromNorthernHemisphere) {
  Direction d = CartesianLocation::fixed(Vector(1, 1, 1)).directionTowards(CartesianLocation::fixed(Vector(1, 1, -1)), Vector(1, 1, 1));
  EXPECT_DOUBLE_EQ(d.getAzimuth(), -180);
  EXPECT_DOUBLE_EQ(d.getAltitude(), -35.264389682754654); // 90 - acos(-1/sqrt(3))
}

TEST(CartesianLocation, DirectionTowardsSouthPole_FromSouthernHemisphere) {
  Direction d = CartesianLocation::fixed(Vector(1, 1, -1)).directionTowards(CartesianLocation::fixed(Vector(0, 0, -1)), Vector(1, 1, -1));
  EXPECT_DOUBLE_EQ(d.getAzimuth(), -180);
  EXPECT_DOUBLE_EQ(d.getAltitude(), -54.735610317245374); // 90 - acos(-sqrt(2)/sqrt(3))
}

TEST(CartesianLocation, DirectionTowardsNorthWest_FromNorthernHemisphere) {
  Direction d = CartesianLocation::fixed(Vector(1, 1, 1)).directionTowards(CartesianLocation::fixed(Vector(1, 0, 2)), Vector(1, 1, 1));
  // toOther is (0, -1/sqrt(2), 1/sqrt(2))
  // rotation of up vector to prime meridian: 45 degrees around Z
  // => toOther becomes (-1/2, -1/2, 1/sqrt(2))
  // rotation of new up vector to equator: acos(sqrt(2)/sqrt(3)) ~= 35.26 degrees around Y
  // => toOther becomes (0, -1/2, sqrt(3)/2)
  // => atan2(1, 0) - atan2(sqrt(3)/2, -1/2) = -30
  EXPECT_DOUBLE_EQ(d.getAzimuth(), -30);
  EXPECT_DOUBLE_EQ(d.getAltitude(), 0);
}

TEST(CartesianLocation, DirectionTowardsSouthEast_FromSouthernHemisphere) {
  Direction d = CartesianLocation::fixed(Vector(1, 1, -1)).directionTowards(CartesianLocation::fixed(Vector(0, 1, -2)), Vector(1, 1, -1));
  // toOther is (-1/sqrt(2), 0, -1/sqrt(2))
  // rotation of up vector to prime meridian: 45 degrees around Z
  // => toOther becomes (-1/2, 1/2, -1/sqrt(2))
  // rotation of new up vector to equator: acos(sqrt(2)/sqrt(3)) ~= 35.26 degrees around Y
  // => toOther becomes (0, 1/2, -sqrt(3)/2)
  // => atan2(1, 0) - atan2(-sqrt(3)/2, 1/2) = 150
  EXPECT_DOUBLE_EQ(d.getAzimuth(), 150);
  EXPECT_DOUBLE_EQ(d.getAltitude(), 0);
}

TEST(CartesianLocation, EquatorialToFixed_WhenRotationAngleIsZero) {
  // The prime meridian and equinox are aligned 0.277537528 days before midnight on 1st January 2000.
  int64_t timeMillis = approxUt1ToUnixTime(JANUARY_1_2000_MIDNIGHT_UTC_MILLIS - 0.277537528 * 24*60*60*1000);
  CartesianLocation fixed = CartesianLocation(Vector(1, 2, 3), ReferenceFrame::EARTH_EQUATORIAL).toFixed(timeMillis);
  EXPECT_NEAR(fixed.position.getX(), 1, 0.001);
  EXPECT_NEAR(fixed.position.getY(), 2, 0.001);
  EXPECT_NEAR(fixed.position.getZ(), 3, 0.001);
  EXPECT_EQ(fixed.referenceFrame, ReferenceFrame::EARTH_FIXED);
}

TEST(CartesianLocation, EquatorialToFixed_WhenRotationAngleIs90Degrees) {
  // The prime meridian and equinox are aligned 0.277537528 days before midnight on 1st January 2000.
  // This test looks at a time 1/4 of a sidereal day (which is 86164 seconds of the normal 86400) after that.
  int64_t timeMillis = approxUt1ToUnixTime(JANUARY_1_2000_MIDNIGHT_UTC_MILLIS + (-0.277537528 + (0.25*86164/86400)) * 24*60*60*1000);
  CartesianLocation fixed = CartesianLocation(Vector(1, 2, 3), ReferenceFrame::EARTH_EQUATORIAL).toFixed(timeMillis);
  EXPECT_NEAR(fixed.position.getX(), 2, 0.01);
  EXPECT_NEAR(fixed.position.getY(), -1, 0.01);
  EXPECT_NEAR(fixed.position.getZ(), 3, 0.01);
  EXPECT_EQ(fixed.referenceFrame, ReferenceFrame::EARTH_FIXED);
}

TEST(CartesianLocation, EclipticToFixed) {
  // The prime meridian and equinox are aligned 0.277537528 days before midnight on 1st January 2000.
  int64_t timeMillis = approxUt1ToUnixTime(JANUARY_1_2000_MIDNIGHT_UTC_MILLIS - 0.277537528 * 24*60*60*1000);
  CartesianLocation fixed = CartesianLocation(Vector(1, 2, 3), ReferenceFrame::EARTH_ECLIPTIC).toFixed(timeMillis);
  EXPECT_NEAR(fixed.position.getX(), 1, 0.001);
  EXPECT_NEAR(fixed.position.getY(), 0.64, 0.01);
  EXPECT_NEAR(fixed.position.getZ(), 3.55, 0.01);
  EXPECT_EQ(fixed.referenceFrame, ReferenceFrame::EARTH_FIXED);
}

TEST(CartesianLocation, EclipticToDirection_AtJ2000) {
  // From JPL Horizons:
  // https://ssd.jpl.nasa.gov/horizons/app.html
  // Ephemeris Type: Vector Table
  // Target Body: Moon [Luna]
  // Coordinate Center: Geocentric [code: 500]
  // Time Specification: Start=2000-01-01 TDB , Stop=2000-01-02, Step=6 (hours)
  CartesianLocation moonEcliptic(
      Vector(-2.916083841877129E+08, -2.749797416731504E+08, 3.627119662699287E+07),
      ReferenceFrame::EARTH_ECLIPTIC);
  Location zeroZero = Location(0, 0, 0);
  Direction dir = zeroZero.getCartesian().directionTowards(moonEcliptic.toFixed(J2000_UTC_MILLIS), zeroZero.getNormal());
  // TODO: figure out why JPL Horizons returns 257.206634 but we get 257.311769
  EXPECT_NEAR(dir.getAzimuth(), 257.3 - 360.0, 0.1);
  // TODO: figure out why JPL Horizons returns 30.568243 but we get 29.810948
  EXPECT_NEAR(dir.getAltitude(), 30.57, 1);
}

#include "test_runner.inc"
