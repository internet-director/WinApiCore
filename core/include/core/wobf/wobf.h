#pragma once

#include <core/config.h>
#include <core/hash.h>

constexpr size_t KERNEL32 = 0;
constexpr size_t ADVAPI32 = 1;
constexpr size_t USER32 = 2;
constexpr size_t NTDLL = 3;
constexpr size_t SHLWAPI = 4;
constexpr size_t GDI32 = 5;
constexpr size_t IPHLPAPI = 6;
constexpr size_t URLMON = 7;
constexpr size_t WS2_32 = 8;
constexpr size_t CRYPT32 = 9;
constexpr size_t SHELL32 = 10;
constexpr size_t GDIPLUS = 11;
constexpr size_t OLE32 = 12;
constexpr size_t PSAPI = 13;
constexpr size_t CABINET = 14;
constexpr size_t IMAGEHLP = 15;
constexpr size_t NETAPI32 = 16;
constexpr size_t WTSAPI32 = 17;
constexpr size_t MPR = 18;
constexpr size_t WINHTTP = 19;

namespace core {
    namespace wobf {
        WOBF_EXPORT HANDLE GetFuncAddrByHash(size_t lib, uint32_t hash);
    }
}

#define API_ALWAYS(dll, func) (static_cast<core::function_t<func>>(core::wobf::GetFuncAddrByHash(dll, core::hash32::calculate(# func))))

#ifdef USE_WINDOWS_DYNAMIC_IMPORT
#define API(dll, func) API_ALWAYS(dll, func)
#else
#define API(dll, func) func
#endif