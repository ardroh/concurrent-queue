#pragma once
#include <optional>

namespace eventHorizon {
template <class T> class IConcurrentQueue {
public:
  virtual ~IConcurrentQueue() = default;
  virtual void EnqueueAsync(T &&element) = 0;
  virtual std::optional<T> Dequeue() = 0;
  virtual bool Wait(const std::chrono::nanoseconds &duration) = 0;
  virtual bool HasItems() = 0;
};

} // namespace eventHorizon
