#pragma once
#include <stdint.h>
#include <stddef.h>

uint64_t tp_now_epoch_ms(void);
void tp_iso_timestamp(char *buf, size_t n);
double tp_bytes_to_mib(uint64_t b);
