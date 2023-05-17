//
// pch.cpp
//

#include "pch.h"

bool runWaiter(STARTUPINFO& si, PROCESS_INFORMATION& pi) {
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	const WCHAR* procName = L"..\\bin\\waiter.exe";

	return CreateProcessW(procName, 0, 0, 0, 0, 0, 0, 0, &si, &pi);
}

void killWaiter(STARTUPINFO& si, PROCESS_INFORMATION& pi) {
	if (TerminateProcess(pi.hProcess, 1)) {
		if (CloseHandle(pi.hProcess)) {
			CloseHandle(pi.hThread);
		}
	}
}