//
// Created by Alexey Mihailov
//

#ifndef APP_SIGNATURE_THREADPOOL_H
#define APP_SIGNATURE_THREADPOOL_H

#include <memory>
#include <thread>
#include <exception>
#include <vector>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <iostream>

#include "JobBase.h"
#include "EThreadPoolState.h"

namespace appsign {
  /// Basic thread pool implementations for the jobs execution
  /// Usage: ctor -> init -> add_job(s) -> [wait] -> join or terminate
  class ThreadPool {
  private:
    std::atomic<EThreadPoolState> state{EThreadPoolState::Created};     ///< The current thread pool state

    /// The thread workers count.
    /// If not specified by calling \ref init(unsigned int par_thread_count) ,
    /// will use a value from std::thread::hardware_concurrency()
    unsigned int thread_count = std::thread::hardware_concurrency();

    std::vector<std::thread> threads; ///< Stores the created managed threads
    std::mutex mutex_threads; ///< Mutex for the operations with the \ref threads storage

    std::queue<std::shared_ptr<JobBase>> queued_jobs; ///< Jobs queue, all arrived jobs go here

    /// Condvar for the waiting until the job arrives into the queue
    /// \see queued_jobs
    std::condition_variable cond_queue_check;
    std::mutex mutex_queue; ///< Mutex for the \ref cond_queue_check

    /// The main thread work function, waits for the jobs and performs them
    void threadWorkFunction();

  public:
    // -- Design explanation:
    // I could put the initialization fully into ctors. Exception throw will be safe in that case
    // (because smart pointers are using the RAII-idiom). But I'll separate the initialization from construction
    // in case of using raw pointers with 'new' and non-RAII methods.

    /// The default ctor
    ThreadPool() = default;

    /// The default deconstructor, terminates the thread pool, if needed
    ~ThreadPool();

    /// Initializes the thread pool, may throw an exception if you try to initialize it again.
    /// \ref thread_count will be initialized by default using a value from std::thread::hardware_concurrency()
    /// If you want to specify your thread count value, use the \ref init(unsigned int par_thread_count) overload
    void init();

    /// Initializes the thread pool, may throw an exception if you try to initialize it again or the thread count is invalid.
    /// \param par_thread_count Desired workers thread count in the pool
    void init(unsigned int par_thread_count);

    /// Adds job to the \ref queued_jobs and notifies the worker thread about the job arrival
    /// \param par_job Target job to execute
    void addJob(const std::shared_ptr<JobBase> &par_job);

    /// Adds multiple jobs one by one.
    /// \see addJob(const std::shared_ptr<JobBase> &par_job)
    /// \param par_jobs Unique ptr to the jobs containing vector
    void addJobs(std::unique_ptr<std::vector<std::shared_ptr<JobBase>>> par_jobs);

    /// Sends signal to the managed worker threads and waits for the join to the current thread
    void joinAll();

    /// Terminates the thread pool execution, sends signals to terminate to all managed threads
    void terminate();

    /// Returns the total worker thread count
    unsigned int getThreadCount() const;
  };

}

#endif //APP_SIGNATURE_THREADPOOL_H
