#include <napi.h>
#include <uv.h>
#include "worker.h"
using namespace Napi;

Napi::Value GetProcessList(const Napi::CallbackInfo &info)
{
    auto env = info.Env();
    auto callback = info[0].As<Napi::Function>();
    (new Worker(callback))->Queue();
    return env.Null();
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    exports.Set("getProcessList", Napi::Function::New(env, GetProcessList));
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
