#include "concurrent_queue.h"
#include <chrono>
#include <iostream>
#include <string>

int main(int argc, char **argv) {
  eventHorizon::ConcurrentQueue<int> cq;
  //   std::string input("abc");
  int input = 123;
  std::cout << "enqueue" << std::endl;
  cq.EnqueueAsync(std::move(input));
  std::cout << "enqueue off" << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  std::cout << "sleep off" << std::endl;
  auto elem = cq.Dequeue();
  std::cout << elem << std::endl;
  return 0;
}