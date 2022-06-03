#ifndef SPACEPOINTER_SRC_QUATERNION_H_
#define SPACEPOINTER_SRC_QUATERNION_H_

#include "vector.h"

class Quaternion {
  private:
    double a;
    double b;
    double c;
    double d;
    Quaternion(double a, double b, double c, double d);

  public:
    Quaternion(double angleRadians, Vector axis);

    Vector rotate(Vector v);
    Quaternion conjugate();
    Quaternion operator*(Quaternion other);
};

#endif
