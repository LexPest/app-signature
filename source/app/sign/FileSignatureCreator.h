#include <utility>

//
// Created by Alexey Mihailov
//

#ifndef APP_SIGNATURE_FILESIGNATURECREATOR_H
#define APP_SIGNATURE_FILESIGNATURECREATOR_H

#include <app/sign/SignatureCreationParams.h>
#include <app/sign/jobs/JobGetSignature.h>
#include <app/utils/thread_pool/ThreadPool.h>
#include <app/sign/internal/FileSignatureCreationContext.h>
#include <mutex>
#include <thread>

namespace appsign {
  /// Provides the functionality for the hash generation from the file (file signature).
  /// Hash is generated block-by-block, blocks are separated by the specified size (in bytes).
  /// Outputs the results in the specified output file.
  class FileSignatureCreator {
  private:
    const SignatureCreationParams creation_params; ///< Encapsulated parameters for the signature creation

    /// Internal function for the separate worker thread,
    /// that waits for the ready hash fragments in the \ref signature_data_to_write and outputs it to the output file
    /// \param par_context The shared context, where the \ref signature_data_to_write is stored
    void write_signature_chunk_thread_func(internal::FileSignatureCreationContext &par_context);

    /// Helper function that joins the separate output worker thread to the current thread
    /// \param par_context The shared context, where the required mutex and condvar are stored
    /// \param par_worker_thread The target output worker thread ref
    void join_output_worker_thread(internal::FileSignatureCreationContext &par_context,
                                   std::thread &par_worker_thread);

  public:
    /// Default (main) ctor, that initializes the worker object. Needs correctly provided params.
    /// It is recommeneded to validate params by using validate() function.
    /// \param parCreationParams Encapsulated signature creation params for the worker object
    explicit FileSignatureCreator(SignatureCreationParams parCreationParams) : creation_params(
        std::move(parCreationParams)) {}

    /// Launches the signature creation
    /// \param par_thread_pool Target thread pool, where the jobs will be supplied to
    void create_file_signature(ThreadPool &par_thread_pool);
  };
}

#endif //APP_SIGNATURE_FILESIGNATURECREATOR_H
