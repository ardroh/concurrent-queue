#include "concurrent_queue_impl.h"
#include <chrono>
#include <iostream>
#include <string>

int main(int argc, char **argv) {
  eventHorizon::ConcurrentQueue<std::unique_ptr<int>> cq;
  //   std::string input("abc");
  auto input = std::make_unique<int>(123);
  cq.enqueue(std::move(input));
  std::unique_ptr<int> element;
  if (!cq.dequeue_wait(element, std::chrono::milliseconds(100))) {
    return 1;
  }

  std::cout << *element << std::endl;
  return 0;
}