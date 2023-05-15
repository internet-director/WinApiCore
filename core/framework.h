#pragma once

#define WIN32_LEAN_AND_MEAN             // Исключите редко используемые компоненты из заголовков Windows
// Файлы заголовков Windows
#include <Windows.h>
#include <tlhelp32.h>
#include "config.h"
#include "mem.h"
#include "process.h"

#ifdef _DEBUG
#include <iostream>
#endif
