#include "tracker.h"
#include "trackable_objects.h"

#include <gtest/gtest.h>
#include <cmath>

#include "time_utils.h"

// Stubbed satellite information for the ISS, data from Celestrak.
std::optional<std::string> fetchIssOmmMessage(std::string ignoredUrl) {
  return std::optional(R"""(
    [{
      "OBJECT_NAME": "ISS (ZARYA)",
      "OBJECT_ID": "1998-067A",
      "EPOCH": "2022-11-06T14:56:55.176576",
      "MEAN_MOTION": 15.49816683,
      "ECCENTRICITY": 0.0006494,
      "INCLINATION": 51.6453,
      "RA_OF_ASC_NODE": 350.9803,
      "ARG_OF_PERICENTER": 46.4928,
      "MEAN_ANOMALY": 41.5169,
      "EPHEMERIS_TYPE": 0,
      "CLASSIFICATION_TYPE": "U",
      "NORAD_CAT_ID": 25544,
      "ELEMENT_SET_NO": 999,
      "REV_AT_EPOCH": 36727,
      "BSTAR": 0.00031024,
      "MEAN_MOTION_DOT": 0.00017184,
      "MEAN_MOTION_DDOT": 0
    }]
  )""");
}

// Stubbed satellite information for SXM-8, data from Celestrak.
std::optional<std::string> fetchSxm8OmmMessage(std::string ignoredUrl) {
  return std::optional(R"""(
    [{
      "OBJECT_NAME": "SXM-8",
      "OBJECT_ID": "2021-049A",
      "EPOCH": "2022-11-03T23:06:20.151072",
      "MEAN_MOTION": 1.00269346,
      "ECCENTRICITY": 0.0001205,
      "INCLINATION": 0.0095,
      "RA_OF_ASC_NODE": 252.0821,
      "ARG_OF_PERICENTER": 135.3727,
      "MEAN_ANOMALY": 277.1172,
      "EPHEMERIS_TYPE": 0,
      "CLASSIFICATION_TYPE": "U",
      "NORAD_CAT_ID": 48838,
      "ELEMENT_SET_NO": 999,
      "REV_AT_EPOCH": 535,
      "BSTAR": 0,
      "MEAN_MOTION_DOT": -2.12e-6,
      "MEAN_MOTION_DDOT": 0
    }]
  )""");
}

TEST(Tracker, IssOn6thNovember2022) {
  bool initSuccess = TrackableObjects::initSatellites(fetchIssOmmMessage);
  EXPECT_EQ(initSuccess, true);
  Tracker tracker(Location(0, 0, 0), Direction(0, 0), TrackableObjects::getTrackingFunction("ISS"));
  Direction direction = tracker.getDirectionAt(1667757600000LL);
  // Observer data from JPL Horizons (ISS):
  EXPECT_NEAR(direction.getAzimuth(), 38.953368, 0.05);
  EXPECT_NEAR(direction.getAltitude(), -48.386143, 0.05);
}

TEST(Tracker, Sxm8On6thNovember2022) {
  bool initSuccess = TrackableObjects::initSatellites(fetchSxm8OmmMessage);
  EXPECT_EQ(initSuccess, true);
  Tracker tracker(Location(0, 0, 0), Direction(0, 0), TrackableObjects::getTrackingFunction("Sirius XM-8"));
  Direction direction = tracker.getDirectionAt(1667757600000LL);
  // Observer data from JPL Horizons (using the following custom TLE for SXM-8):
  // 1 48838U 21049A   22307.96273323 -.00000212  00000+0  00000+0 0  9990
  // 2 48838   0.0095 252.0821 0001205 135.3727 277.1172  1.00269346  5354
  EXPECT_NEAR(direction.getAzimuth(), 269.988828 - 360, 0.5);
  EXPECT_NEAR(direction.getAltitude(), -3.936714, 0.5);
}

TEST(Tracker, MoonAtJ2000) {
  Tracker tracker(Location(0, 0, 0), Direction(0, 0), TrackableObjects::getTrackingFunction("Moon"));
  Direction direction = tracker.getDirectionAt(J2000_UTC_MILLIS);
  EXPECT_NEAR(direction.getAzimuth(), 257.174602 - 360.0, 0.2);
  EXPECT_NEAR(direction.getAltitude(), 30.823721, 1);
}

TEST(Tracker, MoonIn2020) {
  Tracker tracker(Location(0, 0, 0), Direction(0, 0), TrackableObjects::getTrackingFunction("Moon"));
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
