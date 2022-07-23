#include "direction_queue.h"

#include <stdint.h>

#include "direction.h"

DirectionQueue::DirectionQueue() {}

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

Direction DirectionQueue::getDirectionAt(int64_t timeMillis) {
  Direction result;
  {
    std::unique_lock<std::mutex> lock(mutex);
    std::map<int64_t, Direction>::iterator it = directionsByTimeMillis.find(timeMillis);
    // TODO: what if we never find timeMillis and the queue fills up?
    while (it == directionsByTimeMillis.end()) {
      condition.wait(lock);
      it = directionsByTimeMillis.find(timeMillis);
    }
    result = it->second;
    directionsByTimeMillis.erase(directionsByTimeMillis.begin(), it);
  }
  condition.notify_one();
  return result;
}
