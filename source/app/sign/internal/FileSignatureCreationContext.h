//
// Created by Alexey Mihailov
//

#ifndef APP_SIGNATURE_FILESIGNATURECREATIONCONTEXT_H
#define APP_SIGNATURE_FILESIGNATURECREATIONCONTEXT_H

#include <atomic>
#include <map>
#include <mutex>
#include <vector>
#include <fstream>
#include <memory>
#include <app/sign/SignatureCreationParams.h>
#include <app/utils/thread_pool/ThreadPool.h>
#include <condition_variable>
#include <app/utils/semaphore/Semaphore.h>

namespace appsign {
  namespace internal {
    /// Buffer max capacity will be calculated by multiplying thread count by this constant
    /// \see appsign::ThreadPool::getThreadCount
    const unsigned long BUFFER_THRESHOLD_CALC_MULTIPLIER = 4;

    /// Context holds all shared data that is used during the signature creation process
    struct FileSignatureCreationContext {

      const SignatureCreationParams &creation_params; ///< Creation params ref
      const unsigned long long output_buffer_threshold_max_size; ///< Output buffers map max size/capacity

      ThreadPool &thread_pool; ///< Working threads pool ref

      std::atomic<unsigned long long> current_input_sector{0};     ///< Input block/sector proc counter
      std::atomic<unsigned long long> current_output_sector{0};    ///< Output block/sector proc counter

      /// Output buffer map, processed hashes are stored here
      std::map<unsigned long long, std::string> signature_data_to_write;

      std::mutex mutex_input;   ///< Input file stream mutex

      /// Semaphore for the output worker function (that runs in the separate thread)
      /// \see appsign::FileSignatureCreator::write_signature_chunk_thread_func
      Semaphore semaphore_output;

      /// Mutex for the output buffer map access
      /// \see signature_data_to_write
      std::mutex mutex_sign_data_to_write_map;

      /// Flag that determines if the output buffer size is higher than threshold
      /// \see signature_data_to_write
      /// \see output_buffer_threshold_max_size
      std::atomic<bool> output_buffer_is_not_full{true};

      /// Mutex for the \ref cond_output_buf_full
      std::mutex mutex_output_buf_full;

      /// CondVar for the waiting until the output buffer map size will be less than the threshold
      std::condition_variable cond_output_buf_full;

      /// Mutex for the \ref cond_should_join_thread_pool
      std::mutex mutex_should_join_thread_pool;

      /// Atomic flag that determines that all jobs are done and it's time to join the thread pool
      std::atomic<bool> should_join_thread_pool{false};

      /// CondVar for the waiting until all jobs are done and it's the time to join the thread pool
      std::condition_variable cond_should_join_thread_pool;

      /// Atomic flag for the joining the output worker thread
      /// \see appsign::FileSignatureCreator::write_signature_chunk_thread_func
      std::atomic<bool> is_output_thread_joining{false};

      std::ifstream input_fstream;     ///< Input file stream (RAII), will be created during the construction
      std::ofstream output_fstream;    ///< Output file stream (RAII), will be created during the construction


      /// Default (main) constructor. Creates file streams and calculates \ref output_buffer_threshold_max_size
      /// \param par_creation_params Signature creation parameters by ref, will be stored inside
      /// \param par_thread_pool Thread Pool by ref, will be stored inside
      FileSignatureCreationContext(const SignatureCreationParams &par_creation_params,
                                   ThreadPool &par_thread_pool);
    };
  }
}

#endif //APP_SIGNATURE_FILESIGNATURECREATIONCONTEXT_H
