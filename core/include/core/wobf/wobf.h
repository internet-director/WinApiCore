#pragma once

#include <core/config.h>
#include <core/types.h>
#include <core/hash.h>
#include <core/debug.h>
#include <core/StringObf.h>
#include "syscall.h"

#define API_FUNCTION_UNPACK(dll, X) core::hash32::calculate(# X), core::function_t<X>

enum LibraryNumber {
	KERNEL32,
	ADVAPI32,
	USER32,
	NTDLL,
	SHLWAPI,
	GDI32,
	IPHLPAPI,
	URLMON,
	WS2_32,
	CRYPT32,
	SHELL32,
	GDIPLUS,
	OLE32,
	PSAPI,
	CABINET,
	IMAGEHLP,
	NETAPI32,
	WTSAPI32,
	MPR,
	WINHTTP,
	LibrarySize
};

// TODO: brute value by hash in runtime
static const char* dllNames[LibraryNumber::LibrarySize] = {
	{ ("kernel32.dll") },   // KERNEL32 = 0
	{ ("advapi32.dll") },   // ADVAPI32 = 1
	{ ("user32.dll") },     // USER32 =   2
	{ ("ntdll.dll") },      // NTDLL =    3
	{ ("shlwapi.dll") },    // SHLWAPI =  4
	{ ("gdi32.dll") },      // GDI32 =    5
	{ ("iphlpapi.dll") },   // IPHLPAPI = 6
	{ ("urlmon.dll") },     // URLMON =   7
	{ ("ws2_32.dll") },     // WS2_32 =   8
	{ ("crypt32.dll") },    // CRYPT32 =  9
	{ ("shell32.dll") },    // SHELL32 =  10
	{ ("gdiplus.dll") },    // GDIPLUS =  11
	{ ("ole32.dll") },      // OLE32 =    12
	{ ("psapi.dll") },      // PSAPI =    13
	{ ("cabinet.dll") },    // CABINET =  14
	{ ("imagehlp.dll") },   // IMAGEHLP = 15
	{ ("netapi32.dll") },   // NETAPI32 = 16
	{ ("Wtsapi32.dll") },   // WTSAPI32 = 17
	{ ("Mpr.dll") },        // MPR =      18
	{ ("WinHTTP.dll") }     // WINHTTP =  19
};

namespace core {
	class WOBF_EXPORT Wobf {
		struct AddressData {
			LPVOID      addr = nullptr;
			uint32_t    hash = 0;

			constexpr AddressData() = default;
			constexpr AddressData(LPVOID addr, uint32_t hash) :
				addr{ addr }, hash{ hash } {}
		};

	public:
		Wobf();
		// since the dll can be unloaded before the end of the program, 
		// the compiler automatically uses atexit(from crt), so you have to do without statics with destructors
		//~Wobf() { close(); }
		bool init();
		bool initMutlithreading();
		bool close();

		static wtype::PTEB GetTEB();
		static wtype::PPEB GetPEB();

		template<LibraryNumber lib, uint32_t hash, typename F>
		F getAddr(bool locked = true) {
			if (!isInited && !init()) {
				debug(L"Cant init wobf!" END);
				return nullptr;
			}

			GetOrLoadDll(lib);
			return static_cast<F>(GetApiAddr(dllArray[lib].addr, hash, locked));
		}
		HANDLE GetOrLoadDll(uint32_t hash);
		HANDLE GetOrLoadDll(LibraryNumber libNumber);
		LibraryNumber FindLibraryNymberByHash(uint32_t hash) const;
		static const char* GetLibraryName(LibraryNumber lib) {
			if (lib == LibrarySize) return "";
			return dllNames[lib];
		}

	private:
		size_t apiCounter;
		bool isInited, multiThInited;
		HANDLE mutex;
		CRITICAL_SECTION _lock;

		volatile AddressData apiArray[128], dllArray[LibraryNumber::LibrarySize];
		core::function_t<LoadLibraryA> _LoadLibrary;

		void lock() {
			if (isInited && multiThInited) getAddr<KERNEL32, API_FUNCTION_UNPACK(KERNEL32, EnterCriticalSection)>(false)(&_lock);
		}
		void release() {
			if (isInited && multiThInited) getAddr<KERNEL32, API_FUNCTION_UNPACK(KERNEL32, LeaveCriticalSection)>(false)(&_lock);
		}
		HANDLE GetDllBase(size_t libHash);
		HANDLE GetApiAddr(const HANDLE lib, uint32_t fHash, bool locked = true);
	} static _wobf;
}

#define API_ALWAYS(dll, func) (core::_wobf.getAddr<dll, API_FUNCTION_UNPACK(dll, func)>())

#ifdef USE_WINDOWS_DYNAMIC_IMPORT
#define API(dll, func) API_ALWAYS(dll, func)
#else
#define API(dll, func) func
#endif

namespace core {
	class SYS_EXPORT DirectSyscall {
		struct SyscallData {
			DWORD     number = -1;
			uint32_t    hash = 0;

			constexpr SyscallData() = default;
			constexpr SyscallData(DWORD number, uint32_t hash) :
				number{ number }, hash{ hash } {}
		};
	public:
		DirectSyscall();

		bool init();
		void close();

		DWORD getSyscallNumber(uint32_t fHash);
		DWORD getSyscallNumber(const char* ntFunc) {
			return getSyscallNumber(core::hash32::calculate(ntFunc));
		}

		template<uint32_t fHash, typename F>
		F sysCaller() {
			if (!isInit && !init()) {
				return reinterpret_cast<F>(NtAlwaysError);
			}

			DWORD num = getSyscallNumber(fHash);

			if (num == DWORD(-1)) {
				return reinterpret_cast<F>(NtAlwaysError);
			}

			SetCallNumber(num);
			return reinterpret_cast<F>(SystemCall);
		}

	private:
		bool isInit;
		size_t sysCounter;
		HANDLE fileHeader;
		HANDLE fileMap;
		HANDLE fileMapPointer;
		SyscallData syscallArr[128];

		size_t RvaToOffset(PIMAGE_NT_HEADERS NtHeaders, DWORD Rva) const;
	} static _directSyscall;
}

#ifdef _WIN66

#define SYS_ALWAYS(func) (core::_directSyscall.sysCaller<core::hash32::calculate(# func), core::function_t<func>>())
#define SYS(func) SYS_ALWAYS(func)

#else

#define SYS_ALWAYS(func) API_ALWAYS(NTDLL, func)
#define SYS(func) SYS_ALWAYS(func)

#endif
