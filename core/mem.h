#pragma once
#include "config.h"

#define API_EXPORT __declspec(dllexport)

namespace core
{
	API_EXPORT void memInit();
	API_EXPORT void* memcpy(void* dst, const void* src, size_t sz);
	API_EXPORT void* memset(void* dst, int byte, size_t sz);
	API_EXPORT int memcmp(const void* dst, const void* src, size_t sz);
	API_EXPORT void* alloc(size_t sz);
	API_EXPORT int free(void* heap);
}
