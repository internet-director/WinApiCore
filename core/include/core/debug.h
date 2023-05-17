#pragma once
#include <core/debug.h>

struct  debug
{
	debug(const WCHAR* str) {
		DWORD sz = lstrlenW(str);
		print(str, sz);
	}
	debug(const WCHAR* str, DWORD len) {
		print(str, len);
	}

private:
	void print(const WCHAR* str, DWORD len) {
		HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);

		if (hout == INVALID_HANDLE_VALUE) {
			return;
		}
		WriteConsoleW(hout, str, len, &len, NULL);
	}
};