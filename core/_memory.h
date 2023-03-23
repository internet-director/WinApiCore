#ifndef _MEMORY_HDR
#define _MEMORY_HDR
#include "config.h"

namespace mem 
{
	void* memcpy(void* dst, const void* src, size_t sz);
	void* memset(void* dst, int byte, size_t sz);
	int memcmp(const void* dst, const void* src, size_t sz);
	void* alloc(size_t sz);
	int free(void* heap);
}


size_t int2str(LPWSTR str, uint64_t num);
BOOL compare_wide(WCHAR a, WCHAR b);
uint64_t div_up(uint64_t x, uint64_t y);
#endif