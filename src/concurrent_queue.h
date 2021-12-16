#pragma once
#include <optional>

namespace eventHorizon {
template <class T> class IConcurrentQueue {
public:
  virtual ~IConcurrentQueue() = default;
  virtual void enqueue(T &&element) = 0;
  virtual bool dequeue_wait(T &element,
                            const std::chrono::milliseconds &duration) = 0;
};

} // namespace eventHorizon
