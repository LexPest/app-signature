//
// Created by Alexey Mihailov
//

#include "ThreadPool.h"

using namespace appsign;


void ThreadPool::init(unsigned int par_thread_count) {
  if (par_thread_count <= 0) {
    throw std::invalid_argument("Thread count must be higher than zero (0)");
  }
  thread_count = par_thread_count;
  init();
}

void ThreadPool::init() {
  if (state.load() != EThreadPoolState::Created) {
    throw std::logic_error("ThreadPool cannot be initialized twice");
  }

  state.store(EThreadPoolState::Initialization);

  {
    std::lock_guard<std::mutex> _lock(mutex_threads);

    // if it was already terminated, the value will change, so no need for the further init
    if (state.load() != EThreadPoolState::Initialization) {
      return;
    }

    threads.reserve(thread_count);

    for (unsigned int i = 0; i < thread_count; i++) {
      threads.emplace_back([&] { threadWorkFunction(); });
    }

    state.store(EThreadPoolState::Working);
  }

}


void ThreadPool::threadWorkFunction() {
  thread_local std::shared_ptr<JobBase> _next_job = nullptr;
  thread_local bool _is_joining = false;

  thread_local auto getNextJobFunc = [&]() {
    if (!queued_jobs.empty()) {
      _next_job = queued_jobs.front();
      queued_jobs.pop();
    } else {
      _next_job = nullptr;
    }
  };

  while (true) {
    if (!_is_joining) {

      std::unique_lock<std::mutex> _lock(mutex_queue);

      cond_queue_check.wait(_lock,
                            [&] {
                              EThreadPoolState _state = state.load();
                              return !queued_jobs.empty() ||
                                     _state == EThreadPoolState::InTermination ||
                                     _state == EThreadPoolState::JoiningAll;
                            });

      if (state.load() == EThreadPoolState::InTermination) {
        return;
      }

      getNextJobFunc();

      if (state.load() == EThreadPoolState::JoiningAll) {
        _is_joining = true;
      }

    } else {
      std::lock_guard<std::mutex> _lock(mutex_queue);

      if (!queued_jobs.empty()) {
        _next_job = queued_jobs.front();
        queued_jobs.pop();
      }

    }

    if (_next_job != nullptr) {
      _next_job->doJob();
      _next_job->onJobDone();
      _next_job = nullptr;
    }

    if (_is_joining) {
      std::lock_guard<std::mutex> _lock(mutex_queue);
      if (queued_jobs.empty()) {
        return;
      }
    }

  }
}

void ThreadPool::addJob(const std::shared_ptr<JobBase> &par_job) {
  {
    std::unique_lock<std::mutex> _lock(mutex_queue);
    queued_jobs.push(par_job);
  }
  cond_queue_check.notify_one();
}

void ThreadPool::addJobs(std::unique_ptr<std::vector<std::shared_ptr<JobBase>>> par_jobs) {
  for (auto par_job : *par_jobs) {
    addJob(par_job);
  }
}

ThreadPool::~ThreadPool() {
  terminate();
}

void ThreadPool::terminate() {
  {
    std::lock_guard<std::mutex> _lock(mutex_threads);

    EThreadPoolState _state_current = state.load();

    if (_state_current == EThreadPoolState::JoiningAll ||
        _state_current == EThreadPoolState::InTermination ||
        _state_current == EThreadPoolState::Terminated) {
      return;
    }

    state.store(EThreadPoolState::InTermination);
    cond_queue_check.notify_all();

    for (std::thread &_thread : threads) {
      if (_thread.joinable()) {
        _thread.join();
      }
    }

    state.store(EThreadPoolState::Terminated);
  }

}

void ThreadPool::joinAll() {
  {
    std::lock_guard<std::mutex> _lock(mutex_threads);

    EThreadPoolState _state_current = state.load();

    if (_state_current == EThreadPoolState::JoiningAll ||
        _state_current == EThreadPoolState::InTermination ||
        _state_current == EThreadPoolState::Terminated) {
      return;
    }

    state.store(EThreadPoolState::JoiningAll);
    cond_queue_check.notify_all();

    for (std::thread &_thread : threads) {
      if (_thread.joinable()) {
        _thread.join();
      }
    }

    state.store(EThreadPoolState::Terminated);
  }

}

unsigned int ThreadPool::getThreadCount() const {
  return thread_count;
}

