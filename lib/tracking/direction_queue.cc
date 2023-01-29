#include "direction_queue.h"

#include <stdint.h>

#include "direction.h"

DirectionQueue::DirectionQueue() {}

bool DirectionQueue::isFull() {
  bool result;
  {
    std::unique_lock<std::mutex> lock(mutex);
    result = directionsByTimeMillis.size() >= DirectionQueue::DIRECTION_QUEUE_CAPACITY;
  }
  return result;
}

void DirectionQueue::clear() {
  std::unique_lock<std::mutex> lock(mutex);
  directionsByTimeMillis.clear();
}

void DirectionQueue::addDirection(int64_t timeMillis, Direction direction) {
  {
    std::unique_lock<std::mutex> lock(mutex);
    while (directionsByTimeMillis.size() >= DirectionQueue::DIRECTION_QUEUE_CAPACITY) {
      condition.wait(lock);
    }
    directionsByTimeMillis[timeMillis] = direction;
  }
  condition.notify_one();
}

std::pair<int64_t, Direction> DirectionQueue::getDirectionAtOrAfter(int64_t timeMillis) {
  std::pair<int64_t, Direction> result;
  {
    std::unique_lock<std::mutex> lock(mutex);
    std::map<int64_t, Direction>::iterator it = directionsByTimeMillis.lower_bound(timeMillis);
    while (it == directionsByTimeMillis.end()) {
      if (directionsByTimeMillis.size() >= DirectionQueue::DIRECTION_QUEUE_CAPACITY) {
        // The queue is full, but doesn't contain the element we need, so clear it and keep waiting.
        directionsByTimeMillis.clear();
      }
      condition.wait(lock);
      it = directionsByTimeMillis.lower_bound(timeMillis);
    }
    result = std::make_pair(it->first, it->second);
    directionsByTimeMillis.erase(directionsByTimeMillis.begin(), it);
  }
  condition.notify_one();
  return result;
}

std::pair<int64_t, Direction> DirectionQueue::peekDirectionAtOrAfter(int64_t timeMillis) {
  std::pair<int64_t, Direction> result;
  {
    std::unique_lock<std::mutex> lock(mutex);
    std::map<int64_t, Direction>::iterator it = directionsByTimeMillis.lower_bound(timeMillis);
    while (it == directionsByTimeMillis.end()) {
      condition.wait(lock);
      it = directionsByTimeMillis.lower_bound(timeMillis);
    }
    result = std::make_pair(it->first, it->second);
  }
  // Wake up something else, just to avoid getting stuck.
  // If there's one reader and one writer, this shouldn't be needed, but it also shouldn't cause
  // any problems.
  condition.notify_one();
  return result;
}

std::optional<std::pair<int64_t, Direction>> DirectionQueue::getDirectionAtOrAfterNonBlocking(
    int64_t timeMillis) {
  std::optional<std::pair<int64_t, Direction>> result;
  {
    std::unique_lock<std::mutex> lock(mutex);
    std::map<int64_t, Direction>::iterator it = directionsByTimeMillis.lower_bound(timeMillis);
    if (it == directionsByTimeMillis.end()) {
      result = std::nullopt;
    } else {
      result = std::make_pair(it->first, it->second);
    }
    directionsByTimeMillis.erase(directionsByTimeMillis.begin(), it);
  }
  condition.notify_one();
  return result;
}

std::optional<std::pair<int64_t, Direction>> DirectionQueue::peekDirectionAtOrAfterNonBlocking(
    int64_t timeMillis) {
  std::optional<std::pair<int64_t, Direction>> result;
  {
    std::unique_lock<std::mutex> lock(mutex);
    std::map<int64_t, Direction>::iterator it = directionsByTimeMillis.lower_bound(timeMillis);
    if (it == directionsByTimeMillis.end()) {
      result = std::nullopt;
    } else {
      result = std::make_pair(it->first, it->second);
    }
  }
  // Wake up something else, just to avoid getting stuck.
  // If there's one reader and one writer, this shouldn't be needed, but it also shouldn't cause
  // any problems.
  condition.notify_one();
  return result;
}
