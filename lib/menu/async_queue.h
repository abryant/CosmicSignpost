#ifndef COSMIC_SIGNPOST_LIB_MENU_ASYNC_QUEUE_H_
#define COSMIC_SIGNPOST_LIB_MENU_ASYNC_QUEUE_H_

#include <functional>
#include <queue>

// Simple function queue, not thread-safe.
// This is intended for single-threaded processing of the entire UI.
namespace AsyncQueue {
  extern std::queue<std::function<void()>> QUEUE;

  void addToQueue(std::function<void()> function);
  void runQueue();
}

#endif
