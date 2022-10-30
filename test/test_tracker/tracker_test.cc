#include "tracker.h"
#include "trackable_objects.h"

#include <gtest/gtest.h>
#include <cmath>

#include "time_utils.h"

TEST(Tracker, MoonAtJ2000) {
  Tracker tracker(Location(0, 0, 0), Direction(0, 0), TRACKABLE_OBJECTS.at("Moon"));
  Direction direction = tracker.getDirectionAt(J2000_UTC_MILLIS);
  EXPECT_NEAR(direction.getAzimuth(), 257.174602 - 360.0, 0.2);
  EXPECT_NEAR(direction.getAltitude(), 30.823721, 1);
}

TEST(Tracker, MoonIn2020) {
  Tracker tracker(Location(0, 0, 0), Direction(0, 0), TRACKABLE_OBJECTS.at("Moon"));
  Direction direction = tracker.getDirectionAt(1577880000000LL); // 2020-01-01 12:00:00 UTC
  EXPECT_NEAR(direction.getAzimuth(), 98.104074, 0.1);
  EXPECT_NEAR(direction.getAltitude(), 14.836674, 1.1);
}

TEST(Tracker, RaDeclToDirection) {
  // The Moon's location at J2000, according to JPL Horizons.
  std::function<CartesianLocation(int64_t)> f = [](int64_t millis) {
    return EquatorialLocation(221.66146, -10.99202).farCartesian();
  };
  Tracker tracker(Location(0, 0, 0), Direction(0, 0), f);
  Direction direction = tracker.getDirectionAt(J2000_UTC_MILLIS);
  EXPECT_NEAR(direction.getAzimuth(), 257.174602 - 360.0, 0.2);
  EXPECT_NEAR(direction.getAltitude(), 30.823721, 1.01);
}

#include "test_runner.inc"
