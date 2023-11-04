#include "pch.h"
#include <core/thread/condition_variable.h>

core::condition_variable::condition_variable()
{
	InitializeConditionVariable(&cv);
}

void core::condition_variable::wait(critical_section& section) {
	API(KERNEL32, SleepConditionVariableCS)(&cv, &section, INFINITE);
}
void core::condition_variable::notify_one() {
	API(KERNEL32, WakeConditionVariable)(&cv);
}
void core::condition_variable::notify_all() {
	API(KERNEL32, WakeAllConditionVariable)(&cv);
}
