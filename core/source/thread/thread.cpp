#include "pch.h"
#include <core/thread/thread.h>

DWORD WINAPI ThreadProc(LPVOID lpParameter) {
	auto* lambdaFunction = reinterpret_cast<core::function<void()>*>(lpParameter);
	(*lambdaFunction)();
	lambdaFunction->~function();
	core::free(lambdaFunction);
	return 0;
}

core::thread& core::thread::operator=(thread&& other) noexcept {
	if (this != &other) {
		join();
		hThread = other.hThread;
		id = other.id;
		other.hThread = nullptr;
		other.id = 0;
	}
	return *this;
}

core::thread::~thread() {
	join();
}

void core::thread::join() {
	if (hThread == nullptr)
		return;
	core::WaitForSingleObject(hThread, INFINITE);
	detach();
}

void core::thread::kill() {
	if (hThread == nullptr)
		return;
	core::TerminateThread(hThread, 0);
	detach();
}

void core::thread::suspend() {
	if (hThread == nullptr)
		return;
	core::SuspendThread(hThread);
}

void core::thread::resume() {
	if (hThread == nullptr)
		return;
	core::ResumeThread(hThread);
}

void core::thread::detach() noexcept {
	if (hThread == nullptr)
		return;
	core::CloseHandle(hThread);
	hThread = nullptr;
	id = 0;
}

void core::thread::swap(thread& other) noexcept {
	core::swap(hThread, other.hThread);
	core::swap(id, other.id);
}

void core::thread::call(core::function<void()>* func) {
	hThread = API(KERNEL32, CreateThread)(NULL, 0, ThreadProc, static_cast<LPVOID>(func), 0, &id);
}