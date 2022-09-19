#include "quaternion.h"

#include <cmath>

Quaternion::Quaternion(double a, double b, double c, double d) {
  this->a = a;
  this->b = b;
  this->c = c;
  this->d = d;
}

Quaternion::Quaternion(double angleRadians, Vector axis) {
  double halfAngleRadians = angleRadians / 2.0;
  axis = axis.normalized();
  this->a = std::cos(halfAngleRadians);
  double sinHalfAngle = std::sin(halfAngleRadians);
  this->b = axis.getX() * sinHalfAngle;
  this->c = axis.getY() * sinHalfAngle;
  this->d = axis.getZ() * sinHalfAngle;
}

Vector Quaternion::rotate(Vector v) {
  Quaternion subject = Quaternion(0.0, v.getX(), v.getY(), v.getZ());
  Quaternion result = (*this) * subject * conjugate();
  return Vector(result.b, result.c, result.d);
}

Quaternion Quaternion::conjugate() {
  return Quaternion(a, -b, -c, -d);
}

Quaternion Quaternion::operator*(Quaternion other) {
  return Quaternion(
    a * other.a - b * other.b - c * other.c - d * other.d,
    a * other.b + b * other.a + c * other.d - d * other.c,
    a * other.c - b * other.d + c * other.a + d * other.b,
    a * other.d + b * other.c - c * other.b + d * other.a
  );
}

Quaternion Quaternion::rotateX(double angleRadians) {
  return Quaternion(angleRadians, Vector(1, 0, 0));
}

Quaternion Quaternion::rotateY(double angleRadians) {
  return Quaternion(angleRadians, Vector(0, 1, 0));
}

Quaternion Quaternion::rotateZ(double angleRadians) {
  return Quaternion(angleRadians, Vector(0, 0, 1));
}
