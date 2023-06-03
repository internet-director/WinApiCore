#include "pch.h"
#include <core/wobf/wobf.h>

namespace core {
	constexpr Wobf::Wobf() :
		_LoadLibrary{ nullptr },
		_GetProcAddress{ nullptr },
		isInited{ false },
		multiThInited{ false },
		apiCounter{ 0 },
		mutex{ INVALID_HANDLE_VALUE }
	{
	}
	Wobf::~Wobf() {
		if (multiThInited) {
			getAddr<KERNEL32, API_FUNCTION_UNPACK(ReleaseMutex)>()(mutex);
			getAddr<KERNEL32, API_FUNCTION_UNPACK(CloseHandle)>()(mutex);
		}
	}
	bool Wobf::init() {
		if (!isInited) {
			dllArray[NTDLL].addr = GetDllBase(dllArray[NTDLL].hash);
			dllArray[KERNEL32].addr = GetDllBase(dllArray[KERNEL32].hash);

			_LoadLibrary = static_cast<core::function_t<LoadLibraryA>>(GetApiAddr(dllArray[KERNEL32].addr,
				core::hash32::calculate("LoadLibraryA")));
			_GetProcAddress = static_cast<core::function_t<GetProcAddress>>(GetApiAddr(dllArray[KERNEL32].addr,
				core::hash32::calculate("GetProcAddress")));
			isInited = _LoadLibrary != nullptr && _GetProcAddress != nullptr;
		}
		if (!multiThInited) multiThInited = initMutlithreading();
		return isInited && multiThInited;
	}
	bool Wobf::initMutlithreading() {
		if (!isInited) return false;
		if ((mutex = getAddr<KERNEL32, API_FUNCTION_UNPACK(CreateMutexW)>()(NULL, FALSE, L"wobf"))
			== INVALID_HANDLE_VALUE) return false;

		getAddr<KERNEL32, API_FUNCTION_UNPACK(InitializeCriticalSection)>()(&_lock);
		getAddr<KERNEL32, API_FUNCTION_UNPACK(EnterCriticalSection)>(false);
		getAddr<KERNEL32, API_FUNCTION_UNPACK(LeaveCriticalSection)>(false);
		return multiThInited = true;
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
	HANDLE Wobf::GetApiAddr(const HANDLE lib, size_t fHash, bool locked)
	{
		if(locked) lock();
		if (apiCounter == __countof(apiArray)) return nullptr;

		for (size_t i = 0; i < apiCounter; i++) {
			if (apiArray[i].hash == fHash) {
				return apiArray[i].addr;
			}
		}
		if (locked) release();
		if (lib == nullptr) return nullptr;

		PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)lib;
		PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)rvatova(lib, dos->e_lfanew);
		IMAGE_FILE_HEADER f = nt->FileHeader;
		IMAGE_OPTIONAL_HEADER opt = nt->OptionalHeader;
		PIMAGE_EXPORT_DIRECTORY data = (PIMAGE_EXPORT_DIRECTORY)rvatova(lib, opt.DataDirectory[0].VirtualAddress);
		PDWORD name = (PDWORD)rvatova(lib, data->AddressOfNames);
		PDWORD functions = (PDWORD)rvatova(lib, data->AddressOfFunctions);
		PWORD ordAddress = (PWORD)rvatova(lib, data->AddressOfNameOrdinals);

		char* n = nullptr;
		for (int i = 0; i < data->NumberOfNames; i++) {
			n = (char*)rvatova(lib, name[i]);
			if (fHash == core::hash32::calculate(n)) {
				if (locked) lock();
				DWORD functionRVA = functions[ordAddress[i]];
				HANDLE functionAddr = (HANDLE)rvatova(lib, functionRVA);

				if (!isInited) {
					apiArray[apiCounter].addr = functionAddr;
				}
				else {
					apiArray[apiCounter].addr = _GetProcAddress(HMODULE(lib), n);
				}
				apiArray[apiCounter].hash = fHash;
				HANDLE result = apiArray[apiCounter++].addr;
				if (locked) release();
				return result;
			}
		}
		return nullptr;
	}
}