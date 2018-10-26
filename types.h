// vim: set et ts=4 sw=4:

#pragma once

typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned long      uintptr_t;

typedef uint32_t           size_t;

typedef char               int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef long               intptr_t;

typedef int32_t            ssize_t;
#define SSIZE_MAX 2147483647

#ifndef NULL
#define NULL 0
#endif

// Validierung der Größe (für 32bit)
#include "debug/assert.h"
assert_size(int8_t, 1);
assert_size(int16_t, 2);
assert_size(int32_t, 4);
assert_size(int64_t, 8);
assert_size(intptr_t, sizeof(void*));
assert_size(uint8_t, 1);
assert_size(uint16_t, 2);
assert_size(uint32_t, 4);
assert_size(uint64_t, 8);
assert_size(uintptr_t, sizeof(void*));
assert_size(size_t, 4);

