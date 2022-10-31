#include "sgp4_propagator.h"

#include <gtest/gtest.h>
#include <cmath>

const double EPSILON = 0.0000001;

TEST(SGP4, IssAtEpoch) {
  // From https://celestrak.org/NORAD/elements/gp.php?CATNR=25544&FORMAT=JSON-PRETTY on 2022-11-03
  OmmMessage omm {
    epoch: "2022-11-03T18:56:14.155",
    meanMotion: 15.49715636,
    eccentricity: 0.0006369,
    inclination: 51.6454,
    rightAscensionOfAscendingNode: 5.0131,
    argumentOfPericenter: 36.0986,
    meanAnomaly: 71.1952,
    bStarDragCoefficient: 0.00030321,
    meanMotionDot: 0.00016722,
    meanMotionDdot: 0,
  };
  SGP4::Sgp4OrbitalElements elements = SGP4::Sgp4OrbitalElements(omm);
  EXPECT_NEAR(elements.epoch, 26605.78905272996, EPSILON);
  SGP4::Sgp4State state = SGP4::initialiseSgp4(SGP4::WgsVersion::WGS_72, SGP4::OperationMode::IMPROVED, elements);
  SGP4::Sgp4Result result = SGP4::runSgp4(state, 0.0);
  EXPECT_EQ(result.code, SGP4::ResultCode::SUCCESS);
  EXPECT_NEAR(result.x, -2372.279647537571, EPSILON);
  EXPECT_NEAR(result.y, 3828.5069493048777, EPSILON);
  EXPECT_NEAR(result.z, 5078.075510966738, EPSILON);
  EXPECT_NEAR(result.vx, -7.16548809433362, EPSILON);
  EXPECT_NEAR(result.vy, -2.0502944837239263, EPSILON);
  EXPECT_NEAR(result.vz, -1.7911339375433932, EPSILON);
}

TEST(SGP4, IssAt45Minutes) {
  // From https://celestrak.org/NORAD/elements/gp.php?CATNR=25544&FORMAT=JSON-PRETTY on 2022-11-03
  OmmMessage omm {
    epoch: "2022-11-03T18:56:14.155",
    meanMotion: 15.49715636,
    eccentricity: 0.0006369,
    inclination: 51.6454,
    rightAscensionOfAscendingNode: 5.0131,
    argumentOfPericenter: 36.0986,
    meanAnomaly: 71.1952,
    bStarDragCoefficient: 0.00030321,
    meanMotionDot: 0.00016722,
    meanMotionDdot: 0,
  };
  SGP4::Sgp4OrbitalElements elements = SGP4::Sgp4OrbitalElements(omm);
  SGP4::Sgp4State state = SGP4::initialiseSgp4(SGP4::WgsVersion::WGS_72, SGP4::OperationMode::IMPROVED, elements);
  SGP4::Sgp4Result result = SGP4::runSgp4(state, 45.0);
  EXPECT_EQ(result.code, SGP4::ResultCode::SUCCESS);
  EXPECT_NEAR(result.x, 1720.481094581364, EPSILON);
  EXPECT_NEAR(result.y, -4004.195640124594, EPSILON);
  EXPECT_NEAR(result.z, -5222.493525981807, EPSILON);
  EXPECT_NEAR(result.vx, 7.38134215490661, EPSILON);
  EXPECT_NEAR(result.vy, 1.5929527045334955, EPSILON);
  EXPECT_NEAR(result.vz, 1.216698483961547, EPSILON);
}

TEST(SGP4, Sxm8AtEpoch) {
  // From https://celestrak.org/NORAD/elements/gp.php?CATNR=48838&FORMAT=JSON-PRETTY on 2022-11-03
  OmmMessage omm {
    epoch: "2022-11-03T14:29:55.224096",
    meanMotion: 1.00269391,
    eccentricity: 0.000122,
    inclination: 0.0124,
    rightAscensionOfAscendingNode: 247.4836,
    argumentOfPericenter: 141.8183,
    meanAnomaly: 145.816,
    bStarDragCoefficient: 0,
    meanMotionDot: -2.12e-6,
    meanMotionDdot: 0,
  };
  SGP4::Sgp4OrbitalElements elements = SGP4::Sgp4OrbitalElements(omm);
  SGP4::Sgp4State state = SGP4::initialiseSgp4(SGP4::WgsVersion::WGS_72, SGP4::OperationMode::IMPROVED, elements);
  SGP4::Sgp4Result result = SGP4::runSgp4(state, 0.0);
  EXPECT_EQ(result.code, SGP4::ResultCode::SUCCESS);
  EXPECT_NEAR(result.x, -42015.23421696562, EPSILON);
  EXPECT_NEAR(result.y, 3599.7575761200756, EPSILON);
  EXPECT_NEAR(result.z, -22.691802273104383, EPSILON);
  EXPECT_NEAR(result.vx, -0.2626550859221104, EPSILON);
  EXPECT_NEAR(result.vy, -3.0631402150650278, EPSILON);
  EXPECT_NEAR(result.vz, 0.000535319481297561, EPSILON);
}

TEST(SGP4, Sxm8At12Hours) {
  // From https://celestrak.org/NORAD/elements/gp.php?CATNR=48838&FORMAT=JSON-PRETTY on 2022-11-03
  OmmMessage omm {
    epoch: "2022-11-03T14:29:55.224096",
    meanMotion: 1.00269391,
    eccentricity: 0.000122,
    inclination: 0.0124,
    rightAscensionOfAscendingNode: 247.4836,
    argumentOfPericenter: 141.8183,
    meanAnomaly: 145.816,
    bStarDragCoefficient: 0,
    meanMotionDot: -2.12e-6,
    meanMotionDdot: 0,
  };
  SGP4::Sgp4OrbitalElements elements = SGP4::Sgp4OrbitalElements(omm);
  SGP4::Sgp4State state = SGP4::initialiseSgp4(SGP4::WgsVersion::WGS_72, SGP4::OperationMode::IMPROVED, elements);
  SGP4::Sgp4Result result = SGP4::runSgp4(state, 60 * 12);
  EXPECT_EQ(result.code, SGP4::ResultCode::SUCCESS);
  EXPECT_NEAR(result.x, 42034.990146580065, EPSILON);
  EXPECT_NEAR(result.y, -3252.3279194784614, EPSILON);
  EXPECT_NEAR(result.z, 22.48118341662265, EPSILON);
  EXPECT_NEAR(result.vx, 0.23700139063104916, EPSILON);
  EXPECT_NEAR(result.vy, 3.065857526699377, EPSILON);
  EXPECT_NEAR(result.vz, -0.0005421965687144481, EPSILON);
}

#include "test_runner.inc"
