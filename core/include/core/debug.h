#pragma once
#include <core/config.h>

#ifndef KEEPS_ALL_LOGS
struct debug {
	debug(const CHAR* str) {
	}
	debug(const CHAR* str, DWORD len) {
	}
	debug(const WCHAR* str) {
	}
	debug(const WCHAR* str, DWORD len) {
	}
};
#else
struct debug
{
	debug(const CHAR* str) {
		DWORD sz = lstrlenA(str);
		printA(str, sz);
	}
	debug(const CHAR* str, DWORD len) {
		printA(str, len);
	}

	debug(const WCHAR* str) {
		DWORD sz = lstrlenW(str);
		printW(str, sz);
	}
	debug(const WCHAR* str, DWORD len) {
		printW(str, len);
	}

private:
	void printA(const CHAR* str, DWORD len) {
		HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);

		if (hout == INVALID_HANDLE_VALUE) {
			return;
		}
		WriteConsoleA(hout, str, len, &len, NULL);
	}
	void printW(const WCHAR* str, DWORD len) {
		HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);

		if (hout == INVALID_HANDLE_VALUE) {
			return;
		}
		WriteConsoleW(hout, str, len, &len, NULL);
	}
};
#endif