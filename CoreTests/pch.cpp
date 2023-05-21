//
// pch.cpp
//

#include "pch.h"

#define RVATOVA(base, offset) ((SIZE_T)base + (SIZE_T)offset)


bool runWaiter(STARTUPINFO& si, PROCESS_INFORMATION& pi) {
	core::zeromem(&si, sizeof(si));
	core::zeromem(&pi, sizeof(pi));
	si.cb = sizeof(si);

#ifdef X64
	const WCHAR* procName = L"..\\bin_x64\\waiter.exe";
#else
	const WCHAR* procName = L"..\\bin_x86\\waiter.exe";
#endif

	return CreateProcessW(procName, 0, 0, 0, 0, 0, 0, 0, &si, &pi);
}

void killWaiter(STARTUPINFO& si, PROCESS_INFORMATION& pi) {
	if (TerminateProcess(pi.hProcess, 1)) {
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
}

bool goodHash(const char* dll) {
	std::set<core::uint32_t> hashs;
	HANDLE lib = LoadLibraryA(dll);

	if (lib == nullptr) return false;

	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)lib;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)RVATOVA(lib, dos->e_lfanew);
	IMAGE_FILE_HEADER f = nt->FileHeader;
	IMAGE_OPTIONAL_HEADER opt = nt->OptionalHeader;
	PIMAGE_EXPORT_DIRECTORY data = (PIMAGE_EXPORT_DIRECTORY)RVATOVA(lib, opt.DataDirectory[0].VirtualAddress);
	PDWORD name = (PDWORD)RVATOVA(lib, data->AddressOfNames);

	char* n = nullptr;
	for (int i = 0; i < data->NumberOfNames; i++) {
		n = (char*)RVATOVA(lib, *name);
		core::uint32_t hash = core::hash32::calculate(n);
		if (hashs.count(hash) != 0) return false;
		hashs.insert(hash);
		name++;
	}
	return true;
}