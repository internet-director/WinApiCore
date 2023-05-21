#pragma once

#include <core/config.h>
#include <type_traits>

constexpr int KERNEL32 = 0;
constexpr int ADVAPI32 = 1;
constexpr int USER32 = 2;
constexpr int NTDLL = 3;
constexpr int SHLWAPI = 4;
constexpr int GDI32 = 5;

namespace wobf {
    constexpr unsigned long hash_seed[4] = { 0, 0, 0, 0 };

    constexpr unsigned long constexprApiHash(const char* str, int sz = 0) {
        unsigned long hash = 0;
        unsigned int size = -1;
        if (!sz) {
            while (true) {
                if (str[++size] == 0) break;
            }
        }
        else size = sz;

        if (str && size > 0)
        {
            for (int i = 0; i < size; i++, str++)
            {
                char l = *str;
                if (l >= 'A' && l <= 'Z') l -= 'A' - 'a';
                hash = (hash << 6) + (hash << 16) - hash + l;
            }
        }
        return hash;
    }

	void Init();
	LPVOID GetFuncAddrByHash(size_t lib, uint32_t hash);
    /**/
    template<typename T>
    constexpr void expr(int lib, uint32_t hash) {
        return reinterpret_cast<T>(wobf::GetFuncAddrByHash(lib, hash));
    }
}

constexpr unsigned int hashKERNEL32 = wobf::constexprApiHash("kernel32.dll");
constexpr unsigned int hashNTDLL = wobf::constexprApiHash("ntdll.dll");
constexpr unsigned int hashLoadLibraryA = wobf::constexprApiHash("LoadLibraryA");
constexpr unsigned int hashGetProcAddress = wobf::constexprApiHash("GetProcAddress");

typedef int(WINAPI* typeMessageBoxExW)(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType, WORD wLanguageId);
typedef HANDLE(WINAPI* typeCreateToolhelp32Snapshot)(DWORD dwFlags, DWORD th32ProcessID);

#define API(dll, func) ((type ## func)wobf::GetFuncAddrByHash(dll, wobf::constexprApiHash(# func)))