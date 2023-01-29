#ifndef COSMIC_SIGNPOST_LIB_TRACKING_DIRECTION_QUEUE_H_
#define COSMIC_SIGNPOST_LIB_TRACKING_DIRECTION_QUEUE_H_

#include <condition_variable>
#include <optional>
#include <stdint.h>
#include <map>
#include <mutex>

#include "direction.h"

class DirectionQueue {
  private:
    static const int32_t DIRECTION_QUEUE_CAPACITY = 10;
    std::map<int64_t, Direction> directionsByTimeMillis;
    std::condition_variable condition;
    std::mutex mutex;

  public:
    DirectionQueue();

    // Returns true iff the queue is full.
    bool isFull();

    // Removes all elements from the queue.
    void clear();

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

    // Finds the first time in the queue that is at least timeMillis, returns the whole entry
    // (time and Direction), and removes any times before it from the queue.
    // The returned element remains in the queue until an element after it is removed.
    // Returns nullopt if the queue is does not contain such an element.
    std::optional<std::pair<int64_t, Direction>> getDirectionAtOrAfterNonBlocking(int64_t timeMillis);

    // Finds the first time in the queue that is at least timeMillis, and returns the whole entry
    // (time and Direction) without modifying the queue.
    // Returns nullopt if the queue is does not contain such an element.
    std::optional<std::pair<int64_t, Direction>> peekDirectionAtOrAfterNonBlocking(int64_t timeMillis);
};

#endif
