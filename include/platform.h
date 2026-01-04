#pragma once
#include "traceproc.h"

// Returns 0 on success.
// Nonzero means process not accessible / exited / unsupported on this platform backend.
int tp_platform_sample(int pid, tp_sample_t *out);
