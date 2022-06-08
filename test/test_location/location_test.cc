#include "location.h"

#include <gtest/gtest.h>
#include <cmath>

const double EPSILON = 0.000000001;

TEST(Location, Create) {
  Location loc = Location(5, 10, 15);
  EXPECT_DOUBLE_EQ(loc.getLatitude(), 5);
  EXPECT_DOUBLE_EQ(loc.getLongitude(), 10);
  EXPECT_DOUBLE_EQ(loc.getElevation(), 15);
}

TEST(LocationDeathTest, Create_LatutudeNotInRange) {
  ASSERT_DEATH(
    Location(91, 10, 15),
    "latitude not in range");
}

TEST(Location, Create_WrapLongitudeForwards) {
  Location loc = Location(5, 181, 15);
  EXPECT_DOUBLE_EQ(loc.getLatitude(), 5);
  EXPECT_DOUBLE_EQ(loc.getLongitude(), -179);
  EXPECT_DOUBLE_EQ(loc.getElevation(), 15);
}

TEST(Location, Create_WrapLongitudeBackwards) {
  Location loc = Location(5, -541, 15);
  EXPECT_DOUBLE_EQ(loc.getLatitude(), 5);
  EXPECT_DOUBLE_EQ(loc.getLongitude(), 179);
  EXPECT_DOUBLE_EQ(loc.getElevation(), 15);
}

TEST(Location, GetCartesianAtEquator) {
  Location loc = Location(0, 0, 0);
  CartesianLocation c = loc.getCartesian();
  EXPECT_NEAR(c.x, 6378137, EPSILON);
  EXPECT_NEAR(c.y, 0, EPSILON);
  EXPECT_NEAR(c.z, 0, EPSILON);
}

TEST(Location, GetCartesianAtEquator_NonZeroLongitude) {
  Location loc = Location(0, 90, 0);
  CartesianLocation c = loc.getCartesian();
  EXPECT_NEAR(c.x, 0, EPSILON);
  EXPECT_NEAR(c.y, 6378137, EPSILON);
  EXPECT_NEAR(c.z, 0, EPSILON);
}

TEST(Location, GetCartesianAtNorthPole) {
  Location loc = Location(90, 0, 0);
  CartesianLocation c = loc.getCartesian();
  EXPECT_NEAR(c.x, 0, EPSILON);
  EXPECT_NEAR(c.y, 0, EPSILON);
  EXPECT_NEAR(c.z, 6356752.314, EPSILON);
}

TEST(Location, GetCartesianAt45Degrees) {
  Location loc = Location(45, 0, 0);
  CartesianLocation c = loc.getCartesian();
  EXPECT_NEAR(c.x, 4517590.8789357608, EPSILON);
  EXPECT_NEAR(c.y, 0, EPSILON);
  EXPECT_NEAR(c.z, 4487348.4086060142, EPSILON);
}

TEST(Location, GetCartesian_AllPositive) {
  Location loc = Location(5, 10, 1234);
  CartesianLocation c = loc.getCartesian();
  EXPECT_NEAR(c.x, 6258706.4889101423, EPSILON);
  EXPECT_NEAR(c.y, 1103578.8183300032, EPSILON);
  EXPECT_NEAR(c.z, 552291.5101718856, EPSILON);
}

TEST(Location, GetCartesian_AllNegative) {
  Location loc = Location(-85, -165, -1234);
  CartesianLocation c = loc.getCartesian();
  EXPECT_NEAR(c.x, -538638.40358762501, EPSILON);
  EXPECT_NEAR(c.y, -144327.72525369341, EPSILON);
  EXPECT_NEAR(c.z, -6331171.5594826229, EPSILON);
}

TEST(Location, GetNormal_X) {
  Location loc = Location(0, 0, 0);
  Vector v = loc.getNormal();
  EXPECT_NEAR(v.getX(), 1, EPSILON);
  EXPECT_NEAR(v.getY(), 0, EPSILON);
  EXPECT_NEAR(v.getZ(), 0, EPSILON);
}

TEST(Location, GetNormal_Y) {
  Location loc = Location(0, 90, 0);
  Vector v = loc.getNormal();
  EXPECT_NEAR(v.getX(), 0, EPSILON);
  EXPECT_NEAR(v.getY(), 1, EPSILON);
  EXPECT_NEAR(v.getZ(), 0, EPSILON);
}

TEST(Location, GetNormal_Z) {
  Location loc = Location(90, -180, 0);
  Vector v = loc.getNormal();
  EXPECT_NEAR(v.getX(), 0, EPSILON);
  EXPECT_NEAR(v.getY(), 0, EPSILON);
  EXPECT_NEAR(v.getZ(), 1, EPSILON);
}

TEST(Location, GetNormal_45) {
  Location loc = Location(45, 45, 9999999999);
  Vector v = loc.getNormal();
  EXPECT_NEAR(v.getX(), 0.5, EPSILON);
  EXPECT_NEAR(v.getY(), 0.5, EPSILON);
  EXPECT_NEAR(v.getZ(), 1/sqrt(2), EPSILON);
}

TEST(Location, GetNormal_AllPositive) {
  Location loc = Location(5, 10, 1234);
  Vector v = loc.getNormal();
  EXPECT_NEAR(v.getX(), 0.98106026219040698, EPSILON);
  EXPECT_NEAR(v.getY(), 0.17298739392508947, EPSILON);
  EXPECT_NEAR(v.getZ(), 0.08715574274765818, EPSILON);
}

TEST(Location, GetNormal_AllNegative) {
  Location loc = Location(-85, -165, -1234);
  Vector v = loc.getNormal();
  EXPECT_NEAR(v.getX(), -0.084185982829369149, EPSILON);
  EXPECT_NEAR(v.getY(), -0.022557566113149852, EPSILON);
  EXPECT_NEAR(v.getZ(), -0.99619469809174555, EPSILON);
}

TEST(Location, DirectionTo_Above) {
  Direction d = Location(0, 0, 0).directionTo(Location(0, 0, 123));
  EXPECT_NEAR(d.getAltitude(), 90, EPSILON);
}

TEST(Location, DirectionTo_Below) {
  Direction d = Location(0, 0, 0).directionTo(Location(0, 180, 0));
  EXPECT_NEAR(d.getAltitude(), -90, EPSILON);
}

TEST(Location, DirectionTo_East) {
  Direction d = Location(0, 0, 0).directionTo(Location(0, 90, 0));
  EXPECT_NEAR(d.getAzimuth(), 90, EPSILON);
  EXPECT_NEAR(d.getAltitude(), -45, EPSILON);
}

TEST(Location, DirectionTo_North) {
  Direction d = Location(0, 0, 0).directionTo(Location(90, 0, 0));
  EXPECT_NEAR(d.getAzimuth(), 0, EPSILON);
  EXPECT_NEAR(d.getAltitude(), -45.096212151684711, EPSILON);
}

TEST(Location, DirectionTo_SouthWest) {
  Direction d = Location(0, 0, 0).directionTo(Location(-45, -90, 0));
  EXPECT_NEAR(d.getAzimuth(), -134.80757678180817, EPSILON);
  EXPECT_NEAR(d.getAltitude(), -45.047863803488553, EPSILON);
}

#include "test_runner.inc"
