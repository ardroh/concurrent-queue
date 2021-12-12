#pragma once
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>

namespace eventHorizon {
template <class T> class IConcurrentQueue {
public:
  virtual ~IConcurrentQueue() = default;
  virtual void EnqueueAsync(T &&element) = 0;
  virtual T Dequeue() = 0;
  virtual bool Wait(const std::chrono::nanoseconds &duration) = 0;
  virtual bool HasItems() = 0;
};

template <class T> class ConcurrentQueue final : public IConcurrentQueue<T> {
public:
  ~ConcurrentQueue() override {
    terminationRequest_ = true;
    for (auto &th : enqueingThreads_) {
      if (th.second.joinable()) {
        th.second.join();
      }
    }
  }

  void EnqueueAsync(T &&element) override {
    std::thread th(
        [&](T &&elem) {
          if (!queueMutex_.try_lock_for(std::chrono::milliseconds(100)) ||
              terminationRequest_) {
            detach(std::this_thread::get_id());
            return;
          }
          queue_.push(element);
          queueMutex_.unlock();
          detach(std::this_thread::get_id());
        },
        std::move(element));
    std::unique_lock<std::mutex> lk(enqueingThreadsMutex_);
    enqueingThreads_.emplace(std::piecewise_construct,
                             std::forward_as_tuple(th.get_id()),
                             std::forward_as_tuple(std::move(th)));
  }

  T Dequeue() override {
    std::unique_lock<std::timed_mutex> lk(queueMutex_);
    if (queue_.empty()) {
      return {};
    }
    auto elem = queue_.front();
    queue_.pop();
    return elem;
  }

  bool Wait(const std::chrono::nanoseconds &duration) override {
    if (HasItems()) {
      return true;
    }
    std::mutex waitMutex;
    std::unique_lock<std::mutex> lk(waitMutex);
    return itemsEnqueued.wait_for(lk, duration) == std::cv_status::no_timeout;
  }

  bool HasItems() override {
    std::unique_lock<std::timed_mutex> lk(queueMutex_);
    return queue_.empty();
  }

private:
  void detach(const std::thread::id &threadId) {
    if (terminationRequest_) {
      return;
    }

    std::unique_lock<std::mutex> lk(enqueingThreadsMutex_);
    auto threadPair = enqueingThreads_.find(threadId);
    if (threadPair == enqueingThreads_.end()) {
      return;
    }
    threadPair->second.detach();

    enqueingThreads_.erase(threadPair);
  }

private:
  std::queue<T> queue_;
  std::timed_mutex queueMutex_;
  std::unordered_map<std::thread::id, std::thread> enqueingThreads_;
  std::mutex enqueingThreadsMutex_;
  std::atomic_bool terminationRequest_ = false;
  std::condition_variable itemsEnqueued;
};

} // namespace eventHorizon
