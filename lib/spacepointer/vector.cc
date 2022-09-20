#include "vector.h"

#include <cmath>
#include <sstream>

Vector::Vector(double x, double y, double z) {
  this->x = x;
  this->y = y;
  this->z = z;
}

Vector Vector::normalized() const {
  double length = std::sqrt((x * x) + (y * y) + (z * z));
  if (length == 0) {
    return Vector(x, y, z);
  }
  return Vector(x / length, y / length, z / length);
}

double Vector::dotProduct(Vector other) const {
  return (x * other.x) + (y * other.y) + (z * other.z);
}

double Vector::angleRadians(Vector other) const {
  return std::acos(normalized().dotProduct(other.normalized()));
}

double Vector::angleDegrees(Vector other) const {
  return angleRadians(other) * 180.0 / M_PI;
}

Vector Vector::crossProduct(Vector other) const {
  return Vector(
    y * other.z - z * other.y,
    z * other.x - x * other.z,
    x * other.y - y * other.x
  );
}

double Vector::getX() const {
  return x;
}

double Vector::getY() const {
  return y;
}

double Vector::getZ() const {
  return z;
}

Vector Vector::operator+(Vector other) const {
  return Vector(
    x + other.x,
    y + other.y,
    z + other.z
  );
}

Vector Vector::operator-(Vector other) const {
  return Vector(
    x - other.x,
    y - other.y,
    z - other.z
  );
}

Vector Vector::operator*(double scale) const {
  return Vector(
    x * scale,
    y * scale,
    z * scale
  );
}

std::string Vector::toString() const {
  std::ostringstream ss;
  ss << "[" << x << ", " << y << ", " << z << "]";
  return ss.str();
}
