#include "worker.h"
#include <libproc.h>

Worker::Worker(Napi::Function &callback) : AsyncWorker(callback)
{
    this->processes = new kinfo_proc[1024];
    processCount = new uint32_t;
}

Worker::~Worker()
{
    delete[] this->processes;
    delete processCount;
}

void Worker::Execute()
{
    bool done;
    int err;
    size_t length = 0;
    static const int name[] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0};

    *this->processCount = 0;

    // We start by calling sysctl with this->processes == NULL and length == 0.
    // That will succeed, and set length to the appropriate length.
    // We then allocate a buffer of that size and call sysctl again
    // with that buffer.  If that succeeds, we're done.  If that fails
    // with ENOMEM, we have to throw away our buffer and loop.  Note
    // that the loop causes use to call sysctl with NULL again; this
    // is necessary because the ENOMEM failure case sets length to
    // the amount of data returned, not the amount of data that
    // could have been returned.

    this->processes = NULL;
    done = false;
    do
    {
        assert(this->processes == NULL);

        // Call sysctl with a NULL buffer.

        length = 0;
        err = sysctl((int *)name, sizeof(name) / sizeof(*name) - 1, NULL, &length, NULL, 0);
        if (err == -1)
        {
            err = errno;
        }

        // Allocate an appropriately sized buffer based on the results
        // from the previous call.

        if (err == 0)
        {
            this->processes = (kinfo_proc *)malloc(length);
        }

        // Call sysctl again with the new buffer.  If we get an ENOMEM
        // error, toss away our buffer and start again.

        if (err == 0)
        {
            err = sysctl((int *)name, sizeof(name) / sizeof(*name) - 1, this->processes, &length, NULL, 0);
            if (err == -1)
            {
                err = errno;
            }
            if (err == 0)
            {
                done = true;
            }
            else if (err == ENOMEM)
            {
                assert(this->processes != NULL);
                free(this->processes);
                this->processes = NULL;
                err = 0;
            }
        }
    } while (err == 0 && !done);

    if (err == 0)
    {
        *this->processCount = length / sizeof(kinfo_proc);
    }
}

#define NAME_BUFFER_SIZE 4096

void Worker::OnOK()
{
    auto env = this->Env();
    auto result = Napi::Array::New(env);
    for (uint32_t i = 0; i < *processCount; i++)
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
