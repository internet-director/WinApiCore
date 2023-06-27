#pragma once
#include <Windows.h>
#include <tlhelp32.h>
#include <LMaccess.h>
#include <LMalert.h>
#include <lmcons.h>
#include <LM.h>
#include <winternl.h>
#include <wdmguid.h>
#include <winnt.h>
#include <DbgHelp.h>
#include "ntdll_header.h"

namespace core {
	typedef signed char        int8_t;
	typedef short              int16_t;
	typedef int                int32_t;
	typedef long long          int64_t;
	typedef unsigned char      uint8_t;
	typedef unsigned short     uint16_t;
	typedef unsigned int       uint32_t;
	typedef unsigned long long uint64_t;

	constexpr size_t npos = size_t(-1);
}

typedef core::int8_t	int8_t;
typedef core::int16_t	int16_t;
typedef core::int32_t	int32_t;
typedef core::int64_t	int64_t;
typedef core::uint8_t	uint8_t;
typedef core::uint16_t	uint16_t;
typedef core::uint32_t	uint32_t;
typedef core::uint64_t	uint64_t;

constexpr size_t npos = core::npos;

#ifndef _WIN64
typedef core::uint32_t size_t;
#else
typedef core::uint64_t size_t;
#endif

// These macros must exactly match those in the Windows SDK's intsafe.h.
#define INT8_MIN         (-127i8 - 1)
#define INT16_MIN        (-32767i16 - 1)
#define INT32_MIN        (-2147483647i32 - 1)
#define INT64_MIN        (-9223372036854775807i64 - 1)
#define INT8_MAX         127i8
#define INT16_MAX        32767i16
#define INT32_MAX        2147483647i32
#define INT64_MAX        9223372036854775807i64
#define UINT8_MAX        0xffui8
#define UINT16_MAX       0xffffui16
#define UINT32_MAX       0xffffffffui32
#define UINT64_MAX       0xffffffffffffffffui64


/*-------------------------------custom macroses-------------------------------*/

#define END L"\r\n"
#define _INLINE_VAR inline
#define __countof(X) sizeof(X) / sizeof(X[0])

#define VATORAW(section, offset) ( (size_t)section->PointerToRawData + (size_t)offset - (size_t)section->VirtualAddress )
#define RVATOVA(x, y) ( (size_t)x + (size_t)y )

/*---------------------------------dll setting---------------------------------*/

#define USE_WINDOWS_DYNAMIC_IMPORT

#define KEEPS_ALL_LOGS

#if defined(USE_WINDOWS_DYNAMIC_IMPORT) and defined(_DEBUG)
#define KEEPS_WOBF_LOGS
#endif

#ifdef CORE_EXPORTS
#define CORE_EXPORT __declspec(dllexport)
#define MEM_EXPORT __declspec(dllexport)
#define NTAPI_EXPORT __declspec(dllexport)
#define STACK_EXPORT __declspec(dllexport)
#define THREAD_EXPORT __declspec(dllexport)
#define PROCESS_EXPORT __declspec(dllexport)
#define PROCESS_MONITOR_EXPORT __declspec(dllexport)
#define WOBF_EXPORT __declspec(dllexport)
#define SYS_EXPORT __declspec(dllexport)
#else
#define CORE_EXPORT
#define MEM_EXPORT
#define STACK_EXPORT
#define NTAPI_EXPORT
#define THREAD_EXPORT
#define PROCESS_EXPORT
#define PROCESS_MONITOR_EXPORT
#define WOBF_EXPORT
#define SYS_EXPORT
#endif