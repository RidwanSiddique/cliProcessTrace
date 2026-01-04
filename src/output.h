#pragma once
#include <stdio.h>
#include <stdint.h>
#include "traceproc.h"

void tp_print_csv_header(FILE *out);
void tp_emit_csv(FILE *out, const char *ts, const tp_sample_t *cur, uint64_t delta_total_ns);

void tp_emit_jsonl(FILE *out, const char *ts, const tp_sample_t *cur, uint64_t delta_total_ns);
