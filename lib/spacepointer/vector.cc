#include "vector.h"

#include <cmath>

Vector::Vector(double x, double y, double z) {
  this->x = x;
  this->y = y;
  this->z = z;
}

Vector Vector::normalized() {
  double length = std::sqrt((x * x) + (y * y) + (z * z));
  if (length == 0) {
    return Vector(x, y, z);
  }
  return Vector(x / length, y / length, z / length);
}

double Vector::dotProduct(Vector other) {
  return (x * other.x) + (y * other.y) + (z * other.z);
}

double Vector::angleRadians(Vector other) {
  return std::acos(normalized().dotProduct(other.normalized()));
}

double Vector::angleDegrees(Vector other) {
  return angleRadians(other) * 180.0 / M_PI;
}

Vector Vector::crossProduct(Vector other) {
  return Vector(
    y * other.z - z * other.y,
    z * other.x - x * other.z,
    x * other.y - y * other.x
  );
}

double Vector::getX() {
  return x;
}

double Vector::getY() {
  return y;
}

double Vector::getZ() {
  return z;
}

Vector Vector::operator+(Vector other) {
  return Vector(
    x + other.x,
    y + other.y,
    z + other.z
  );
}

Vector Vector::operator-(Vector other) {
  return Vector(
    x - other.x,
    y - other.y,
    z - other.z
  );
}