#include "earth_rotation.h"

#include <gtest/gtest.h>

#include "angle_utils.h"
#include "time_utils.h"

TEST(EarthRotation, ApplyPrecession_AtJ2000) {
  Vector result = EarthRotation::applyPrecession(Vector(1, 2, 3), 0.0);
  EXPECT_DOUBLE_EQ(result.getX(), 1);
  EXPECT_DOUBLE_EQ(result.getY(), 2);
  EXPECT_DOUBLE_EQ(result.getZ(), 3);
}

TEST(EarthRotation, ApplyPrecession_12HoursAfterJ2000) {
  double timeJulianCenturies = 0.5 / 36525; // Half a day in a Julian century.
  Vector result = EarthRotation::applyPrecession(Vector(1, 2, 3), timeJulianCenturies);
  EXPECT_NEAR(result.getX(), 1, 0.00001);
  EXPECT_NEAR(result.getY(), 2, 0.00001);
  EXPECT_NEAR(result.getZ(), 3, 0.00001);
}

TEST(EarthRotation, ApplyPrecession_OneCenturyAfterJ2000) {
  Vector result = EarthRotation::applyPrecession(Vector(2, 3, 6), 1.0);
  EXPECT_NEAR(result.getLength(), 7, 0.000000001);
}

TEST(EarthRotation, GetDeltaPsiAndDeltaEpsilon_AtJ2000) {
  std::pair<double, double> pair = EarthRotation::getDeltaPsiAndDeltaEpsilon(0.0);
  double deltaPsi = pair.first;
  double deltaEpsilon = pair.second;
  EXPECT_NEAR(deltaPsi, 0, 20);
  EXPECT_NEAR(deltaEpsilon, 0, 20);
}

TEST(EarthRotation, GetDeltaPsiAndDeltaEpsilon_12HoursAfterJ2000) {
  double timeJulianCenturies = 0.5 / 36525; // Half a day in a Julian century.
  std::pair<double, double> pair = EarthRotation::getDeltaPsiAndDeltaEpsilon(timeJulianCenturies);
  double deltaPsi = pair.first;
  double deltaEpsilon = pair.second;
  EXPECT_NEAR(deltaPsi, 0, 20);
  EXPECT_NEAR(deltaEpsilon, 0, 20);
}

TEST(EarthRotation, GetDeltaPsiAndDeltaEpsilon_OneCenturyAfterJ2000) {
  std::pair<double, double> pair = EarthRotation::getDeltaPsiAndDeltaEpsilon(1.0);
  double deltaPsi = pair.first;
  double deltaEpsilon = pair.second;
  EXPECT_NEAR(deltaPsi, 0, 20);
  EXPECT_NEAR(deltaEpsilon, 0, 20);
}

TEST(EarthRotation, ApplyNutation_AtJ2000) {
  std::pair<double, double> deltaPsiAndDeltaEpsilon = EarthRotation::getDeltaPsiAndDeltaEpsilon(0.0);
  Vector result = EarthRotation::applyNutation(Vector(1, 2, 3), deltaPsiAndDeltaEpsilon, 0.0);
  EXPECT_NEAR(result.getX(), 1, 0.001);
  EXPECT_NEAR(result.getY(), 2, 0.001);
  EXPECT_NEAR(result.getZ(), 3, 0.001);
}

TEST(EarthRotation, ApplyNutation_12HoursAfterJ2000) {
  double timeJulianCenturies = 0.5 / 36525; // Half a day in a Julian century.
  std::pair<double, double> deltaPsiAndDeltaEpsilon = EarthRotation::getDeltaPsiAndDeltaEpsilon(timeJulianCenturies);
  Vector result = EarthRotation::applyNutation(Vector(1, 2, 3), deltaPsiAndDeltaEpsilon, timeJulianCenturies);
  EXPECT_NEAR(result.getX(), 1, 0.001);
  EXPECT_NEAR(result.getY(), 2, 0.001);
  EXPECT_NEAR(result.getZ(), 3, 0.001);
}

TEST(EarthRotation, ApplyNutation_OneCenturyAfterJ2000) {
  std::pair<double, double> deltaPsiAndDeltaEpsilon = EarthRotation::getDeltaPsiAndDeltaEpsilon(1.0);
  Vector result = EarthRotation::applyNutation(Vector(2, 3, 6), deltaPsiAndDeltaEpsilon, 1.0);
  // TODO: find some test data
  EXPECT_NEAR(result.getLength(), 7, 0.000000001);
}

