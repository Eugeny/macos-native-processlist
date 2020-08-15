#pragma once
#include <napi.h>
#include <uv.h>
#include <sys/sysctl.h>
#include <pwd.h>

class Worker : public Napi::AsyncWorker
{
public:
    Worker(Napi::Function &callback);
    ~Worker();

    void Execute();
    void OnOK();

private:
    kinfo_proc *processes;
    uint32_t *processCount;
};
