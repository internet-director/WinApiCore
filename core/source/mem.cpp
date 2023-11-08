#include "pch.h"
#include "mem.h"

HANDLE proc_heap = nullptr;

namespace core {
	
	void memInit() {
		proc_heap = core::GetProcessHeap();
	}
	constexpr volatile void* memcpy(volatile void* dst, const void* src, size_t sz) {
		for (volatile size_t i = 0; i < sz; i++) {
			((uint8_t*)dst)[i] = ((uint8_t*)src)[i];
		}
		return dst;
	}
	constexpr volatile void* memset(volatile void* dst, int byte, size_t sz) {
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

	constexpr volatile void* zeromem(volatile void* dst, size_t sz)
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

	int free(void* heap) noexcept
	{
		return API(KERNEL32, HeapFree)(proc_heap, NULL, heap);
	}

	void Wide2Char(const WCHAR* data, char* out, UINT len)
	{
		for (int i = 0; i < len; i++) {
			if (!data[i]) {
				out[i] = 0;
				break;
			}
			// TODO
			out[i] = data[i];
		}
	}
}

void* operator new(size_t size)
{
	return core::alloc(size);
}

void operator delete(void* p) noexcept
{
	core::free(p);
}

void operator delete(void* ptr, size_t) noexcept {
	core::free(ptr);
}

void* _cdecl operator new(size_t _Size, void* _Where) noexcept
{
	(void)_Size;
	return _Where;
}