#ifndef SPACEPOINTER_LIB_SPACEPOINTER_TRACKER_H_
#define SPACEPOINTER_LIB_SPACEPOINTER_TRACKER_H_

#include <cstdint>
#include <functional>

#include "cartesian_location.h"
#include "direction.h"
#include "location.h"

// Tracks an object's position from the current position.
// This accounts for movement, by finding angular velocities.
class Tracker {
  private:
    // Current location of the pointer.
    Location currentLocation;
    // Current direction of the pointer, assumed to be non-moving.
    Direction currentDirection;

    // Tracked location.
    std::function<CartesianLocation(int64_t)> trackedObject;

  public:
    Tracker(
      Location currentLocation,
      Direction currentDirection,
      std::function<CartesianLocation(int64_t)> trackedObject);
    void setCurrentLocation(Location currentLocation);
    void setCurrentDirection(Direction direction);
    void setTrackedObject(std::function<CartesianLocation(int64_t)> trackedObject);

    Direction getDirectionAt(int64_t timeMillis);
};

#endif
