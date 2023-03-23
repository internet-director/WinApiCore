#include <iostream>
#include <string>
#include <Windows.h>

#include <core/_memory.h>

#define __countof(X) sizeof(X) / sizeof(X[0])

LPCWSTR black_list[] = { L".", L".." };

int printConsole(LPCWSTR buffer, DWORD sz) 
{
	static HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);

	if (hout == INVALID_HANDLE_VALUE) {
		return -1;
	}

	WriteConsoleW(hout, buffer, sz, &sz, NULL);
	return sz;
}

struct _config {
	bool fullPrint = false;
} CONFIG;


bool parseAruments(int argc, LPWSTR* argv) 
{
	for (int i = 1; i < argc; i++) {
		if (!lstrcmpW(L".", argv[i])) CONFIG.fullPrint = true;
		else return false;
	}
	return true;
}

int main()
{
	int argc = 0;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	if (!parseAruments(argc, argv)) {
		return 1;
	}

	LPWSTR buffer = static_cast<LPWSTR>(mem::alloc((INT16_MAX + 6) * sizeof(WCHAR)));
	{
		DWORD sz = GetCurrentDirectoryW(INT16_MAX, buffer + 4);
		mem::memcpy(buffer, L"\\\\.\\", 4 * sizeof(WCHAR));
		mem::memcpy(buffer + sz + 4, L"\\*", 3 * sizeof(WCHAR));
	}
	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFileW(buffer, & fd);

	if (hFile == INVALID_HANDLE_VALUE) {
		mem::free(buffer);
		return 2;
	}

	do {
		bool bCheck = false;
		size_t len = wcslen(fd.cFileName);
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			for (uint32_t i = 0; i < __countof(black_list) && !bCheck; i++) {
				if (!lstrcmpW(fd.cFileName, black_list[i])) bCheck = true;
			}
			
		}
		else {
		}

		if (!bCheck && len != 0) {
			if(!CONFIG.fullPrint) {
				WCHAR skip[2] = { fd.cFileName[0], 0 };
				if (!lstrcmpW(skip, L".")) continue;
			}
			printConsole(fd.cFileName, len);
			printConsole(END, 2);
		}
	} while (FindNextFileW(hFile, &fd));

	FindClose(hFile);
	mem::free(buffer);
	return 0;
}