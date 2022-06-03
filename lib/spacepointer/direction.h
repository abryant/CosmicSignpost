#ifndef SPACEPOINTER_SRC_DIRECTION_H_
#define SPACEPOINTER_SRC_DIRECTION_H_

class Direction {
  private:
    // [-180, 180)
    double azimuth;
    // [-90, 90]
    double altitude;

  public:
    Direction(double azimuth, double altitude);
    double getAzimuth();
    double getAltitude();
};

#endif
