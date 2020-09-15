//
// Created by Alexey Mihailov
//

#include "FileSignatureCreationContext.h"

using namespace appsign::internal;

FileSignatureCreationContext::FileSignatureCreationContext(const SignatureCreationParams &par_creation_params,
                                                           ThreadPool &par_thread_pool) :
    creation_params(par_creation_params), thread_pool(par_thread_pool),
    output_buffer_threshold_max_size(par_thread_pool.getThreadCount() * BUFFER_THRESHOLD_CALC_MULTIPLIER) {
  input_fstream = std::ifstream(par_creation_params.input_file_path, std::ios::in | std::ios::binary);
  output_fstream = std::ofstream(par_creation_params.output_file_path, std::ios::out | std::ios::trunc);
}
