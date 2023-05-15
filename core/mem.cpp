#include "pch.h"
#include "mem.h"

HANDLE proc_heap;

namespace core {
	void memInit() {
		proc_heap = GetProcessHeap();
	}
	void* memcpy(void* dst, const void* src, size_t sz) {
		for (volatile size_t i = 0; i < sz; i++) {
			((uint8_t*)dst)[i] = ((uint8_t*)src)[i];
		}
		return dst;
	}
	void* memset(void* dst, int byte, size_t sz) {
		for (volatile size_t i = 0; i < sz; i++) {
			((uint8_t*)dst)[i] = byte;
		}
		return dst;
	}
	int memcmp(const void* dst, const void* src, size_t sz)
	{
		uint8_t* bdst = (uint8_t*)dst;
		uint8_t* bsrc = (uint8_t*)src;

		for (size_t i = 0; i < sz; i++) {
			if (bdst[i] != bsrc[i]) return (bdst[i] > bsrc[i]) ? 1 : -1;
		}
		return 0;
	}

	void* alloc(size_t sz)
	{
		return HeapAlloc(proc_heap, NULL, sz);
	}

	int free(void* heap)
	{
		return HeapFree(proc_heap, NULL, heap);
	}
}