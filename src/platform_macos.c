#include "platform.h"

#ifdef __APPLE__

#include <string.h>
#include <stdio.h>
#include <libproc.h>
#include <sys/proc_info.h>

static void safe_copy(char *dst, size_t cap, const char *src) {
    if (!dst || cap == 0) return;
    if (!src) { dst[0] = '\0'; return; }
    snprintf(dst, cap, "%s", src);
}

int tp_platform_sample(int pid, tp_sample_t *out) {
    if (!out) return 1;
    memset(out, 0, sizeof(*out));
    out->pid = pid;

    // Name (best-effort)
    char namebuf[256];
    int nr = proc_name(pid, namebuf, sizeof(namebuf));
    if (nr > 0) safe_copy(out->name, sizeof(out->name), namebuf);
    else safe_copy(out->name, sizeof(out->name), "(unknown)");

    // Task info (memory + cpu)
    struct proc_taskinfo ti;
    int bytes = proc_pidinfo(pid, PROC_PIDTASKINFO, 0, &ti, sizeof(ti));
    if (bytes != sizeof(ti)) return 2; // exited/permission

    out->rss_bytes = (uint64_t)ti.pti_resident_size;
    out->vms_bytes = (uint64_t)ti.pti_virtual_size;

    // Apple docs: total_user/total_system are nanoseconds (best-effort)
    out->user_ns = (uint64_t)ti.pti_total_user;
    out->sys_ns  = (uint64_t)ti.pti_total_system;

    // Best-effort process status
    struct proc_bsdinfo bi;
    bytes = proc_pidinfo(pid, PROC_PIDTBSDINFO, 0, &bi, sizeof(bi));
    if (bytes == sizeof(bi)) {
        snprintf(out->state, sizeof(out->state), "status=%d", bi.pbi_status);
    } else {
        snprintf(out->state, sizeof(out->state), "status=?");
    }

    return 0;
}

#else
int tp_platform_sample(int pid, tp_sample_t *out) { (void)pid; (void)out; return 1; }
#endif
