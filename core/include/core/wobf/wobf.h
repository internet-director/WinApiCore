#pragma once

#include <core/config.h>
#include <core/hash.h>

constexpr size_t KERNEL32 = 0;
constexpr size_t ADVAPI32 = 1;
constexpr size_t USER32 = 2;
constexpr size_t NTDLL = 3;
constexpr size_t SHLWAPI = 4;
constexpr size_t GDI32 = 5;

namespace wobf {
    void Init();
    WOBF_EXPORT LPVOID GetFuncAddrByHash(size_t lib, uint32_t hash);
}

#define API_ALWAYS(dll, func) (static_cast<core::decay_t<decltype(func)>>(wobf::GetFuncAddrByHash(dll, core::hash32::calculate(# func))))

#ifdef USE_WINDOWS_DYNAMIC_IMPORT
#define API(dll, func) API_ALWAYS(dll, func)
#else
#define API(dll, func) func
#endif