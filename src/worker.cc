#include "worker.h"
#include <sys/sysctl.h>
#include <libproc.h>
#include <array>
#include <system_error>

Worker::Worker(Napi::Function &callback) : AsyncWorker(callback), processCount(0)
{
}

Worker::~Worker()
{
}

void Worker::Execute()
{
    static int mib[] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL};

    while (1)
    {
        size_t length = 0;
        int err = sysctl(mib, std::size(mib), nullptr, &length, nullptr, 0);
        if (err == -1) // This should not fail normally, throw if failed
            throw std::system_error(errno, std::generic_category());

        this->processes = std::make_unique<kinfo_proc[]>(length / sizeof(kinfo_proc));

        err = sysctl(mib, std::size(mib), this->processes.get(), &length, nullptr, 0);
        if (err == -1)
        {
            if (errno != ENOMEM) // Try again if we got ENOMEM, otherwise throw
                throw std::system_error(errno, std::generic_category());
        }
        else
        {
            this->processCount = length / sizeof(kinfo_proc);
            break;
        }
    }
}

constexpr size_t NAME_BUFFER_SIZE = 4096;

void Worker::OnOK()
{
    auto env = this->Env();
    auto result = Napi::Array::New(env);
    for (size_t i = 0; i < this->processCount; i++)
    {
        auto object = Napi::Object::New(env);

        char buffer[NAME_BUFFER_SIZE];
        proc_name(this->processes[i].kp_proc.p_pid, buffer, NAME_BUFFER_SIZE);
        object.Set("name", Napi::String::New(env, buffer));
        proc_pidpath(this->processes[i].kp_proc.p_pid, buffer, NAME_BUFFER_SIZE);
        object.Set("path", Napi::String::New(env, buffer));
        object.Set("pid", Napi::Number::New(env, this->processes[i].kp_proc.p_pid));
        object.Set("ppid", Napi::Number::New(env, this->processes[i].kp_eproc.e_ppid));
        object.Set("pgid", Napi::Number::New(env, this->processes[i].kp_eproc.e_pgid));
        result.Set(i, object);
    }

    this->Callback().Call({result});
}
