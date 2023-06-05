﻿// pch.h: это предварительно скомпилированный заголовочный файл.
// Перечисленные ниже файлы компилируются только один раз, что ускоряет последующие сборки.
// Это также влияет на работу IntelliSense, включая многие функции просмотра и завершения кода.
// Однако изменение любого из приведенных здесь файлов между операциями сборки приведет к повторной компиляции всех(!) этих файлов.
// Не добавляйте сюда файлы, которые планируете часто изменять, так как в этом случае выигрыша в производительности не будет.

#ifndef PCH_H
#define PCH_H

// Добавьте сюда заголовочные файлы для предварительной компиляции
#include "framework.h"
#include "include/core/types.h"
#include "include/core/config.h"
#include "include/core/mem.h"
#include "include/core/hash.h"
#include "include/core/wobf/wobf.h"
#include "include/core/process.h"
#include "include/core/thread.h"
#include "include/core/debug.h"

#endif //PCH_H
