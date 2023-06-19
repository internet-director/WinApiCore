#include <Windows.h>
#include <core/core.h>

#ifdef _DEBUG 
#include <iostream>
int main()
#else
int entry()
#endif
{
	core::Process proc;
	WCHAR arg[] = L"notepad";
	if (proc.run(NULL, arg, NULL)) {
		proc.wait(1000);
		proc.kill();
	}

	return 0;
	/*
	int argc = 0;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	if (argc < 2) return false;

	bool res = false;
	core::Process proc(core::ProcessMonitor::getPid(argv[1]), PROCESS_TERMINATE);
	if (proc.isOpen()) {
		proc.kill();
		res = true;
	}
	if(res) {
		debug(L"done");
	}
	else {
		debug(L"error");
	}

	return 0;*/
}