TEST(EarthRotation, ApplySiderealRotation_AtJ2000) {
  int64_t timeUtcMillis = approxUt1ToUnixTime(JANUARY_1_2000_12PM_UTC_MILLIS);
  std::pair<double, double> deltaPsiAndDeltaEpsilon = EarthRotation::getDeltaPsiAndDeltaEpsilon(0.0);
  Vector result = EarthRotation::applySiderealRotation(Vector(1, 2, 3), deltaPsiAndDeltaEpsilon, timeUtcMillis);
  EXPECT_NEAR(result.getX(), -1.77, 0.01);
  EXPECT_NEAR(result.getY(), 1.36, 0.01);
  EXPECT_NEAR(result.getZ(), 3, 0.01);
}

TEST(EarthRotation, ApplySiderealRotation_WhenRotationIsZero) {
  // This should be 1999-12-31 17:21:24 UTC according to Cosmographia.
  int64_t timeUtcMillis = approxUt1ToUnixTime(946660755000); // 1999-12-31 17:19:15
  std::pair<double, double> deltaPsiAndDeltaEpsilon = EarthRotation::getDeltaPsiAndDeltaEpsilon(0.0);
  Vector result = EarthRotation::applySiderealRotation(Vector(1, 2, 3), deltaPsiAndDeltaEpsilon, timeUtcMillis);
  EXPECT_NEAR(result.getX(), 1, 0.001);
  EXPECT_NEAR(result.getY(), 2, 0.001);
  EXPECT_NEAR(result.getZ(), 3, 0.001);
}

TEST(EarthRotation, ApplySiderealRotation_12HoursAfterJ2000) {
  double timeJulianCenturies = 0.5 / 36525; // Half a day in a Julian century.
  int64_t timeUtcMillis = approxUt1ToUnixTime(JANUARY_1_2000_12PM_UTC_MILLIS + (12 * 60 * 60 * 1000));
  std::pair<double, double> deltaPsiAndDeltaEpsilon = EarthRotation::getDeltaPsiAndDeltaEpsilon(timeJulianCenturies);
  Vector result = EarthRotation::applySiderealRotation(Vector(1, 2, 3), deltaPsiAndDeltaEpsilon, timeUtcMillis);
  EXPECT_NEAR(result.getX(), 1.76, 0.01);
  EXPECT_NEAR(result.getY(), -1.38, 0.01);
  EXPECT_NEAR(result.getZ(), 3, 0.001);
}

TEST(EarthRotation, ApplySiderealRotation_OneCenturyAfterJ2000) {
  int64_t timeUtcMillis = approxUt1ToUnixTime(JANUARY_1_2000_12PM_UTC_MILLIS + (36525LL * 24 * 60 * 60 * 1000));
  std::pair<double, double> deltaPsiAndDeltaEpsilon = EarthRotation::getDeltaPsiAndDeltaEpsilon(1.0);
  Vector result = EarthRotation::applySiderealRotation(Vector(2, 3, 6), deltaPsiAndDeltaEpsilon, timeUtcMillis);
  EXPECT_NEAR(result.getLength(), 7, 0.000000001);
}

TEST(EarthRotation, ApplySiderealRotation_ThreeMonthsLaterWhenRotationIsZero) {
  // This should be 2000-03-31 11:23:37 UTC according to Cosmographia.
  int64_t timeUtcMillis = approxUt1ToUnixTime(954501923000); // 2000-03-31 11:25:23 UTC
  double timeJulianCenturies = daysSinceJ2000(timeUtcMillis) / 36525.0;
  std::pair<double, double> deltaPsiAndDeltaEpsilon = EarthRotation::getDeltaPsiAndDeltaEpsilon(timeJulianCenturies);
  Vector result = EarthRotation::applySiderealRotation(Vector(1, 2, 3), deltaPsiAndDeltaEpsilon, timeUtcMillis);
  EXPECT_NEAR(result.getX(), 1, 0.001);
  EXPECT_NEAR(result.getY(), 2, 0.001);
  EXPECT_NEAR(result.getZ(), 3, 0.001);
}

TEST(EarthRotation, ApplySiderealRotation_SixMonthsLaterWhenRotationIsZero) {
  // This should be 2000-07-01 05:21:54 UTC according to Cosmographia.
  int64_t timeUtcMillis = approxUt1ToUnixTime(962429019000); // 2000-07-01 05:23:39 UTC
  double timeJulianCenturies = daysSinceJ2000(timeUtcMillis) / 36525.0;
  std::pair<double, double> deltaPsiAndDeltaEpsilon = EarthRotation::getDeltaPsiAndDeltaEpsilon(timeJulianCenturies);
  Vector result = EarthRotation::applySiderealRotation(Vector(1, 2, 3), deltaPsiAndDeltaEpsilon, timeUtcMillis);
  EXPECT_NEAR(result.getX(), 1, 0.001);
  EXPECT_NEAR(result.getY(), 2, 0.001);
  EXPECT_NEAR(result.getZ(), 3, 0.001);
}

#include "test_runner.inc"
