#include "tracker.h"

#include <sys/time.h>

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

int64_t Tracker::getCurrentTimeMillis() {
  struct timeval time_now;
  while (true) {
    int result = gettimeofday(&time_now, NULL);
    if (result == 0) {
      break;
    }
  }
  int64_t resultMillis = (((int64_t) time_now.tv_sec) * 1000) + (((int64_t) time_now.tv_usec) / 1000);
  return resultMillis;
}

Direction Tracker::getDirection() {
  CartesianLocation trackedObjectLocation = trackedObject(getCurrentTimeMillis());
  return currentLocation.getCartesian().directionTowards(
      trackedObjectLocation, currentLocation.getNormal());
}
