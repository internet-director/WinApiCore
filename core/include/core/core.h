#pragma once
 
#include <core/config.h>
#include <core/types.h>
#include <core/debug.h>
#include <core/wobf/wobf.h>
#include <core/hash.h>
#include <core/mem.h>
#include <core/process.h>
#include <core/thread/thread.h>
#include <core/thread/mutex.h>
#include <core/thread/critical_section.h>
#include <core/thread/condition_variable.h>
#include <core/stackArray.h>

namespace core {
	bool CORE_EXPORT init();
	int CORE_EXPORT close();
}