// vim: set et ts=4 sw=4:

#pragma once

#include "types.h"

extern "C" void* memcpy(void *dest, void const *src, size_t size);
extern "C" void* memmove(void *dest, void const *src, size_t size);
extern "C" void* memset(void *dest, int pattern, size_t size);

