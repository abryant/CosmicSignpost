#include "tracker.h"

#include <cstddef>

Tracker::Tracker(
    Location currentLocation,
    Direction currentDirection,
    std::function<CartesianLocation(int64_t)> trackedObject)
    : currentLocation(currentLocation),
      currentDirection(currentDirection),
      trackedObject(trackedObject) {}

void Tracker::setCurrentLocation(Location currentLocation) {
  this->currentLocation = currentLocation;
}

void Tracker::setCurrentDirection(Direction direction) {
  this->currentDirection = direction;
}

void Tracker::setTrackedObject(std::function<CartesianLocation(int64_t)> trackedObject) {
  this->trackedObject = trackedObject;
}

Direction Tracker::getDirectionAt(int64_t timeMillis) {
  CartesianLocation from = currentLocation.getCartesian().toFixed(timeMillis);
  CartesianLocation to = trackedObject(timeMillis).toFixed(timeMillis);
  return from.directionTowards(to, currentLocation.getNormal());
}
