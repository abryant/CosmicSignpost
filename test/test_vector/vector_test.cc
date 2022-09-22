#include "vector.h"

#include <gtest/gtest.h>
#include <cmath>

TEST(Vector, CreateAndGet) {
  Vector v = Vector(1, 2, 3);

  EXPECT_EQ(v.getX(), 1);
  EXPECT_EQ(v.getY(), 2);
  EXPECT_EQ(v.getZ(), 3);
}

TEST(Vector, NormalizeX) {
  Vector v = Vector(10, 0, 0).normalized();
  EXPECT_EQ(v.getX(), 1);
  EXPECT_EQ(v.getY(), 0);
  EXPECT_EQ(v.getZ(), 0);
}

TEST(Vector, NormalizeAll) {
  Vector v = Vector(1, 1, 1).normalized();
  EXPECT_DOUBLE_EQ(v.getX(), 1.0 / sqrt(3));
  EXPECT_DOUBLE_EQ(v.getY(), 1.0 / sqrt(3));
  EXPECT_DOUBLE_EQ(v.getZ(), 1.0 / sqrt(3));
}

TEST(Vector, DotProduct) {
  EXPECT_DOUBLE_EQ(Vector(2, 4, 8).dotProduct(Vector(1, 3, 5)), 54.0);
}

TEST(Vector, CrossProduct) {
  Vector v = Vector(2, 4, 8).crossProduct(Vector(1, 3, 5));
  EXPECT_DOUBLE_EQ(v.getX(), -4.0);
  EXPECT_DOUBLE_EQ(v.getY(), -2.0);
  EXPECT_DOUBLE_EQ(v.getZ(), 2.0);
}

TEST(Vector, AngleRadians) {
  EXPECT_DOUBLE_EQ(Vector(2, 0, 0).angleRadians(Vector(0, 0, 5)), M_PI / 2);
  EXPECT_DOUBLE_EQ(Vector(0, -1, 0).angleRadians(Vector(1, 0, 0)), M_PI / 2);
  EXPECT_DOUBLE_EQ(Vector(1, 1, 0).angleRadians(Vector(1, 0, 0)), M_PI / 4);
  EXPECT_DOUBLE_EQ(Vector(1, 0, 0).angleRadians(Vector(-1, -1, 0)), 3 * M_PI / 4);
  EXPECT_DOUBLE_EQ(Vector(0, 0, -1).angleRadians(Vector(0, 1, 1)), 3 * M_PI / 4);
}

TEST(Vector, AngleDegrees) {
  EXPECT_DOUBLE_EQ(Vector(2, 0, 0).angleDegrees(Vector(0, 0, 5)), 90.0);
  EXPECT_DOUBLE_EQ(Vector(0, -1, 0).angleDegrees(Vector(1, 0, 0)), 90.0);
  EXPECT_DOUBLE_EQ(Vector(1, 1, 0).angleDegrees(Vector(1, 0, 0)), 45.0);
  EXPECT_DOUBLE_EQ(Vector(1, 0, 0).angleDegrees(Vector(-1, -1, 0)), 135.0);
  EXPECT_DOUBLE_EQ(Vector(0, 0, -1).angleDegrees(Vector(0, 1, 1)), 135.0);
}

TEST(Vector, Add) {
  Vector v = Vector(2, 4, 8) + Vector(1, 3, 5);
  EXPECT_DOUBLE_EQ(v.getX(), 3.0);
  EXPECT_DOUBLE_EQ(v.getY(), 7.0);
  EXPECT_DOUBLE_EQ(v.getZ(), 13.0);
}

TEST(Vector, Subtract) {
  Vector v = Vector(2, 4, 8) - Vector(1, 3, 5);
  EXPECT_DOUBLE_EQ(v.getX(), 1.0);
  EXPECT_DOUBLE_EQ(v.getY(), 1.0);
  EXPECT_DOUBLE_EQ(v.getZ(), 3.0);
}

TEST(Vector, MultiplyDouble) {
  Vector v = Vector(2, 4, 8) * 3;
  EXPECT_DOUBLE_EQ(v.getX(), 6.0);
  EXPECT_DOUBLE_EQ(v.getY(), 12.0);
  EXPECT_DOUBLE_EQ(v.getZ(), 24.0);
}

TEST(Vector, Length) {
  EXPECT_DOUBLE_EQ(Vector(2, 3, 6).getLength(), 7.0);
  EXPECT_DOUBLE_EQ(Vector(9, 6, 2).getLength(), 11.0);
}

#include "test_runner.inc"
