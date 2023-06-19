#include <Windows.h>
#include <core/core.h>

HANDLE openProc(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId)
{
	NTSTATUS Status;
	OBJECT_ATTRIBUTES Obja;
	HANDLE Handle;
	CLIENT_ID ClientId;

	ClientId.UniqueThread = NULL;
	ClientId.UniqueProcess = LongToHandle(dwProcessId);

	InitializeObjectAttributes(
		&Obja,
		NULL,
		(bInheritHandle ? OBJ_INHERIT : 0),
		NULL,
		NULL
	);
	Status = API(NTDLL, NtOpenProcess)(
		&Handle,
		(ACCESS_MASK)dwDesiredAccess,
		&Obja,
		&ClientId
	);
	if (NT_SUCCESS(Status)) {
		return Handle;
	}
	else {
		return nullptr;
	}
}
#ifdef _DEBUG 
#include <iostream>
int main()
#else
int entry()
#endif
{
	core::Process proc;

	std::cout << size_t(GetProcessHeap()) << std::endl;
	std::cout << size_t(core::GetProcessHeap()) << std::endl;
	std::cout << size_t(core::GetProcessHeap()) - size_t(GetProcessHeap()) << std::endl;

	return 0;

	HANDLE arr = core::alloc(1000);
	core::free(arr);

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