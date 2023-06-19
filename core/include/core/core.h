#pragma once
 
#include <core/config.h>
#include <core/types.h>
#include <core/debug.h>
#include <core/wobf/wobf.h>
#include <core/hash.h>
#include <core/mem.h>
#include <core/process.h>
#include <core/thread.h>
#include <core/stackArray.h>

namespace core {
	bool CORE_EXPORT init();
	void CORE_EXPORT close();
}