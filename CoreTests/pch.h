//
// pch.h
//

#pragma once

#include "gtest/gtest.h"
#include "../core/process.h"

bool runWaiter(STARTUPINFO& si, PROCESS_INFORMATION& pi);
void killWaiter(STARTUPINFO& si, PROCESS_INFORMATION& pi);