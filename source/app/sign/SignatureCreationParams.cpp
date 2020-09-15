//
// Created by Alexey Mihailov
//

#include "SignatureCreationParams.h"

using namespace appsign;

void SignatureCreationParams::validate() {
  if (block_size <= 0) {
    throw std::invalid_argument("Block size must be higher than zero (0)");
  }

  if (!(std::ifstream(input_file_path).good())) {
    throw std::invalid_argument("Input file path is invalid, file is not found");
  }

  if (!(std::ofstream(output_file_path).good())) {
    throw std::invalid_argument("Output file path is invalid, cannot open output file stream");
  }
}
