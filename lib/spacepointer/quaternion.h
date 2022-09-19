#ifndef SPACEPOINTER_LIB_SPACEPOINTER_QUATERNION_H_
#define SPACEPOINTER_LIB_SPACEPOINTER_QUATERNION_H_

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
    static Quaternion rotateX(double angleRadians);
    static Quaternion rotateY(double angleRadians);
    static Quaternion rotateZ(double angleRadians);
};

#endif
