#ifndef SPACEPOINTER_LIB_SPACEPOINTER_TRACKABLE_OBJECTS_H_
#define SPACEPOINTER_LIB_SPACEPOINTER_TRACKABLE_OBJECTS_H_

#include <functional>
#include <map>
#include <string>

#include "equatorial_location.h"
#include "location.h"
#include "moon_orbit.h"
#include "planetary_orbit.h"

const std::map<std::string, std::function<CartesianLocation(int64_t)>> TRACKABLE_OBJECTS;

#endif
