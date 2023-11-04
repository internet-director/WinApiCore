#pragma once

#define WIN32_LEAN_AND_MEAN             // Исключите редко используемые компоненты из заголовков Windows
// Файлы заголовков Windows
#include "config.h"
#include "include/core/wobf/wobf.h"
#include "hash.h"
#include "mem.h"
#include "include/core/thread/thread.h"
#include "include/core/thread/mutex.h"
#include "include/core/thread/critical_section.h"
#include "include/core/thread/condition_variable.h"
#include "process.h"
#include "debug.h"
#include "stackArray.h"
#include "Vector.h"
#include "function.h"

/*
#ifdef _DEBUG
#include <iostream>
#endif
*/