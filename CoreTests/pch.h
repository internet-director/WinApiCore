//
// pch.h
//

#pragma once

#include "gtest/gtest.h"
#include <Windows.h>
#include <core/core.h>

bool runWaiter(STARTUPINFO& si, PROCESS_INFORMATION& pi);
void killWaiter(STARTUPINFO& si, PROCESS_INFORMATION& pi);
bool goodHash(const char* dll);