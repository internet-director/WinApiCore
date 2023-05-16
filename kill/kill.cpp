#include <Windows.h>
#include <core/mem.h>
#include <core/debug.h>
#include <core/process.h>

#ifdef _DEBUG 
int main()
#else
int entry()
#endif
{
	core::memInit();

	int argc = 0;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	if (argc < 2) return false;

	bool res = false;
	core::Process proc(argv[1]);
	if (proc.open()) {
		res = proc.suspend();
		Sleep(5000);
		res = proc.resume();
		Sleep(5000);
		proc.kill();
	}
	if(res) {
		debug(L"done");
	}
	else {
		debug(L"error");
	}

	return 0;
}