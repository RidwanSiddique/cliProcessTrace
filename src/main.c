#define _POSIX_C_SOURCE 200809L
#include "traceproc.h"
#include "platform.h"
#include "util.h"
#include "output.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

static volatile sig_atomic_t g_stop = 0;
static void on_sigint(int sig) { (void)sig; g_stop = 1; }

static void usage(const char *p) {
    fprintf(stderr,
        "traceproc - portable process trace sampler (C)\n\n"
        "Usage:\n"
        "  %s <pid> [--interval <ms>] [--duration <sec>] [--format csv|jsonl] [--out <file>] [--header]\n\n"
        "Examples:\n"
        "  %s 1234 --interval 250 --format csv --header\n"
        "  %s 1234 --interval 500 --duration 10 --format jsonl --out trace.jsonl\n",
        p, p, p
    );
}

static tp_format_t parse_format(const char *s) {
    if (!s) return TP_FMT_CSV;
    if (strcmp(s, "csv") == 0) return TP_FMT_CSV;
    if (strcmp(s, "jsonl") == 0) return TP_FMT_JSONL;
    return TP_FMT_CSV;
}

static void sleep_ms(int ms) {
    if (ms < 1) ms = 1;
#ifdef _WIN32
    // Windows: Sleep(ms)
    #include <windows.h>
    Sleep((DWORD)ms);
#else
    #include <time.h>
    struct timespec req = { .tv_sec = ms / 1000, .tv_nsec = (ms % 1000) * 1000000L };
    nanosleep(&req, NULL);
#endif
}

int main(int argc, char **argv) {
    if (argc < 2 || strcmp(argv[1], "--help") == 0) { usage(argv[0]); return 2; }

    tp_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.interval_ms = 500;
    cfg.format = TP_FMT_CSV;
    cfg.duration_sec = 0;
    cfg.out_path = NULL;
    cfg.header = 0;

    // PID
    char *end = NULL;
    long pid_l = strtol(argv[1], &end, 10);
    if (*argv[1] == '\0' || (end && *end != '\0') || pid_l <= 0) {
        fprintf(stderr, "Invalid PID: %s\n", argv[1]);
        return 2;
    }
    cfg.pid = (int)pid_l;

    // Args
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--interval") == 0 && i + 1 < argc) {
            cfg.interval_ms = atoi(argv[++i]);
            if (cfg.interval_ms < 50) cfg.interval_ms = 50;
        } else if (strcmp(argv[i], "--duration") == 0 && i + 1 < argc) {
            cfg.duration_sec = atoi(argv[++i]);
            if (cfg.duration_sec < 0) cfg.duration_sec = 0;
        } else if (strcmp(argv[i], "--format") == 0 && i + 1 < argc) {
            cfg.format = parse_format(argv[++i]);
        } else if (strcmp(argv[i], "--out") == 0 && i + 1 < argc) {
            cfg.out_path = argv[++i];
        } else if (strcmp(argv[i], "--header") == 0) {
            cfg.header = 1;
        } else {
            usage(argv[0]);
            return 2;
        }
    }

    // Output
    FILE *out = stdout;
    if (cfg.out_path) {
        out = fopen(cfg.out_path, "a");
        if (!out) {
            fprintf(stderr, "Failed to open %s: %s\n", cfg.out_path, strerror(errno));
            return 1;
        }
    }

    signal(SIGINT, on_sigint);

    if (cfg.format == TP_FMT_CSV && cfg.header) {
        tp_print_csv_header(out);
        fflush(out);
    }

    tp_sample_t prev, cur;
    int have_prev = 0;

    uint64_t start_ms = tp_now_epoch_ms();

    while (!g_stop) {
        if (cfg.duration_sec > 0) {
            uint64_t now_ms = tp_now_epoch_ms();
            if ((now_ms - start_ms) >= (uint64_t)cfg.duration_sec * 1000ULL) break;
        }

        int rc = tp_platform_sample(cfg.pid, &cur);
        if (rc != 0) {
            fprintf(stderr, "Process %d not accessible (exited/permission/unsupported backend). Stopping.\n", cfg.pid);
            break;
        }

        uint64_t delta_total_ns = 0;
        if (have_prev) {
            uint64_t prev_total = prev.user_ns + prev.sys_ns;
            uint64_t cur_total  = cur.user_ns + cur.sys_ns;
            delta_total_ns = (cur_total >= prev_total) ? (cur_total - prev_total) : 0;
        }

        char ts[64];
        tp_iso_timestamp(ts, sizeof(ts));

        if (cfg.format == TP_FMT_CSV) tp_emit_csv(out, ts, &cur, delta_total_ns);
        else tp_emit_jsonl(out, ts, &cur, delta_total_ns);

        fflush(out);

        prev = cur;
        have_prev = 1;

        sleep_ms(cfg.interval_ms);
    }

    if (out != stdout) fclose(out);
    return 0;
}
