#ifndef SPACEPOINTER_LIB_SPACEPOINTER_TRACKER_H_
#define SPACEPOINTER_LIB_SPACEPOINTER_TRACKER_H_

#include <cstdint>
#include <functional>

#include "cartesian_location.h"
#include "direction.h"
#include "location.h"
#include "trackable_objects.h"

// Tracks an object's position from the current position.
// This accounts for movement, by finding angular velocities.
class Tracker {
  private:
    // Current location of the pointer.
    Location currentLocation;
    // Current direction of the pointer, assumed to be non-moving.
    Direction currentDirection;

    // Tracked location.
    TrackableObjects::tracking_function trackingFunction;

  public:
    Tracker(
      Location currentLocation,
      Direction currentDirection,
      TrackableObjects::tracking_function trackingFunction);
    void setCurrentLocation(Location currentLocation);
    void setCurrentDirection(Direction direction);
    void setTrackingFunction(TrackableObjects::tracking_function trackingFunction);
    Location getCurrentLocation();

    Direction getDirectionAt(int64_t timeMillis);
    double getDistanceAt(int64_t timeMillis);
};

#endif
