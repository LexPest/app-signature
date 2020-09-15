cmake_minimum_required(VERSION 3.12)
project(app_signature)

set(CMAKE_CXX_STANDARD 11)

set(VERSION_MAJOR 2)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

add_executable(app_signature source/app/utils/thread_pool/ThreadPool.cpp source/app/utils/thread_pool/ThreadPool.h source/app/utils/thread_pool/JobBase.cpp source/app/utils/thread_pool/JobBase.h source/app/sign/jobs/JobGetSignature.cpp source/app/sign/jobs/JobGetSignature.h source/app/main.cpp source/app/sign/SignatureCreationParams.cpp source/app/sign/SignatureCreationParams.h source/app/sign/FileSignatureCreator.cpp source/app/sign/FileSignatureCreator.h source/app/utils/thread_pool/EThreadPoolState.h
                source/md5/md5.cpp source/md5/md5.h source/app/sign/internal/FileSignatureCreationContext.cpp source/app/sign/internal/FileSignatureCreationContext.h source/app/utils/semaphore/Semaphore.cpp source/app/utils/semaphore/Semaphore.h)
target_include_directories(app_signature PRIVATE source)