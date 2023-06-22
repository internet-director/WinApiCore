#include <Windows.h>
#include <core/core.h>

#define VATORAW(section, offset) ( (size_t)section->PointerToRawData + (size_t)offset - (size_t)section->VirtualAddress )


#ifdef _DEBUG 
#include <iostream>
int main()
#else
int entry()
#endif
{
	size_t fHash = core::hash32::calculate("NtCreateFile");
	HANDLE h = API(KERNEL32, CreateFileW)(L"C:\\Windows\\system32\\ntdll.dll", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (h != INVALID_HANDLE_VALUE) {
		std::cout << "win";

		LARGE_INTEGER liFileSize;

		if (GetFileSizeEx(h, &liFileSize)) {
			HANDLE hMap = CreateFileMappingW(h, NULL, PAGE_READONLY, 0, 0, NULL);

			if (hMap != nullptr) {
				HANDLE lpBasePtr = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
				if (lpBasePtr != nullptr) {
					PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)lpBasePtr;
					PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)core::Wobf::rvatova(lpBasePtr, dos->e_lfanew);
					IMAGE_FILE_HEADER f = nt->FileHeader;
					PIMAGE_DATA_DIRECTORY exportData = &nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

					PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(nt);
					DWORD offset = 0;
					for (size_t i = 0; i < f.NumberOfSections; i++, section++) {
						if (exportData->VirtualAddress >= section->VirtualAddress &&
							exportData->VirtualAddress < section->VirtualAddress + section->Misc.VirtualSize) {
							break;
						}
					}

					PIMAGE_EXPORT_DIRECTORY exportTable = (PIMAGE_EXPORT_DIRECTORY)core::Wobf::rvatova(lpBasePtr, VATORAW(section, exportData->VirtualAddress));

					PDWORD name = (PDWORD)core::Wobf::rvatova(lpBasePtr, VATORAW(section, exportTable->AddressOfNames));
					PDWORD functions = (PDWORD)core::Wobf::rvatova(lpBasePtr, VATORAW(section, exportTable->AddressOfFunctions));
					PWORD ordAddress = (PWORD)core::Wobf::rvatova(lpBasePtr, VATORAW(section, exportTable->AddressOfNameOrdinals));

					char* n = nullptr;
					for (int i = 0; i < exportTable->NumberOfNames; i++) {
						n = (char*)core::Wobf::rvatova(lpBasePtr, VATORAW(section, name[i]));
						std::cout << n << "\n";
						if (fHash == core::hash32::calculate(n)) {
							size_t functionRVA = VATORAW(section, functions[ordAddress[i]]);
							HANDLE functionAddr = (HANDLE)core::Wobf::rvatova(lpBasePtr, VATORAW(section, functionRVA));
							for (int j = 0; j < 60; j++) {
								std::cout << std::hex << (int)((uint8_t*)functionAddr)[j] << " ";
							}
							break;

						}
					}

					UnmapViewOfFile(lpBasePtr);
				}
			}
		}

		/*
		PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)lib;
		PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)rvatova(lib, dos->e_lfanew);
		IMAGE_FILE_HEADER f = nt->FileHeader;
		PIMAGE_DATA_DIRECTORY exportData = &nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
		PIMAGE_EXPORT_DIRECTORY data = (PIMAGE_EXPORT_DIRECTORY)rvatova(lib, exportData->VirtualAddress);
		PDWORD name = (PDWORD)rvatova(lib, data->AddressOfNames);
		PDWORD functions = (PDWORD)rvatova(lib, data->AddressOfFunctions);
		PWORD ordAddress = (PWORD)rvatova(lib, data->AddressOfNameOrdinals);

		char* n = nullptr;
		for (int i = 0; i < data->NumberOfNames; i++) {
			n = (char*)rvatova(lib, name[i]);
			if (fHash == core::hash32::calculate(n)) {
				if (locked) lock();
				size_t functionRVA = functions[ordAddress[i]];
				HANDLE functionAddr = (HANDLE)rvatova(lib, functionRVA);

				// function forwarded
				if (functionRVA > (size_t)exportData->VirtualAddress &&
					functionRVA < (size_t)exportData->VirtualAddress + exportData->Size) {
					char dllName[MAX_PATH];
					LPCSTR forwardedFunctionName = reinterpret_cast<LPCSTR>(rvatova(lib, functionRVA));
					size_t dotIndex = core::find(forwardedFunctionName, '.',
						core::strlen(forwardedFunctionName));

					// invalid forwaring
					if (dotIndex == core::npos) {
						if (locked) release();
						return nullptr;
					}

					core::memcpy(dllName, forwardedFunctionName, dotIndex);
					core::memcpy(dllName + dotIndex, ".dll", 4);
					dllName[dotIndex + 4] = 0;

					HANDLE forwardedModule = GetOrLoadDll(core::hash32::calculate(dllName));
					// TODO: fix recursion
					functionAddr = GetApiAddr(forwardedModule, core::hash32::calculate(forwardedFunctionName + dotIndex + 1), false);
				}
				if (functionAddr == nullptr) {
					if (locked) release();
					return nullptr;
				}

				apiArray[apiCounter].addr = functionAddr;
				apiArray[apiCounter++].hash = fHash;
				if (locked) release();
				return functionAddr;
			}
		}*/
	}

	return 0;
	core::Process proc;
	WCHAR arg[] = L"notepad";
	if (proc.run(NULL, arg, NULL)) {
		proc.wait(1000);
		proc.kill();
	}

	return 0;
	/*
	int argc = 0;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	if (argc < 2) return false;

	bool res = false;
	core::Process proc(core::ProcessMonitor::getPid(argv[1]), PROCESS_TERMINATE);
	if (proc.isOpen()) {
		proc.kill();
		res = true;
	}
	if(res) {
		debug(L"done");
	}
	else {
		debug(L"error");
	}

	return 0;*/
}