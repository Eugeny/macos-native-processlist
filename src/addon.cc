#include <nan.h>
#include "worker.h"
using namespace Nan;

NAN_METHOD(GetProcessList) {
  auto callback = new Callback(info[0].As<v8::Function>());
  AsyncQueueWorker(new Worker(callback));
}

NAN_MODULE_INIT(Init) {
  Export(target, "getProcessList", GetProcessList);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init)
