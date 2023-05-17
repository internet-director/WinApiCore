#pragma once
#include "config.h"

namespace core
{
	MEM_EXPORT void memInit();
	MEM_EXPORT void* memcpy(void* dst, const void* src, size_t sz);
	MEM_EXPORT void* memset(void* dst, int byte, size_t sz);
	MEM_EXPORT int memcmp(const void* dst, const void* src, size_t sz);
	MEM_EXPORT void* alloc(size_t sz);
	template<typename T>
	MEM_EXPORT T* talloc(size_t sz) {
		return static_cast<T*>(alloc(sz * sizeof(T)));
	}
	MEM_EXPORT int free(void* heap);
}
