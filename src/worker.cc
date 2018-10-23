#include "worker.h"
#include <libproc.h>

using namespace Nan;

Worker::Worker(Callback* callback) : AsyncWorker(callback) {
    this->processes = new kinfo_proc[1024];
    processCount = new uint32_t;
}

Worker::~Worker() {
  delete[] this->processes;
  delete processCount;
}

void Worker::Execute() {
    bool done;
    int err;
    size_t length = 0;
    static const int name[] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0 };

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
    do {
        assert(this->processes == NULL);

        // Call sysctl with a NULL buffer.

        length = 0;
        err = sysctl((int*)name, sizeof(name) / sizeof(*name) - 1, NULL, &length, NULL, 0);
        if (err == -1) {
            err = errno;
        }

        // Allocate an appropriately sized buffer based on the results
        // from the previous call.

        if (err == 0) {
            this->processes = (kinfo_proc*)malloc(length);
        }

        // Call sysctl again with the new buffer.  If we get an ENOMEM
        // error, toss away our buffer and start again.

        if (err == 0) {
            err = sysctl((int*)name, sizeof(name) / sizeof(*name) - 1, this->processes, &length, NULL, 0);
            if (err == -1) {
                err = errno;
            }
            if (err == 0) {
                done = true;
            } else if (err == ENOMEM) {
                assert(this->processes != NULL);
                free(this->processes);
                this->processes = NULL;
                err = 0;
            }
        }
    } while (err == 0 && ! done);

    if (err == 0) {
        *this->processCount = length / sizeof(kinfo_proc);
    }
}

#define NAME_BUFFER_SIZE 4096

void Worker::HandleOKCallback() {
  HandleScope scope;

  v8::Local<v8::Array> result = New<v8::Array>(*processCount);
  for (uint32_t i = 0; i < *processCount; i++) {
    v8::Local<v8::Object> object = New<v8::Object>();

    char buffer[NAME_BUFFER_SIZE];
    proc_name(this->processes[i].kp_proc.p_pid, buffer, NAME_BUFFER_SIZE);
    Set(object, New<v8::String>("name").ToLocalChecked(), New<v8::String>(buffer).ToLocalChecked());
    proc_pidpath(this->processes[i].kp_proc.p_pid, buffer, NAME_BUFFER_SIZE);
    Set(object, New<v8::String>("path").ToLocalChecked(), New<v8::String>(buffer).ToLocalChecked());
    Set(object, New<v8::String>("pid").ToLocalChecked(), New<v8::Number>(this->processes[i].kp_proc.p_pid));
    Set(object, New<v8::String>("ppid").ToLocalChecked(), New<v8::Number>(this->processes[i].kp_eproc.e_ppid));
    Set(object, New<v8::String>("pgid").ToLocalChecked(), New<v8::Number>(this->processes[i].kp_eproc.e_pgid));
    Set(result, i, New<v8::Value>(object));
  }

  v8::Local<v8::Value> argv[] = { result };
  AsyncResource resource("macos-native-processlist:addon.HandleOKCallback");
  callback->Call(1, argv, &resource);
}
