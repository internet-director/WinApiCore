#include <Windows.h>
#include <core/debug.h>
#include <core/mem.h>

LPCWSTR black_list[] = { L".", L".." };

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

#ifdef _DEBUG 
int main()
#else
int entry()
#endif
{
	core::memInit();

	int argc = 0;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	if (!parseAruments(argc, argv)) {
		return 1;
	}

	LPWSTR buffer = static_cast<LPWSTR>(core::alloc((INT16_MAX + 6) * sizeof(WCHAR)));
	if (buffer == nullptr) {
		return 1;
	}
	{
		DWORD sz = GetCurrentDirectoryW(INT16_MAX, buffer + 4);
		core::memcpy(buffer, L"\\\\.\\", 4 * sizeof(WCHAR));
		core::memcpy(buffer + sz + 4, L"\\*", 3 * sizeof(WCHAR));
	}
	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFileW(buffer, & fd);

	if (hFile == INVALID_HANDLE_VALUE) {
		core::free(buffer);
		return 2;
	}

	do {
		bool bCheck = false;
		DWORD len = lstrlenW(fd.cFileName);
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
			debug(fd.cFileName, len);
			debug(END);
		}
	} while (FindNextFileW(hFile, &fd));

	FindClose(hFile);
	core::free(buffer);
	return 0;
}