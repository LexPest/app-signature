#include <utility>

//
// Created by Alexey Mihailov
//

#ifndef APP_SIGNATURE_SIGNATURECREATIONPARAMS_H
#define APP_SIGNATURE_SIGNATURECREATIONPARAMS_H

#include <string>
#include <fstream>
#include <exception>

namespace appsign {

  /// Contains the encapsulated params for the \ref FileSignatureCreator
  /// Usage: ctor -> validate() [Note: may throw exceptions] -> use members (all public)
  struct SignatureCreationParams {
    const std::string input_file_path;      ///< The input file path, might be relative or absolute
    const std::string output_file_path;     ///< The output file path, might be relative or absolute
    const unsigned long long block_size;    ///< The block size for the hash generation

    /// Default (main) ctor, encapsulates all the params into the single object.
    /// It is highly recommended to call the validate() function before the usage.
    /// \param par_input Turns into \ref input_file_path
    /// \param par_output Turns into \ref output_file_path
    /// \param par_block_size Turns into \ref block_size
    SignatureCreationParams(std::string par_input, std::string par_output, unsigned long long par_block_size) :
        input_file_path(std::move(par_input)), output_file_path(std::move(par_output)), block_size(par_block_size) {}

    /// Validates the provided params, will throw an exception if something isn't right
    void validate();
  };

}

#endif //APP_SIGNATURE_SIGNATURECREATIONPARAMS_H
