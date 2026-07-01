
#ifndef UTILS_UTILS_H
#define UTILS_UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

int ct_utils_format(char* buffer, size_t size, const char* format, ...);

bool ct_utils_isBitSet(uint64_t value, int n);
void ct_utils_setBit(uint64_t *value, int n);
void ct_utils_clearBit(uint64_t *value, int n);

#endif // UTILS_UTILS_H