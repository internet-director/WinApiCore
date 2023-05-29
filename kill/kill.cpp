#include <Windows.h>
#include <core/core.h>

#ifdef _DEBUG 
int main()
#else
int entry()
#endif
{
	core::Process proc;
	WCHAR arg[] = L"notepad";
	if (proc.run(NULL, arg, CREATE_SUSPENDED)) {
		bool res = proc.hollowing(L"C:\\Users\\internet_director\\prog\\other\\console\\bin_x64\\messager.exe");
		if (res) {
			debug(L"”–¿¿¿¿¿¿!!!");
		}
		else {
			debug(L"SHIIIIIIT");
		}
		proc.wait();
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