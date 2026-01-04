#pragma once
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int pid;
    char name[256];      // best-effort
    char state[64];      // best-effort

    uint64_t rss_bytes;  // resident memory
    uint64_t vms_bytes;  // virtual memory

    uint64_t user_ns;    // user CPU time (ns, best-effort)
    uint64_t sys_ns;     // system CPU time (ns, best-effort)
} tp_sample_t;

typedef enum {
    TP_FMT_CSV = 0,
    TP_FMT_JSONL = 1
} tp_format_t;

typedef struct {
    int pid;
    int interval_ms;       // default 500
    int duration_sec;      // 0 = run until Ctrl+C or process ends
    tp_format_t format;    // csv/jsonl
    const char *out_path;  // NULL => stdout
    int header;            // CSV header
} tp_config_t;

#ifdef __cplusplus
}
#endif
