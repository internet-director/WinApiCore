#include "_memory.h"
#include <Windows.h>

static HANDLE proc_heap = GetProcessHeap();

namespace mem {
	void* memcpy(void* dst, const void* src, size_t sz) {
		uint8_t* bdst = (uint8_t*)dst;
		uint8_t* bsrc = (uint8_t*)src;

		while (sz) bdst[--sz] = bsrc[sz];
		return dst;
	}
	void* memset(void* dst, int byte, size_t sz) {
		uint8_t* bdst = (uint8_t*)dst;

		while (sz) bdst[--sz] = byte;
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



size_t int2str(LPWSTR str, uint64_t num)
{
	uint64_t i = 0;
	if (num == 0) {
		str[0] = L'0';
		str[1] = 0;
		return 1;
	}
	while (num) {
		str[i++] = L'0' + num % 10;
		num /= 10;
	}
	for (size_t j = 0; j < i / 2; j++) {
		WCHAR tmp = str[j];
		str[j] = str[i - j - 1];
		str[i - j - 1] = tmp;
	}
	str[i] = 0;
	return i;
}

BOOL compare_wide(WCHAR a, WCHAR b) {
	static WCHAR a1[2] = { 0 }, b1[2] = { 0 };
	a1[0] = a;
	b1[0] = b;
	return !lstrcmpW(a1, b1);
}

uint64_t div_up(uint64_t x, uint64_t y)
{
	return (x - 1) / y + 1;
}

void gen_rand_bytes(uint8_t* ptr, size_t sz)
{
	for (size_t i = 0; i < sz; i++) {

	}
}
