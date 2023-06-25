#include <Windows.h>
#include <core/core.h>


#ifdef _DEBUG 
int main()
#else
int entry()
#endif
{

	int argc = 0;
	LPWSTR* argv = API(KERNEL32, CommandLineToArgvW)(API(KERNEL32, GetCommandLineW)(), &argc);

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

	return 0;
}