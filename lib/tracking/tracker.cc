#include "tracker.h"

#include <cstddef>

const int64_t SPIN_MILLIS_PER_ROTATION = 10000;

Tracker::Tracker(
    Location currentLocation,
    Direction currentDirection,
    TrackableObjects::tracking_function trackingFunction)
    : currentLocation(currentLocation),
      currentDirection(currentDirection),
      trackingFunction(trackingFunction),
      spinning(false) {}

void Tracker::setCurrentLocation(Location currentLocation) {
  this->currentLocation = currentLocation;
}

Location Tracker::getCurrentLocation() {
  return currentLocation;
}

void Tracker::setCurrentDirection(Direction direction) {
  this->currentDirection = direction;
}

void Tracker::setSpinning(bool spinning) {
  this->spinning = spinning;
}

void Tracker::setTrackingFunction(TrackableObjects::tracking_function trackingFunction) {
  this->trackingFunction = trackingFunction;
}

void Tracker::setDirectionFunction(std::optional<direction_function> directionFunction) {
  this->directionFunction = directionFunction;
}

Direction Tracker::getSpinningDirectionAt(int64_t timeMillis) {
  double azimuth = (timeMillis % SPIN_MILLIS_PER_ROTATION) * 360.0 / SPIN_MILLIS_PER_ROTATION;
  // TODO: make it possible to specify the altitude for spinning.
  double altitude = 0.0;
  return Direction(azimuth, altitude);
}

Direction Tracker::getDirectionAt(int64_t timeMillis) {
  if (spinning) {
    return getSpinningDirectionAt(timeMillis);
  }
  if (directionFunction.has_value()) {
    return directionFunction.value()(timeMillis);
  }
  CartesianLocation from = currentLocation.getCartesian().toFixed(timeMillis);
  CartesianLocation to = trackingFunction(timeMillis).toFixed(timeMillis);
  return from.directionTowards(to, currentLocation.getNormal());
}

double Tracker::getDistanceAt(int64_t timeMillis) {
  CartesianLocation from = currentLocation.getCartesian().toFixed(timeMillis);
  CartesianLocation to = trackingFunction(timeMillis).toFixed(timeMillis);
  return (to.position - from.position).getLength();
}
