//
// Created by Alexey Mihailov
//

#ifndef APP_SIGNATURE_JOBBASE_H
#define APP_SIGNATURE_JOBBASE_H


namespace appsign {

  /// Abstract class for the simple job definition, that is intended for the execution in a \ref ThreadPool
  class JobBase {
  public:
    /// Main job actions should be defined in this method
    virtual void doJob() = 0;

    /// Job post-actions should be defined in this method
    virtual void onJobDone() = 0;
  };
}

#endif //APP_SIGNATURE_JOBBASE_H
