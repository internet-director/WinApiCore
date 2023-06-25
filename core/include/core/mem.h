#pragma once
#include "config.h"
#include <core/types.h>
#include <core/NtApi.h>

namespace core
{
	MEM_EXPORT void memInit();
	MEM_EXPORT constexpr volatile void* memcpy(volatile void* dst, const void* src, size_t sz);
	MEM_EXPORT constexpr volatile void* memset(volatile void* dst, int byte, size_t sz);
	MEM_EXPORT constexpr int memcmp(const void* dst, const void* src, size_t sz);
	MEM_EXPORT constexpr volatile void* zeromem(volatile void* dst, size_t sz);

	template<typename T>
	MEM_EXPORT constexpr void fill(T* dst, const T& obj, size_t sz) {
		for (size_t i = 0; i < sz; i++) {
			dst[i] = obj;
		}
	}

	template<typename T>
	MEM_EXPORT constexpr void zero(T& obj) {
		zeromem(&obj, sizeof T);
	}

	MEM_EXPORT void* alloc(size_t sz);
	MEM_EXPORT int free(void* heap);

	template<typename T>
	MEM_EXPORT T* talloc(size_t sz) {
		return static_cast<T*>(alloc(sz * sizeof(T)));
	}

	template<typename T>
	MEM_EXPORT constexpr size_t strlen(const T* str)
	{
		size_t sz = -1;
		while (str[++sz]);
		return sz;
	}

	void Wide2Char(const WCHAR* data, char* out, UINT len);

	template<typename T>
	size_t find(const T* array, const T& obj, size_t sz) {
		for (size_t i = 0; i < sz; i++) {
			if (array[i] == obj) return i;
		}
		return core::npos;
	}
}