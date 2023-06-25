#include "pch.h"
#include <core/wobf/wobf.h>

namespace core {
	Wobf::Wobf() :
		_LoadLibrary{ nullptr },
		isInited{ false },
		multiThInited{ false },
		apiCounter{ 0 },
		mutex{ INVALID_HANDLE_VALUE }
	{
		core::zeromem(&_lock, sizeof _lock);
		core::zeromem(apiArray, sizeof apiArray);
		core::zeromem(dllArray, sizeof dllArray);
	}

	bool Wobf::init() {
		if (!isInited) {
			dllArray[NTDLL].addr = GetDllBase(core::hash32::calculate(dllNames[NTDLL]));
			dllArray[KERNEL32].addr = GetDllBase(core::hash32::calculate(dllNames[KERNEL32]));

			_LoadLibrary = static_cast<core::function_t<LoadLibraryA>>(GetApiAddr(dllArray[KERNEL32].addr,
				core::hash32::calculate("LoadLibraryA")));
			isInited = _LoadLibrary != nullptr;
		}
		return isInited;
	}
	bool Wobf::initMutlithreading() {
		if (!isInited) return false;
		if ((mutex = getAddr<KERNEL32, API_FUNCTION_UNPACK(KERNEL32, CreateMutexW)>(false)(NULL, FALSE, L"wobf"))
			== INVALID_HANDLE_VALUE) {
			return false;
		}

		getAddr<KERNEL32, API_FUNCTION_UNPACK(KERNEL32, InitializeCriticalSection)>(false)(&_lock);
		getAddr<KERNEL32, API_FUNCTION_UNPACK(KERNEL32, EnterCriticalSection)>(false);
		getAddr<KERNEL32, API_FUNCTION_UNPACK(KERNEL32, LeaveCriticalSection)>(false);
		return multiThInited = true;
	}
	bool Wobf::close() {
		bool res = true;
		if (multiThInited) {
			if (reinterpret_cast<HANDLE>(getAddr<KERNEL32, API_FUNCTION_UNPACK(KERNEL32, ReleaseMutex)>(false)(mutex)) == nullptr) res = false;
			if (reinterpret_cast<HANDLE>(getAddr<KERNEL32, API_FUNCTION_UNPACK(KERNEL32, CloseHandle)>(false)(mutex)) == nullptr) res = false;
		}

		if (isInited) {
			for (size_t i = 0; i < LibrarySize; i++) {
				if (i == KERNEL32 || i == NTDLL) continue;
				if (dllArray[i].addr == nullptr) continue;
				API(KERNEL32, FreeLibrary)(static_cast<HMODULE>(dllArray[i].addr));
			}
		}
		return res;
	}
	PPEB Wobf::GetPEB()
	{
#ifdef _WIN64
		return  (PPEB)__readgsqword(0x60);
#else
		return  (PPEB)__readfsdword(0x30);
#endif
	}
	HANDLE Wobf::GetDllBase(size_t libHash) {
		PPEB peb = GetPEB();
		PLDR_DATA_TABLE_ENTRY module_ptr, first_mod;

		module_ptr = (PLDR_DATA_TABLE_ENTRY)peb->Ldr->InMemoryOrderModuleList.Flink;
		first_mod = module_ptr;

		char dll_name[64];

		do
		{
			core::Wide2Char(module_ptr->FullDllName.Buffer, dll_name, module_ptr->FullDllName.Length / 2);
			dll_name[module_ptr->FullDllName.Length / 2] = NULL;
			if (core::hash32::calculate(dll_name) == libHash) return module_ptr->Reserved2[0];
			else module_ptr = (PLDR_DATA_TABLE_ENTRY)module_ptr->Reserved1[0];

		} while (module_ptr != nullptr && module_ptr != first_mod);

		return nullptr;
	}
	HANDLE Wobf::GetOrLoadDll(size_t hash)
	{
		LibraryNumber lib = FindLibraryNymberByHash(hash);
		if (lib == LibrarySize) return nullptr;
		return GetOrLoadDll(lib);
	}
	HANDLE Wobf::GetOrLoadDll(LibraryNumber libNumber)
	{
		if (dllArray[libNumber].addr != nullptr) return dllArray[libNumber].addr;
		if (_LoadLibrary == nullptr) return nullptr;
		dllArray[libNumber].addr = _LoadLibrary(dllNames[libNumber]);
		dllArray[libNumber].hash = core::hash32::calculate(dllNames[libNumber]);
		return dllArray[libNumber].addr;
	}
	LibraryNumber Wobf::FindLibraryNymberByHash(size_t hash) const
	{
		for (size_t i = 0; i < LibrarySize; i++) {
			if (dllArray[i].hash == hash) return LibraryNumber(i);
			if (core::hash32::calculate(dllNames[i]) == hash) return LibraryNumber(i);
		}
		return LibraryNumber::LibrarySize;
	}
	HANDLE Wobf::GetApiAddr(const HANDLE lib, size_t fHash, bool locked)
	{
		if (locked) lock();
		if (apiCounter == __countof(apiArray)) return nullptr;

		for (size_t i = 0; i < apiCounter; i++) {
			if (apiArray[i].hash == fHash) {
				if (locked) release();
				return apiArray[i].addr;
			}
		}
		if (locked) release();
		if (lib == nullptr) return nullptr;

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

					if (apiCounter == __countof(apiArray) - 1) {
						return functionAddr;
					}
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
		}
		return nullptr;
	}



	DirectSyscall::DirectSyscall() :
		isInit{ false },
		sysCounter{ 0 },
		fileHeader{ INVALID_HANDLE_VALUE },
		fileMap{ nullptr },
		fileMapPointer{ nullptr }
	{
		core::zeromem(syscallArr, sizeof syscallArr);
		if (!init()) close();
	}

	bool DirectSyscall::init()
	{
		this->fileHeader = API(KERNEL32, CreateFileW)(L"C:\\Windows\\system32\\ntdll.dll", GENERIC_READ,
			FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (this->fileHeader != INVALID_HANDLE_VALUE) {
			this->fileMap = API(KERNEL32, CreateFileMappingW)(fileHeader, NULL, PAGE_READONLY, 0, 0, NULL);

			if (this->fileMap != nullptr) {
				this->fileMapPointer = API(KERNEL32, MapViewOfFile)(fileMap, FILE_MAP_READ, 0, 0, 0);

				if (this->fileMapPointer != nullptr) return true;
			}
		}
		close();
		return false;
	}

	void DirectSyscall::close()
	{
		if (this->fileMapPointer != nullptr) API(KERNEL32, UnmapViewOfFile)(fileMapPointer);
		if (this->fileMap != nullptr) core::CloseHandle(fileMap);
		if (this->fileHeader != nullptr) core::CloseHandle(fileHeader);
	}

	DWORD DirectSyscall::getSyscallNumber(uint32_t fHash)
	{
		DWORD result = -1;

		for (size_t i = 0; i < __countof(syscallArr); i++) {
			if (syscallArr[i].hash == fHash) {
				return syscallArr[i].number;
			}
		}

		if (fileMapPointer == nullptr) return result;

		PIMAGE_DOS_HEADER DosHeader = (PIMAGE_DOS_HEADER)fileMapPointer;
		PIMAGE_NT_HEADERS NtHeaders = (PIMAGE_NT_HEADERS)((PBYTE)fileMapPointer + DosHeader->e_lfanew);
		PIMAGE_DATA_DIRECTORY DataDirectory = (PIMAGE_DATA_DIRECTORY)NtHeaders->OptionalHeader.DataDirectory;
		PIMAGE_EXPORT_DIRECTORY ExportDirectory = (PIMAGE_EXPORT_DIRECTORY)RVATOVA(fileMapPointer,
			RvaToOffset(NtHeaders, DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress));

		PDWORD Names = (PDWORD)RVATOVA(fileMapPointer, RvaToOffset(NtHeaders, ExportDirectory->AddressOfNames));
		PDWORD Functions = (PDWORD)RVATOVA(fileMapPointer, RvaToOffset(NtHeaders, ExportDirectory->AddressOfFunctions));
		PWORD Ordinals = (PWORD)RVATOVA(fileMapPointer, RvaToOffset(NtHeaders, ExportDirectory->AddressOfNameOrdinals));

		for (int i = 0; i < ExportDirectory->NumberOfNames; i++) {
			char* name = (PCHAR)(RvaToOffset(NtHeaders, Names[i]) + (PBYTE)fileMapPointer);

			if (fHash == core::hash32::calculate(name)) {
				uint8_t* ptr = (uint8_t*)RVATOVA(fileMapPointer, RvaToOffset(NtHeaders, Functions[Ordinals[i]]));
				DWORD syscallNumber = ((DWORD*)(ptr + 4))[0];

				if (sysCounter == __countof(syscallArr) - 1) {
					return syscallNumber;
				}

				syscallArr[sysCounter] = SyscallData(syscallNumber, fHash);
				return syscallArr[sysCounter++].number;
			}
		}
		return (DWORD)-1;
	}

	size_t DirectSyscall::RvaToOffset(PIMAGE_NT_HEADERS NtHeaders, DWORD Rva) const {
		if (Rva == 0 || NtHeaders == nullptr) return 0;

		PIMAGE_SECTION_HEADER SectionHeader = IMAGE_FIRST_SECTION(NtHeaders);

		for (size_t i = 0; i < NtHeaders->FileHeader.NumberOfSections; i++) {
			size_t Size = SectionHeader[i].Misc.VirtualSize ?
				SectionHeader[i].Misc.VirtualSize : SectionHeader[i].SizeOfRawData;

			if (SectionHeader[i].VirtualAddress <= Rva &&
				Rva <= (DWORD)SectionHeader[i].VirtualAddress + SectionHeader[i].SizeOfRawData)
			{
				if (Rva >= SectionHeader[i].VirtualAddress && Rva < SectionHeader[i].VirtualAddress + Size) {

					return SectionHeader[i].PointerToRawData + (Rva - SectionHeader[i].VirtualAddress);
				}
			}
		}
		return 0;
	}
}