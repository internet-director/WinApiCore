//
// pch.h
//

#pragma once

#include "gtest/gtest.h"
#include <Windows.h>
#include <core/config.h>
#include <core/mem.h>
#include <core/process.h>
#include <core/wobf/wobf.h>

bool runWaiter(STARTUPINFO& si, PROCESS_INFORMATION& pi);
void killWaiter(STARTUPINFO& si, PROCESS_INFORMATION& pi);
bool goodHash(const char* dll);