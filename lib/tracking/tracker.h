#ifndef COSMIC_SIGNPOST_LIB_TRACKING_TRACKER_H_
#define COSMIC_SIGNPOST_LIB_TRACKING_TRACKER_H_

#include <cstdint>
#include <functional>
#include <optional>

#include "cartesian_location.h"
#include "direction.h"
#include "location.h"
#include "trackable_objects.h"

typedef std::function<Direction(int64_t)> direction_function;

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
    // Pointing direction, used for calibration.
    std::optional<direction_function> directionFunction;
    // Whether the tracker is in spinning mode.
    bool spinning;

  public:
    Tracker(
      Location currentLocation,
      Direction currentDirection,
      TrackableObjects::tracking_function trackingFunction);
    void setCurrentLocation(Location currentLocation);
    void setCurrentDirection(Direction direction);
    void setSpinning(bool spinning);
    void setTrackingFunction(TrackableObjects::tracking_function trackingFunction);
    void setDirectionFunction(std::optional<direction_function> directionFunction);
    Location getCurrentLocation();

    Direction getSpinningDirectionAt(int64_t timeMillis);
    Direction getDirectionAt(int64_t timeMillis);
    double getDistanceAt(int64_t timeMillis);
};

#endif
