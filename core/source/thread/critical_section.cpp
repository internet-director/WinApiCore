#include "pch.h"
#include <core/wobf/wobf.h>
#include <core/thread/critical_section.h>

core::critical_section::critical_section()
{
	API(KERNEL32, InitializeCriticalSection)(&section);
}

core::critical_section::~critical_section() {
	API(KERNEL32, DeleteCriticalSection)(&section);
}

void core::critical_section::enter()
{
	API(KERNEL32, EnterCriticalSection)(&section);
}

void core::critical_section::leave()
{
	API(KERNEL32, LeaveCriticalSection)(&section);
}

PCRITICAL_SECTION core::critical_section::operator*() noexcept
{
	return &section;
}
