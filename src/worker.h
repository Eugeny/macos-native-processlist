#pragma once

#include <napi.h>
#include <memory>
#include <sys/sysctl.h>

class Worker : public Napi::AsyncWorker
{
public:
    Worker(Napi::Function &callback);
    ~Worker();

    void Execute();
    void OnOK();

private:
    std::unique_ptr<kinfo_proc[]> processes;
    size_t processCount;
};
