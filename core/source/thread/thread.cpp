#include "pch.h"
#include <core/thread/thread.h>

DWORD WINAPI ThreadProc(LPVOID lpParameter) {
	auto lambdaFunction = reinterpret_cast<core::function<void()>*>(lpParameter);
	(*lambdaFunction)();
	return 0;
}

core::thread::~thread() {
	if (!joined) join();
}

void core::thread::join() {
	if (hThread == nullptr)
		return;
	joined = true;
	API(KERNEL32, WaitForSingleObject)(hThread, INFINITE);
	core::CloseHandle(hThread);
	hThread = nullptr;
	id = 0;
	func.~unique_ptr();
}

void core::thread::swap(thread& other) noexcept {
	core::swap(hThread, other.hThread);
	core::swap(joined, other.joined);
	core::swap(id, other.id);
	core::swap(func, other.func);
}

void core::thread::call()
{
	hThread = CreateThread(NULL, 0, ThreadProc, static_cast<LPVOID>(func.get()), 0, &id);
}
