#ifndef SPACEPOINTER_SRC_CARTESIAN_LOCATION_H_
#define SPACEPOINTER_SRC_CARTESIAN_LOCATION_H_

#include "direction.h"
#include "reference_frame.h"
#include "vector.h"

class CartesianLocation {
  public:
    // X coordinate in metres.
    // The X axis points towards the prime meridian at the equator.
    const double x;
    // Y coordinate in metres.
    // The Y axis points towards 90 degrees east of the prime meridian at the equator.
    const double y;
    // Z coordinate in metres.
    // The Z axis points towards the north pole.
    const double z;

    const ReferenceFrame referenceFrame;

    CartesianLocation(double x, double y, double z, ReferenceFrame referenceFrame);
    static CartesianLocation fixed(double x, double y, double z);

    Vector towards(CartesianLocation other);
    Direction directionTowards(CartesianLocation other, Vector up);
};

#endif
