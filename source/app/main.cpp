//
// Created by Alexey Mihailov
//

#include <app/utils/thread_pool/ThreadPool.h>
#include <app/sign/jobs/JobGetSignature.h>
#include <memory>
#include <app/sign/SignatureCreationParams.h>

#include <iostream>
#include <exception>
#include <app/sign/FileSignatureCreator.h>


appsign::SignatureCreationParams getCreationParamsFromArgs(int argc, char **argv){
  if (argc >= 3){
    std::string _input_file_path;
    std::string _output_file_path;
    unsigned long long _block_size = 1048576;

    _input_file_path = argv[1];
    _output_file_path = argv[2];

    if (argc >= 4) {
      _block_size = strtoull(argv[3], 0, 10);;
    }

    return appsign::SignatureCreationParams(_input_file_path, _output_file_path, _block_size);
  } else {
    throw std::invalid_argument("Error: invalid arguments.\nUsage: app_signature <input> <output> [<block_size>]");
  }
}


// You can specify an argument to init(), that will set the number of threads
// by default: determined by hardware capabilities

int main(int argc, char **argv) {
  std::cout << "Program started..." << std::endl;
  appsign::SignatureCreationParams _creation_params = getCreationParamsFromArgs(argc, argv);

  std::cout << "Launch params:" << std::endl;
  std::cout << "Input file: "<< _creation_params.input_file_path << std::endl;
  std::cout << "Output file: "<< _creation_params.output_file_path << std::endl;
  std::cout << "Block size: "<< _creation_params.block_size << std::endl;

  _creation_params.validate();

  std::cout << "Validation complete" << std::endl;

  appsign::ThreadPool _thread_pool;
  _thread_pool.init();

  std::cout << "Worker threads count: " << _thread_pool.getThreadCount() << std::endl;

  appsign::FileSignatureCreator _file_signature_creator (_creation_params);
  _file_signature_creator.create_file_signature(_thread_pool);

  std::cout << "Program complete" << std::endl;
}