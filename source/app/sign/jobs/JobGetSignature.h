//
// Created by Alexey Mihailov
//

#ifndef APP_SIGNATURE_JOBGETSIGNATURE_H
#define APP_SIGNATURE_JOBGETSIGNATURE_H

#include <app/utils/thread_pool/JobBase.h>
#include <atomic>
#include <app/sign/internal/FileSignatureCreationContext.h>
#include <map>
#include <mutex>
#include <memory>

namespace appsign {
  /// Job implementation for the reading a block from the file and hash generation
  class JobGetSignature : public JobBase {
  private:
    internal::FileSignatureCreationContext &context; ///< Ref to the shared proc context

    /// Determines if the new job should be generated and pushed to the thread pool after the current
    /// (if End-Of-File has not yet been reached)
    bool need_to_continue_after_write = true;

    /// Determines if the worker needs to wait before the continuation, because of output buffer map is full
    /// \see appsign::internal::FileSignatureCreationContext::output_buffer_threshold_max_size
    bool need_to_wait_output_buffer_is_full = false;

  public:
    /// Default (main) ctor, creation needs a context
    /// \param parContext The shared context for the proc
    explicit JobGetSignature(internal::FileSignatureCreationContext &parContext);

    /// Reads from the file and generates hash, writes to the buffer map
    void doJob() override;

    /// Determines if the continuation is needed and pushes the next job to the pool thread, if yes
    void onJobDone() override;
  };
}

#endif //APP_SIGNATURE_JOBGETSIGNATURE_H
