#ifndef SPACEPOINTER_LIB_SPACEPOINTER_VECTOR_H_
#define SPACEPOINTER_LIB_SPACEPOINTER_VECTOR_H_

class Vector {
  private:
    double x;
    double y;
    double z;

  public:
    Vector(double x, double y, double z);
    Vector normalized();
    double dotProduct(Vector other);
    Vector crossProduct(Vector other);
    double angleRadians(Vector other);
    double angleDegrees(Vector other);
    double getX();
    double getY();
    double getZ();
    Vector operator+(Vector other);
    Vector operator-(Vector other);
};

#endif
