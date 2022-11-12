#include "tracker.h"

#include <cstddef>

Tracker::Tracker(
    Location currentLocation,
    Direction currentDirection,
    TrackableObjects::tracking_function trackingFunction)
    : currentLocation(currentLocation),
      currentDirection(currentDirection),
      trackingFunction(trackingFunction) {}

void Tracker::setCurrentLocation(Location currentLocation) {
  this->currentLocation = currentLocation;
}

void Tracker::setCurrentDirection(Direction direction) {
  this->currentDirection = direction;
}

void Tracker::setTrackingFunction(TrackableObjects::tracking_function trackingFunction) {
  this->trackingFunction = trackingFunction;
}

Direction Tracker::getDirectionAt(int64_t timeMillis) {
  CartesianLocation from = currentLocation.getCartesian().toFixed(timeMillis);
  CartesianLocation to = trackingFunction(timeMillis).toFixed(timeMillis);
  return from.directionTowards(to, currentLocation.getNormal());
}
