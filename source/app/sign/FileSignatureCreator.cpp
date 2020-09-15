//
// Created by Alexey Mihailov
//

#include "FileSignatureCreator.h"

using namespace appsign;

void FileSignatureCreator::write_signature_chunk_thread_func(internal::FileSignatureCreationContext &par_context) {
  while (true) {
    par_context.semaphore_output.wait();

    {
      std::lock_guard<std::mutex> _lock(par_context.mutex_sign_data_to_write_map);

      while (true) {
        bool _should_break = false;
        auto _current_output_sector_num = par_context.current_output_sector.load();
        auto _found_val = par_context.signature_data_to_write.find(_current_output_sector_num);
        if (_found_val != par_context.signature_data_to_write.end()) {
          std::cout << "[Output] - block num " << _current_output_sector_num << std::endl;
          par_context.output_fstream << _found_val->second;
          par_context.signature_data_to_write.erase(_found_val);
          par_context.current_output_sector.fetch_add(1);
        } else {
          _should_break = true;
        }

        if (par_context.signature_data_to_write.size() < par_context.output_buffer_threshold_max_size) {
          std::lock_guard<std::mutex> _lock_buf_full(par_context.mutex_output_buf_full);
          par_context.output_buffer_is_not_full.store(true);
          par_context.cond_output_buf_full.notify_all();
        }

        if (_should_break) {
          break;
        }
      }


    }

    if (par_context.is_output_thread_joining.load()) {
      // flushing ofstream to avoid bug in some MSVC compilers accord. to §27.8.1.10/4
      par_context.output_fstream.flush();
      return;
    }
  }
}

void FileSignatureCreator::join_output_worker_thread(internal::FileSignatureCreationContext &par_context,
                                                     std::thread &par_worker_thread) {
  par_context.is_output_thread_joining.store(true);
  par_context.semaphore_output.notify();
  if (par_worker_thread.joinable()) {
    par_worker_thread.join();
  }
}

void FileSignatureCreator::create_file_signature(ThreadPool &par_thread_pool) {

  internal::FileSignatureCreationContext _context{creation_params, par_thread_pool};

  unsigned int _thread_count = par_thread_pool.getThreadCount();

  //supply the start jobs

  for (unsigned int i = 0; i < _thread_count; i++) {
    _context.thread_pool.addJob(std::move(std::make_shared<JobGetSignature>(_context)));
  }

  //start the output worker thread
  std::thread _output_worker_thread = std::thread([&]() { write_signature_chunk_thread_func(_context); });

  // wait and join
  {
    std::unique_lock<std::mutex> _unique_lock(_context.mutex_should_join_thread_pool);
    _context.cond_should_join_thread_pool.wait(_unique_lock, [&]() { return _context.should_join_thread_pool.load(); });
    _context.thread_pool.joinAll();

  }
  join_output_worker_thread(_context, _output_worker_thread);

}
