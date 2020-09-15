//
// Created by Alexey Mihailov
//

#ifndef APP_SIGNATURE_ETHREADPOOLSTATE_H
#define APP_SIGNATURE_ETHREADPOOLSTATE_H

#include <stdint.h>

namespace appsign {
  /// Describes the current thread pool state
  enum class EThreadPoolState : uint8_t {

    /// Thread pool is created
        Created,

    /// Thread pool is initializing now, creating threads
        Initialization,

    /// Thread pool now is active
        Working,

    /// Thread pool is now joining all the managed threads
        JoiningAll,

    /// Thread pool termination process has been started
        InTermination,

    /// Thread pool is terminated and is ready for the disposal
        Terminated
  };
}

#endif //APP_SIGNATURE_ETHREADPOOLSTATE_H
