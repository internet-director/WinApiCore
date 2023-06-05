#pragma once
#include "config.h"

namespace core
{
	MEM_EXPORT void memInit();
	MEM_EXPORT constexpr void* memcpy(void* dst, const void* src, size_t sz);
	MEM_EXPORT constexpr void* memset(void* dst, int byte, size_t sz);
	MEM_EXPORT constexpr int memcmp(const void* dst, const void* src, size_t sz);
	MEM_EXPORT constexpr void* zeromem(void* dst, size_t sz);

	MEM_EXPORT void* alloc(size_t sz);
	template<typename T>
	MEM_EXPORT T* talloc(size_t sz) {
		return static_cast<T*>(alloc(sz * sizeof(T)));
	}
	MEM_EXPORT int free(void* heap);

	template<typename T>
	MEM_EXPORT constexpr size_t strlen(const T* str)
	{
		size_t sz = -1;
		while (str[++sz]);
		return sz;
	}

	void Wide2Char(const WCHAR* data, char* out, UINT len);
}