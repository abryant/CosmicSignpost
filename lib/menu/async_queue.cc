#include "async_queue.h"

std::queue<std::function<void()>> AsyncQueue::QUEUE;

void AsyncQueue::addToQueue(std::function<void()> function) {
  AsyncQueue::QUEUE.push(function);
}

void AsyncQueue::runQueue() {
  while (!AsyncQueue::QUEUE.empty()) {
    std::function<void()> f = AsyncQueue::QUEUE.front();
    AsyncQueue::QUEUE.pop();
    f();
  }
}
