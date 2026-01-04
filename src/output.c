#include "output.h"
#include "util.h"

void tp_print_csv_header(FILE *out) {
    fprintf(out, "time,pid,name,state,rss_mib,vms_mib,user_ms,sys_ms,total_ms,delta_total_ms\n");
}

void tp_emit_csv(FILE *out, const char *ts, const tp_sample_t *cur, uint64_t delta_total_ns) {
    double rss_mib = tp_bytes_to_mib(cur->rss_bytes);
    double vms_mib = tp_bytes_to_mib(cur->vms_bytes);

    uint64_t user_ms = cur->user_ns / 1000000ULL;
    uint64_t sys_ms  = cur->sys_ns  / 1000000ULL;
    uint64_t total_ms = (cur->user_ns + cur->sys_ns) / 1000000ULL;
    uint64_t delta_ms = delta_total_ns / 1000000ULL;

    fprintf(out, "%s,%d,\"%s\",\"%s\",%.2f,%.2f,%llu,%llu,%llu,%llu\n",
            ts,
            cur->pid,
            cur->name,
            cur->state,
            rss_mib,
            vms_mib,
            (unsigned long long)user_ms,
            (unsigned long long)sys_ms,
            (unsigned long long)total_ms,
            (unsigned long long)delta_ms
    );
}

static void json_escape(FILE *out, const char *s) {
    // minimal JSON string escape
    for (; *s; s++) {
        if (*s == '\\' || *s == '"') fputc('\\', out);
        fputc(*s, out);
    }
}

void tp_emit_jsonl(FILE *out, const char *ts, const tp_sample_t *cur, uint64_t delta_total_ns) {
    fprintf(out, "{\"time\":\"");
    json_escape(out, ts);
    fprintf(out, "\",\"pid\":%d,\"name\":\"", cur->pid);
    json_escape(out, cur->name);
    fprintf(out, "\",\"state\":\"");
    json_escape(out, cur->state);
    fprintf(out,
        "\",\"rss_bytes\":%llu,\"vms_bytes\":%llu,"
        "\"user_ns\":%llu,\"sys_ns\":%llu,\"delta_total_ns\":%llu}\n",
        (unsigned long long)cur->rss_bytes,
        (unsigned long long)cur->vms_bytes,
        (unsigned long long)cur->user_ns,
        (unsigned long long)cur->sys_ns,
        (unsigned long long)delta_total_ns
    );
}
