#pragma once
#include <nan.h>
#include <sys/sysctl.h>
#include <pwd.h>

class Worker : public Nan::AsyncWorker {
 public:
  Worker(Nan::Callback *callback);
  ~Worker();

  void Execute();
  void HandleOKCallback();
 private:
  kinfo_proc* processes;
  uint32_t* processCount;
};
