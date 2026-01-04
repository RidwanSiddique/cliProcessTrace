#include "platform.h"

#ifdef _WIN32
#include <string.h>

int tp_platform_sample(int pid, tp_sample_t *out) {
    (void)pid;
    if (!out) return 1;
    memset(out, 0, sizeof(*out));
    // TODO (planned): Implement Windows backend using:
    // - OpenProcess + GetProcessTimes for user/sys time
    // - GetProcessMemoryInfo (PSAPI) for RSS/WorkingSet
    // - QueryFullProcessImageName or similar for name
    return 3; // unsupported for now
}
#else
int tp_platform_sample(int pid, tp_sample_t *out) { (void)pid; (void)out; return 3; }
#endif
