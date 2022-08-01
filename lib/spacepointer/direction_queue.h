#ifndef SPACEPOINTER_LIB_SPACEPOINTER_DIRECTION_QUEUE_H_
#define SPACEPOINTER_LIB_SPACEPOINTER_DIRECTION_QUEUE_H_

#include <condition_variable>
#include <stdint.h>
#include <map>
#include <mutex>

#include "direction.h"

class DirectionQueue {
  private:
    static const int32_t DIRECTION_QUEUE_CAPACITY = 100;
    std::map<int64_t, Direction> directionsByTimeMillis;
    std::condition_variable condition;
    std::mutex mutex;

  public:
    DirectionQueue();

    // Returns true iff the queue is full.
    bool isFull();

    // Adds the given direction at the given time.
    // Blocks if the queue is full.
    void addDirection(int64_t timeMillis, Direction direction);

    // Finds the first time in the queue that is at least timeMillis, returns the whole entry
    // (time and Direction), and removes any times before it from the queue.
    // The returned element remains in the queue until an element after it is removed.
    // Blocks if the queue is empty.
    std::pair<int64_t, Direction> getDirectionAtOrAfter(int64_t timeMillis);

    // Finds the first time in the queue that is at least timeMillis, and returns the whole entry
    // (time and Direction) without modifying the queue.
    // Blocks if the queue is empty.
    std::pair<int64_t, Direction> peekDirectionAtOrAfter(int64_t timeMillis);
};

#endif
