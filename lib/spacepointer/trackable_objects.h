#ifndef SPACEPOINTER_LIB_SPACEPOINTER_TRACKABLE_OBJECTS_H_
#define SPACEPOINTER_LIB_SPACEPOINTER_TRACKABLE_OBJECTS_H_

#include <functional>
#include <optional>
#include <map>
#include <string>

#include "equatorial_location.h"
#include "location.h"
#include "moon_orbit.h"
#include "planetary_orbit.h"
#include "satellite_orbit.h"

namespace TrackableObjects {

  typedef std::function<CartesianLocation(int64_t)> tracking_function;

  extern const std::vector<std::string> LOW_EARTH_ORBIT_SATELLITES;
  extern const std::vector<std::string> GEOSYNCHRONOUS_SATELLITES;
  extern const std::vector<std::string> PLANETS;
  extern const std::vector<std::string> STARS;
  extern const std::vector<std::string> CITIES;
  extern const std::vector<std::string> OTHER;

  bool initSatellites(std::function<std::optional<std::string>(std::string)> urlFetchFunction);

  tracking_function getTrackingFunction(std::string name);
};

#endif
