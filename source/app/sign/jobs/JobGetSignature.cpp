//
// Created by Alexey Mihailov
//

#include <md5/md5.h>
#include "JobGetSignature.h"


using namespace appsign;

void JobGetSignature::doJob() {
  unsigned long long _current_input_sector_val = 0;
  std::shared_ptr<std::vector<char>> _buffer = nullptr;
  {
    std::lock_guard<std::mutex> _lock(context.mutex_input);
    if (!context.input_fstream.eof()) {
      _current_input_sector_val = context.current_input_sector.fetch_add(1);
      std::cout << "[" << std::this_thread::get_id() << "] Reading block num " << _current_input_sector_val
                << std::endl;
      _buffer = std::make_shared<std::vector<char>>(context.creation_params.block_size, 0);
      context.input_fstream.read(_buffer->data(), _buffer->size());
    } else {
      //job's done, do not supply the new one
      need_to_continue_after_write = false;
    }
  }

  if (_buffer != nullptr) {
    std::cout << "[" << std::this_thread::get_id() << "] Processing block num " << _current_input_sector_val
              << std::endl;
    std::string _result_hash_string = md5(_buffer);

    {
      std::lock_guard<std::mutex> _lock(context.mutex_sign_data_to_write_map);
      context.signature_data_to_write.insert(std::pair<unsigned long long, std::string>
                                                 (_current_input_sector_val, std::move(_result_hash_string)));

      if (context.signature_data_to_write.size() > context.output_buffer_threshold_max_size) {
        context.output_buffer_is_not_full.store(false);
        need_to_wait_output_buffer_is_full = true;
      }
    }
    context.semaphore_output.notify();
  }
}

void JobGetSignature::onJobDone() {
  std::cout << "[" << std::this_thread::get_id() << "] Processing complete! " << std::endl;

  if (need_to_continue_after_write) {
    if (need_to_wait_output_buffer_is_full) {
      //wait for the output if the buffers' map is full
      {
        std::unique_lock<std::mutex> _unique_lock(context.mutex_output_buf_full);
        context.cond_output_buf_full.wait(_unique_lock, [&]() {
          return context.output_buffer_is_not_full.load();
        });
      }
    }
    //create and push the next job
    std::shared_ptr<JobBase> _next_job = std::make_shared<JobGetSignature>(context);
    context.thread_pool.addJob(std::move(_next_job));
  } else {
    context.should_join_thread_pool.store(true);
    context.cond_should_join_thread_pool.notify_all();
  }
}

JobGetSignature::JobGetSignature(internal::FileSignatureCreationContext &parContext) : context(parContext) {}
