#include "concurrent_queue.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>

namespace eventHorizon {

template <class T> class ConcurrentQueue final : public IConcurrentQueue<T> {
public:
  ~ConcurrentQueue() override = default;

  virtual void enqueue(T &&element) override {
    std::unique_lock<std::mutex> lk(queueMutex_);
    queue_.emplace(std::move(element));
  }

  virtual bool
  dequeue_wait(T &element, const std::chrono::milliseconds &duration) override {
    std::unique_lock<std::mutex> lk(queueMutex_);
    if (queue_.empty() && elementEnqueued.wait_for(lk, duration) ==
                              std::cv_status::timeout) {
      return false;
    }
    element = std::move(queue_.front());
    queue_.pop();
    return true;
  }

private:
  std::queue<T> queue_;
  std::mutex queueMutex_;
  std::condition_variable elementEnqueued;
};

} // namespace eventHorizon