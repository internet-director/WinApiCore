#include "pch.h"
#include <core/wobf/wobf.h>

#define RVATOVA(base, offset) ((SIZE_T)base + (SIZE_T)offset)
size_t api_counter = 0;
bool inited = false;

constexpr const uint32_t hashLoadLibraryA = core::hash32::calculate("LoadLibraryA");
constexpr const uint32_t hashGetProcAddress = core::hash32::calculate("GetProcAddress");

using typeGetProcAddress = core::decay_t<decltype(GetProcAddress)>;
using typeLoadLibraryA = core::decay_t<decltype(LoadLibraryA)>;
typeGetProcAddress _GetProcAddress;
typeLoadLibraryA _LoadLibrary;

struct AddressData {
    const char* name = nullptr;
    LPVOID      addr = nullptr;
    uint32_t    hash = 0;

    constexpr AddressData() = default;
    constexpr AddressData(const char* name) : name(name) {
        hash = core::hash32::calculate(name);
    }
};

AddressData apiArray[128];

AddressData dllArray[] = {
    { ("kernel32.dll") },   //KERNEL32 = 0
    { ("advapi32.dll") },   //ADVAPI32 = 1
    { ("user32.dll") },     //USER32 = 2
    { ("ntdll.dll") },      //NTDLL = 3
    { ("shlwapi.dll") },    //SHLWAPI = 4
    { ("gdi32.dll") },      //GDI32 = 5
    { ("iphlpapi.dll") },   //IPHLPAPI = 6
    { ("urlmon.dll") },     //URLMON = 7
    { ("ws2_32.dll") },     //WS2_32 = 8
    { ("crypt32.dll") },    //CRYPT32 = 9
    { ("shell32.dll") },    //SHELL32 = 10
    { ("gdiplus.dll") },    //GDIPLUS = 11
    { ("ole32.dll") },      //OLE32 = 12
    { ("psapi.dll") },      //PSAPI = 13
    { ("cabinet.dll") },    //CABINET = 14;
    { ("imagehlp.dll") },   //IMAGEHLP = 15
    { ("netapi32.dll") },   //NETAPI32 = 16
    { ("Wtsapi32.dll") },   //WTSAPI32 = 17
    { ("Mpr.dll") },        //MPR = 18
    { ("WinHTTP.dll") }     //WINHTTP = 19
};

void Wide2Char(const WCHAR* data, char* out, UINT len)
{
    for (int i = 0; i < len; i++) {
        if (!data[i]) {
            out[i] = 0;
            break;
        }
        // TODO
        out[i] = data[i];
    }
}

PPEB GetPEB()
{
#ifdef _WIN64
    return  (PPEB)__readgsqword(0x60);
#else
    return  (PPEB)__readfsdword(0x30);
#endif
}

HMODULE GetDllBase(UINT dllHash)
{
    PPEB peb;
    PLDR_DATA_TABLE_ENTRY module_ptr, first_mod;

    peb = GetPEB();

    module_ptr = (PLDR_DATA_TABLE_ENTRY)peb->Ldr->InMemoryOrderModuleList.Flink;
    first_mod = module_ptr;

    char dll_name[64];

    do
    {
        Wide2Char(module_ptr->FullDllName.Buffer, dll_name, module_ptr->FullDllName.Length / 2);
        dll_name[module_ptr->FullDllName.Length / 2] = NULL;
        if (core::hash32::calculate(dll_name) == dllHash) return (HMODULE)module_ptr->Reserved2[0];
        else module_ptr = (PLDR_DATA_TABLE_ENTRY)module_ptr->Reserved1[0];

    } while (module_ptr && module_ptr != first_mod);

    return nullptr;
}

HANDLE GetApiAddr(HANDLE lib, size_t fHash)
{
    if (api_counter == __countof(apiArray)) return nullptr;

    for (size_t i = 0; i < api_counter; i++) {
        if (apiArray[i].hash == fHash) {
            return apiArray[i].addr;
        }
    }
    apiArray[api_counter].hash = fHash;

    if (lib == nullptr) return nullptr;

    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)lib;
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)RVATOVA(lib, dos->e_lfanew);
    IMAGE_FILE_HEADER f = nt->FileHeader;
    IMAGE_OPTIONAL_HEADER opt = nt->OptionalHeader;
    PIMAGE_EXPORT_DIRECTORY data = (PIMAGE_EXPORT_DIRECTORY)RVATOVA(lib, opt.DataDirectory[0].VirtualAddress);
    PDWORD name = (PDWORD)RVATOVA(lib, data->AddressOfNames);
    PDWORD functions = (PDWORD)RVATOVA(lib, data->AddressOfFunctions);
    PWORD ordAddress= (PWORD)RVATOVA(lib, data->AddressOfNameOrdinals);

    char* n = nullptr;
    for (int i = 0; i < data->NumberOfNames; i++) {
        n = (char*)RVATOVA(lib, name[i]);
        if (fHash == core::hash32::calculate(n)) {
            DWORD functionRVA = functions[ordAddress[i]];
            HANDLE functionAddr = (HANDLE)RVATOVA(lib, functionRVA);
            api_counter++;
            if(!inited) {
                apiArray[api_counter - 1].addr = functionAddr;
            }
            else {
                apiArray[api_counter - 1].addr = _GetProcAddress(HMODULE(lib), n);
            }
            return apiArray[api_counter - 1].addr;
        }
    }
    return nullptr;
}

void Init()
{
    dllArray[NTDLL].addr = GetDllBase(dllArray[NTDLL].hash);
    dllArray[KERNEL32].addr = GetDllBase(dllArray[KERNEL32].hash);
   
    _LoadLibrary = (typeLoadLibraryA)(GetApiAddr(dllArray[KERNEL32].addr, hashLoadLibraryA));
    _GetProcAddress = (typeGetProcAddress)(GetApiAddr(dllArray[KERNEL32].addr, hashGetProcAddress));
}

LPVOID core::wobf::GetFuncAddrByHash(size_t lib, uint32_t hash)
{
    if (!inited) {
        Init();
        inited = true;
    }
    if (dllArray[lib].addr == nullptr)
        dllArray[lib].addr = (_LoadLibrary)(dllArray[lib].name);
    return GetApiAddr(dllArray[lib].addr, hash);
}