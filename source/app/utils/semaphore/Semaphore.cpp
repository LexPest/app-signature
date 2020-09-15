//
// Created by Alexey Mihailov
//

#include "Semaphore.h"

using namespace appsign;

void Semaphore::wait() {
  std::unique_lock<std::mutex> _lock(mutex);
  while (count <= 0) {
    cond_variable.wait(_lock, [&]() { return count > 0; });
  }
  count--;
}

void Semaphore::notify() {
  std::lock_guard<std::mutex> _lock(mutex);
  count++;
  cond_variable.notify_one();
}
