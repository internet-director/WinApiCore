#include <Windows.h>
#include <core/core.h>

/*
template<typename ...Args>
NTSTATUS __fastcall asm_nt(Args... args);
*/
extern "C" NTSTATUS __fastcall asm_nt(HANDLE h);
extern "C" NTSTATUS __fastcall asm_alloc(
	IN HANDLE               ProcessHandle,
	IN OUT PVOID * BaseAddress,
	IN ULONG                ZeroBits,
	IN OUT PULONG           RegionSize,
	IN ULONG                AllocationType,
	IN ULONG                Protect
);

//#define LOAD

#ifdef LOAD
#define VATORAW(section, offset) offset
#else
#define VATORAW(section, offset) ( (size_t)section->PointerToRawData + (size_t)offset - (size_t)section->VirtualAddress )
#endif

#define RVATOVA(x, y) ( (size_t)x + (size_t)y )

static
DWORD
WINAPI
RvaToOffset(
	PIMAGE_NT_HEADERS NtHeaders,
	DWORD Rva)
{
	PIMAGE_SECTION_HEADER SectionHeader;
	DWORD                 i, Size;

	if (Rva == 0) return 0;

	SectionHeader = IMAGE_FIRST_SECTION(NtHeaders);

	for (i = 0; i < NtHeaders->FileHeader.NumberOfSections; i++) {

		Size = SectionHeader[i].Misc.VirtualSize ?
			SectionHeader[i].Misc.VirtualSize : SectionHeader[i].SizeOfRawData;

		if (SectionHeader[i].VirtualAddress <= Rva &&
			Rva <= (DWORD)SectionHeader[i].VirtualAddress + SectionHeader[i].SizeOfRawData)
		{
			if (Rva >= SectionHeader[i].VirtualAddress &&
				Rva < SectionHeader[i].VirtualAddress + Size) {

				return SectionHeader[i].PointerToRawData + (Rva - SectionHeader[i].VirtualAddress);
			}
		}
	}
	return 0;
}

size_t syscallNumber(const char* ntFunc)
{
	size_t result = -1;
	size_t fHash = core::hash32::calculate(ntFunc);
	HANDLE h = CreateFileW(L"C:\\Windows\\system32\\ntdll.dll", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (h != INVALID_HANDLE_VALUE) {
		LARGE_INTEGER liFileSize;

		if (GetFileSizeEx(h, &liFileSize)) {
			HANDLE hMap = CreateFileMappingW(h, NULL, PAGE_READONLY, 0, 0, NULL);

			if (hMap != nullptr) {
#ifdef LOAD
				HANDLE lpBasePtr = LoadLibraryA("ntdll.dll");
#else
				HANDLE lpBasePtr = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
#endif
				if (lpBasePtr != nullptr) {
					PIMAGE_DOS_HEADER DosHeader = (PIMAGE_DOS_HEADER)lpBasePtr;
					PIMAGE_NT_HEADERS NtHeaders = (PIMAGE_NT_HEADERS)((PBYTE)lpBasePtr + DosHeader->e_lfanew);
					PIMAGE_DATA_DIRECTORY DataDirectory = (PIMAGE_DATA_DIRECTORY)NtHeaders->OptionalHeader.DataDirectory;
					PIMAGE_EXPORT_DIRECTORY ExportDirectory = (PIMAGE_EXPORT_DIRECTORY)RVATOVA(lpBasePtr,
						RvaToOffset(NtHeaders, DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress));

					PDWORD Names = (PDWORD)RVATOVA(lpBasePtr, RvaToOffset(NtHeaders, ExportDirectory->AddressOfNames));
					PDWORD Functions = (PDWORD)RVATOVA(lpBasePtr, RvaToOffset(NtHeaders, ExportDirectory->AddressOfFunctions));
					PWORD Ordinals = (PWORD)RVATOVA(lpBasePtr, RvaToOffset(NtHeaders, ExportDirectory->AddressOfNameOrdinals));

					for (int i = 0; i < ExportDirectory->NumberOfNames; i++) {
						char* name = (PCHAR)(RvaToOffset(NtHeaders, Names[i]) + (PBYTE)lpBasePtr);

						if (fHash == core::hash32::calculate(name)) {
							uint8_t* ptr = (uint8_t*)RVATOVA(lpBasePtr, RvaToOffset(NtHeaders, Functions[Ordinals[i]]));
							//std::cout <<  (int)ptr[4] << " ";
							result = ptr[4];
							break;
						}
					}

#ifdef LOAD
					//CloseHandle(lpBasePtr);
#else
					UnmapViewOfFile(lpBasePtr);
#endif
				}
				CloseHandle(hMap);
			}
		}
		CloseHandle(h);
	}
	return result;
}

#ifdef _DEBUG 
#include <iostream>
int main()
#else
int entry()
#endif
{
	{
		HANDLE ph = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
		if (ph != nullptr) {
			HANDLE res = (HANDLE)0x1;
			ULONG sz = 0x1000;
			//NTSTATUS code = asm_alloc(ph, &res, 0, &sz, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			NTSTATUS code = API(NTDLL, NtAllocateVirtualMemory)(ph, &res, 0, &sz, MEM_COMMIT, PAGE_READWRITE);

			uint8_t* ptr = (uint8_t*)res;

			for (int i = 0; i < sz; i++) {
				ptr[i] = 0;
			}

			if (NT_SUCCESS(code))
			{
				std::cout << "WIN";
			}
			else {
				std::cout << "error " << code << " " << GetLastError();
			}
			core::CloseHandle(ph);
		}
		return 0;
	}
	{
		HANDLE f = API(KERNEL32, CreateFileW)(L"D:\\testfile.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		size_t num = syscallNumber("NtClose");
		std::cout << num << "\n";
		if (f != INVALID_HANDLE_VALUE) {
			NTSTATUS code = asm_nt(f);

			if (NT_SUCCESS(code)) {
				std::cout << "work";
			}
			else {
				std::cout << "error " << code << "\n" << core::CloseHandle(f);
			}

		}
		return 0;
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