#ifndef COSMIC_SIGNPOST_LIB_TRACKING_CARTESIAN_LOCATION_H_
#define COSMIC_SIGNPOST_LIB_TRACKING_CARTESIAN_LOCATION_H_

#include <cstdint>

#include "direction.h"
#include "reference_frame.h"
#include "vector.h"

class CartesianLocation {
  public:
    // X,Y,Z coordinates in metres. The meanings of X, Y, and Z depend on the reference frame.
    const Vector position;
    const ReferenceFrame referenceFrame;

    CartesianLocation(Vector position, ReferenceFrame referenceFrame);
    static CartesianLocation fixed(Vector position);

    Vector towards(CartesianLocation other);
    Direction directionTowards(CartesianLocation other, Vector up);

    CartesianLocation toFixed(int64_t timeMillis);

    std::string toString();
};

#endif
