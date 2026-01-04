#include "platform.h"

#ifndef __APPLE__
#if defined(__linux__)
#include <string.h>

int tp_platform_sample(int pid, tp_sample_t *out) {
    (void)pid;
    if (!out) return 1;
    memset(out, 0, sizeof(*out));
    // TODO (planned): Implement Linux backend using /proc/<pid>/stat and /proc/<pid>/status
    // Fields to populate: name/state/rss/vms/user_ns/sys_ns (convert from ticks)
    return 3; // unsupported for now
}
#else
int tp_platform_sample(int pid, tp_sample_t *out) { (void)pid; (void)out; return 3; }
#endif
#endif
