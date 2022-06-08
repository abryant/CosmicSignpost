#include "cartesian_location.h"

#include <gtest/gtest.h>
#include <cmath>

TEST(CartesianLocation, Towards) {
  Vector v = CartesianLocation(5, 5, 5).towards(CartesianLocation(1, 2, 3));
  EXPECT_DOUBLE_EQ(v.getX(), -4.0 / sqrt(29));
  EXPECT_DOUBLE_EQ(v.getY(), -3.0 / sqrt(29));
  EXPECT_DOUBLE_EQ(v.getZ(), -2.0 / sqrt(29));
}

TEST(CartesianLocation, DirectionTowardsAbove_AtNorthPole) {
  Direction d = CartesianLocation(0, 0, 0).directionTowards(CartesianLocation(0, 0, 1), Vector(0, 0, 1));
  EXPECT_GE(d.getAzimuth(), -180);
  EXPECT_LT(d.getAzimuth(), 180);
  EXPECT_DOUBLE_EQ(d.getAltitude(), 90);
}

TEST(CartesianLocation, DirectionTowardsBelow_AtNorthPole) {
  Direction d = CartesianLocation(0, 0, 0).directionTowards(CartesianLocation(0, 0, -1), Vector(0, 0, 1));
  EXPECT_GE(d.getAzimuth(), -180);
  EXPECT_LT(d.getAzimuth(), 180);
  EXPECT_DOUBLE_EQ(d.getAltitude(), -90);
}

TEST(CartesianLocation, DirectionTowardsAbove_AtPrimeMeridian) {
  Direction d = CartesianLocation(0, 0, 0).directionTowards(CartesianLocation(1, 0, 0), Vector(1, 0, 0));
  EXPECT_GE(d.getAzimuth(), -180);
  EXPECT_LT(d.getAzimuth(), 180);
  EXPECT_DOUBLE_EQ(d.getAltitude(), 90);
}

TEST(CartesianLocation, DirectionTowardsAbove_AtEast) {
  Direction d = CartesianLocation(0, 0, 0).directionTowards(CartesianLocation(0, 1, 0), Vector(0, 1, 0));
  EXPECT_GE(d.getAzimuth(), -180);
  EXPECT_LT(d.getAzimuth(), 180);
  EXPECT_DOUBLE_EQ(d.getAltitude(), 90);
}

TEST(CartesianLocation, DirectionTowardsBelow_AtPrimeMeridian) {
  Direction d = CartesianLocation(0, 0, 0).directionTowards(CartesianLocation(-1, 0, 0), Vector(1, 0, 0));
  EXPECT_GE(d.getAzimuth(), -180);
  EXPECT_LT(d.getAzimuth(), 180);
  EXPECT_DOUBLE_EQ(d.getAltitude(), -90);
}

TEST(CartesianLocation, DirectionTowardsNorth_AtPrimeMeridian) {
  Direction d = CartesianLocation(0, 0, 0).directionTowards(CartesianLocation(0, 0, 1), Vector(1, 0, 0));
  EXPECT_DOUBLE_EQ(d.getAzimuth(), 0);
  EXPECT_DOUBLE_EQ(d.getAltitude(), 0);
}

TEST(CartesianLocation, DirectionTowardsEast_AtPrimeMeridian) {
  Direction d = CartesianLocation(0, 0, 0).directionTowards(CartesianLocation(0, 1, 0), Vector(1, 0, 0));
  EXPECT_DOUBLE_EQ(d.getAzimuth(), 90);
  EXPECT_DOUBLE_EQ(d.getAltitude(), 0);
}

TEST(CartesianLocation, DirectionTowardsSouth_AtPrimeMeridian) {
  Direction d = CartesianLocation(0, 0, 0).directionTowards(CartesianLocation(0, 0, -1), Vector(1, 0, 0));
  EXPECT_DOUBLE_EQ(d.getAzimuth(), -180);
  EXPECT_DOUBLE_EQ(d.getAltitude(), 0);
}

TEST(CartesianLocation, DirectionTowardsWest_AtPrimeMeridian) {
  Direction d = CartesianLocation(0, 0, 0).directionTowards(CartesianLocation(0, -1, 0), Vector(1, 0, 0));
  EXPECT_DOUBLE_EQ(d.getAzimuth(), -90);
  EXPECT_DOUBLE_EQ(d.getAltitude(), 0);
}

TEST(CartesianLocation, DirectionTowardsWest_AtEast) {
  Direction d = CartesianLocation(0, 0, 0).directionTowards(CartesianLocation(1, 0, 0), Vector(0, 1, 0));
  EXPECT_DOUBLE_EQ(d.getAzimuth(), -90);
  EXPECT_DOUBLE_EQ(d.getAltitude(), 0);
}

TEST(CartesianLocation, DirectionTowardsPrimeMeridian_AtEast) {
  Direction d = CartesianLocation(0, 1, 0).directionTowards(CartesianLocation(1, 0, 0), Vector(0, 1, 0));
  EXPECT_DOUBLE_EQ(d.getAzimuth(), -90);
  EXPECT_DOUBLE_EQ(d.getAltitude(), -45);
}

TEST(CartesianLocation, DirectionTowardsSouthernHemisphere_FromNorthernHemisphere) {
  Direction d = CartesianLocation(1, 1, 1).directionTowards(CartesianLocation(1, 1, -1), Vector(1, 1, 1));
  EXPECT_DOUBLE_EQ(d.getAzimuth(), -180);
  EXPECT_DOUBLE_EQ(d.getAltitude(), -35.264389682754654); // 90 - acos(-1/sqrt(3))
}

TEST(CartesianLocation, DirectionTowardsSouthPole_FromSouthernHemisphere) {
  Direction d = CartesianLocation(1, 1, -1).directionTowards(CartesianLocation(0, 0, -1), Vector(1, 1, -1));
  EXPECT_DOUBLE_EQ(d.getAzimuth(), -180);
  EXPECT_DOUBLE_EQ(d.getAltitude(), -54.735610317245374); // 90 - acos(-sqrt(2)/sqrt(3))
}

TEST(CartesianLocation, DirectionTowardsNorthWest_FromNorthernHemisphere) {
  Direction d = CartesianLocation(1, 1, 1).directionTowards(CartesianLocation(1, 0, 2), Vector(1, 1, 1));
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
  Direction d = CartesianLocation(1, 1, -1).directionTowards(CartesianLocation(0, 1, -2), Vector(1, 1, -1));
  // toOther is (-1/sqrt(2), 0, -1/sqrt(2))
  // rotation of up vector to prime meridian: 45 degrees around Z
  // => toOther becomes (-1/2, 1/2, -1/sqrt(2))
  // rotation of new up vector to equator: acos(sqrt(2)/sqrt(3)) ~= 35.26 degrees around Y
  // => toOther becomes (0, 1/2, -sqrt(3)/2)
  // => atan2(1, 0) - atan2(-sqrt(3)/2, 1/2) = 150
  EXPECT_DOUBLE_EQ(d.getAzimuth(), 150);
  EXPECT_DOUBLE_EQ(d.getAltitude(), 0);
}

#include "test_runner.inc"
