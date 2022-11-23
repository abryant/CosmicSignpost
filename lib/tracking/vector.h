#ifndef COSMIC_SIGNPOST_LIB_TRACKING_VECTOR_H_
#define COSMIC_SIGNPOST_LIB_TRACKING_VECTOR_H_

#include <string>

class Vector {
  private:
    double x;
    double y;
    double z;

  public:
    Vector(double x, double y, double z);
    Vector normalized() const;
    double dotProduct(Vector other) const;
    Vector crossProduct(Vector other) const;
    double angleRadians(Vector other) const;
    double angleDegrees(Vector other) const;
    double getX() const;
    double getY() const;
    double getZ() const;
    double getLength() const;
    Vector operator+(Vector other) const;
    Vector operator-(Vector other) const;
    Vector operator*(double scale) const;
    std::string toString() const;
};

#endif
