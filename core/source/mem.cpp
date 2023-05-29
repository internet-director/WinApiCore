#include "pch.h"
#include "mem.h"

HANDLE proc_heap = nullptr;

namespace core {
	
	void memInit() {
		proc_heap = API(KERNEL32, GetProcessHeap)();
	}
	constexpr void* memcpy(void* dst, const void* src, size_t sz) {
		for (volatile size_t i = 0; i < sz; i++) {
			((uint8_t*)dst)[i] = ((uint8_t*)src)[i];
		}
		return dst;
	}
	constexpr void* memset(void* dst, int byte, size_t sz) {
		for (volatile size_t i = 0; i < sz; i++) {
			((uint8_t*)dst)[i] = byte;
		}
		return dst;
	}
	constexpr int memcmp(const void* dst, const void* src, size_t sz)
	{
		uint8_t* bdst = (uint8_t*)dst;
		uint8_t* bsrc = (uint8_t*)src;

		for (size_t i = 0; i < sz; i++) {
			if (bdst[i] != bsrc[i]) return (bdst[i] > bsrc[i]) ? 1 : -1;
		}
		return 0;
	}

	constexpr void* zeromem(void* dst, size_t sz)
	{
		return memset(dst, 0, sz);
	}

	void* alloc(size_t sz)
	{
		if (proc_heap == nullptr) {
			core::memInit();
		}
		return API(KERNEL32, HeapAlloc)(proc_heap, NULL, sz);
	}

	int free(void* heap)
	{
		return API(KERNEL32, HeapFree)(proc_heap, NULL, heap);
	}
}