//
// Created by Alexey Mihailov
//

#ifndef APP_SIGNATURE_SEMAPHORE_H
#define APP_SIGNATURE_SEMAPHORE_H

#include <mutex>
#include <condition_variable>

namespace appsign {
  /// Simple semaphore implementation based on a mutex and a condvar.
  /// Intended for the usage in the single worker thread exclusively.
  class Semaphore {
  private:
    std::mutex mutex;    ///< Mutex required for the implementation
    std::condition_variable cond_variable;  ///< Condvar required for the implementation

    unsigned long count = 0;  ///< Semaphore notification count

  public:
    /// Use this function in the worker thread to wait until the notification arrives
    void wait();

    /// Notify the semaphore (and the worker thread), increases the notifications \ref count by 1
    void notify();
  };
}


#endif //APP_SIGNATURE_SEMAPHORE_H
