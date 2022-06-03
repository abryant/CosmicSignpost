#include "quaternion.h"

#include <gtest/gtest.h>
#include <cmath>

#include "vector.h"

const double EPSILON = 0.00001;

TEST(Quaternion, Create) {
  Quaternion(M_PI / 2, Vector(1, 0, 0));
}

TEST(Quaternion, RotateX) {
  Quaternion q = Quaternion(M_PI / 2, Vector(1, 0, 0));
  Vector v = q.rotate(Vector(1, 2, 3));
  EXPECT_NEAR(v.getX(), 1.0, EPSILON);
  EXPECT_NEAR(v.getY(), -3.0, EPSILON);
  EXPECT_NEAR(v.getZ(), 2.0, EPSILON);
}

TEST(Quaternion, RotateY) {
  Quaternion q = Quaternion(M_PI / 2, Vector(0, 1, 0));
  Vector v = q.rotate(Vector(1, 2, 3));
  EXPECT_NEAR(v.getX(), 3.0, EPSILON);
  EXPECT_NEAR(v.getY(), 2.0, EPSILON);
  EXPECT_NEAR(v.getZ(), -1.0, EPSILON);
}

TEST(Quaternion, RotateZ) {
  Quaternion q = Quaternion(M_PI / 2, Vector(0, 0, 1));
  Vector v = q.rotate(Vector(1, 2, 3));
  EXPECT_NEAR(v.getX(), -2.0, EPSILON);
  EXPECT_NEAR(v.getY(), 1.0, EPSILON);
  EXPECT_NEAR(v.getZ(), 3.0, EPSILON);
}

TEST(Quaternion, RotateDiagonal) {
  Quaternion q = Quaternion(2 * M_PI / 3, Vector(1, 1, 1));
  Vector v = q.rotate(Vector(1, 2, 3));
  EXPECT_NEAR(v.getX(), 3.0, EPSILON);
  EXPECT_NEAR(v.getY(), 1.0, EPSILON);
  EXPECT_NEAR(v.getZ(), 2.0, EPSILON);
}

TEST(Quaternion, ConjugateAndRotateDiagonal) {
  Quaternion q = Quaternion(2 * M_PI / 3, Vector(1, 1, 1)).conjugate();
  Vector v = q.rotate(Vector(3, 1, 2));
  EXPECT_NEAR(v.getX(), 1.0, EPSILON);
  EXPECT_NEAR(v.getY(), 2.0, EPSILON);
  EXPECT_NEAR(v.getZ(), 3.0, EPSILON);
}

TEST(Quaternion, MultiplyByConjugate) {
  Quaternion q1 = Quaternion(2 * M_PI / 3, Vector(1, 1, 1));
  Quaternion q2 = q1 * q1.conjugate();
  Vector v = q2.rotate(Vector(1, 2, 3));
  EXPECT_NEAR(v.getX(), 1.0, EPSILON);
  EXPECT_NEAR(v.getY(), 2.0, EPSILON);
  EXPECT_NEAR(v.getZ(), 3.0, EPSILON);
}

TEST(Quaternion, MultiplyByOther) {
  Quaternion q1 = Quaternion(2 * M_PI / 3, Vector(1, 1, 1));
  Quaternion q2 = q1 * Quaternion(M_PI / 2, Vector(1, 0, 0));
  // Rotate 90 degrees around X, and then by q1.
  Vector v = q2.rotate(Vector(1, 2, 3));
  EXPECT_NEAR(v.getX(), 2.0, EPSILON);
  EXPECT_NEAR(v.getY(), 1.0, EPSILON);
  EXPECT_NEAR(v.getZ(), -3.0, EPSILON);
}

#include "test_runner.inc"
