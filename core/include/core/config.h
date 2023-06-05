#pragma once
#include <core/types.h>
/*---------------------------------dll setting---------------------------------*/

#define USE_WINDOWS_DYNAMIC_IMPORT TRUE

#if USE_WINDOWS_DYNAMIC_IMPORT == TRUE and defined(_CORE_)
#define MEM_EXPORT __declspec(dllexport)
#define THREAD_EXPORT __declspec(dllexport)
#define PROCESS_EXPORT __declspec(dllexport)
#define PROCESS_MONITOR_EXPORT __declspec(dllexport)
#define WOBF_EXPORT __declspec(dllexport)
#else
#define MEM_EXPORT
#define THREAD_EXPORT
#define PROCESS_EXPORT
#define PROCESS_MONITOR_EXPORT
#define WOBF_EXPORT
#endif