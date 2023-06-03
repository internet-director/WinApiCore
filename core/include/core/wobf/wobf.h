#pragma once

#include <core/config.h>
#include <core/hash.h>

#define API_FUNCTION_UNPACK(X) core::hash32::calculate(# X), core::function_t<X>

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
	WINHTTP
};

namespace core {
	class WOBF_EXPORT Wobf {
		struct AddressData {
			const char* name = nullptr;
			HANDLE      addr = nullptr;
			uint32_t    hash = 0;

			constexpr AddressData() = default;
			constexpr AddressData(const char* name) :
				name{ name },
				hash{ core::hash32::calculate(name) } {}
		};

	public:
		constexpr Wobf();
		~Wobf();
		template<typename B, typename O>
		static size_t rvatova(B base, O offset) noexcept {
			return size_t(base) + size_t(offset);
		}

		bool init();

		template<LibraryNumber lib, size_t hash, typename F>
		F getAddr(bool locked = true) {
			if (!isInited && !init())
				return nullptr;

			if (dllArray[lib].addr == nullptr)
				dllArray[lib].addr = (_LoadLibrary)(dllArray[lib].name);
			return static_cast<F>(GetApiAddr(dllArray[lib].addr, hash, locked));
		}


	private:
		size_t apiCounter;
		bool isInited, multiThInited;
		HANDLE mutex;
		CRITICAL_SECTION _lock;

		core::function_t<LoadLibraryA> _LoadLibrary;
		core::function_t<GetProcAddress> _GetProcAddress;

		AddressData apiArray[128];
		volatile AddressData dllArray[20] = {
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

		bool initMutlithreading();
		void lock() {
			if (multiThInited) getAddr<KERNEL32, API_FUNCTION_UNPACK(EnterCriticalSection)>(false)(&_lock);
		}
		void release() {
			if (multiThInited) getAddr<KERNEL32, API_FUNCTION_UNPACK(LeaveCriticalSection)>(false)(&_lock);
		}
		PPEB GetPEB();
		HANDLE GetDllBase(size_t libHash);
		HANDLE GetApiAddr(const HANDLE lib, size_t fHash, bool locked = true);
	} static _wobf;
}

#define API_ALWAYS(dll, func) (core::_wobf.getAddr<dll, core::hash32::calculate(# func), core::function_t<func>>())

#ifdef USE_WINDOWS_DYNAMIC_IMPORT
#define API(dll, func) API_ALWAYS(dll, func)
#else
#define API(dll, func) func
#endif