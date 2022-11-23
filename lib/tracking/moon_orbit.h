#ifndef COSMIC_SIGNPOST_LIB_TRACKING_MOON_ORBIT_H_
#define COSMIC_SIGNPOST_LIB_TRACKING_MOON_ORBIT_H_

#include <cstdint>

#include "cartesian_location.h"

namespace MoonOrbit {
  // The position of the Moon relative to the Earth at the given time, in ecliptic coordinates.
  CartesianLocation positionAt(int64_t timeMillis);

  // The position of the Earth-Moon barycentre relative to the Earth at the given time,
  // in ecliptic coordinates.
  CartesianLocation earthMoonBarycentreAt(int64_t timeMillis);
};

#endif
