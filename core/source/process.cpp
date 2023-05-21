#include "pch.h"
#include "process.h"

namespace core {
	ProcessMonitor::ProcessMonitor() noexcept
	{
		clear();
	}
	ProcessMonitor::ProcessMonitor(int pid): ProcessMonitor()
	{
		pe = getEntry<PROCESSENTRY32W>(FINDP_BY_PID, pid);
		te = getEntry<THREADENTRY32>(FINDT_BY_PARENT_PID, pid);
	}
	ProcessMonitor::ProcessMonitor(const WCHAR* processName): ProcessMonitor()
	{
		pe = getEntry<PROCESSENTRY32W>(FINDP_BY_NAME, processName);
		te = getEntry<THREADENTRY32>(FINDT_BY_PARENT_PID, pe.th32ProcessID);
	}
	int ProcessMonitor::getPid()
	{
		return pe.th32ProcessID;
	}
	int ProcessMonitor::getTid()
	{
		return te.th32ThreadID;
	}
	int ProcessMonitor::getPid(const WCHAR* processName)
	{
		return getEntry<PROCESSENTRY32W>(FINDP_BY_NAME, processName).th32ProcessID;
	}
	int ProcessMonitor::getTid(const WCHAR* processName)
	{
		return -1;
	}
	const WCHAR* ProcessMonitor::getName() const
	{
		if (pe.th32ProcessID == -1) {
			return nullptr;
		}
		return pe.szExeFile;
	}
	const WCHAR* ProcessMonitor::getName(const WCHAR* processName)
	{
		return getEntry<PROCESSENTRY32W>(FINDP_BY_NAME, processName).szExeFile;
	}
	void ProcessMonitor::clear() noexcept
	{
		initPEntry(pe);
		initTEntry(te);
	}


	Process::Process()
	{	
		clearHandle();
	}

	Process::Process(int pid, int pAccess, int tAccess): Process()
	{
		ProcessMonitor monitor(pid);
		if (!monitor.isExist()) {
			return;
		}
		if (pAccess == -1) pAccess = tAccess = PROCESS_ALL_ACCESS;
		if (tAccess == -1) tAccess = pAccess;

		open(monitor.getPid(), monitor.getTid(), pAccess, tAccess);
	}

	/*Process::Process(const HANDLE handle) : Process()
	{
		this->pHandle = handle;
	}*/

	Process::Process(const WCHAR* processName, int pAccess, int tAccess) : Process()
	{
		
	}

	/*Process::Process(Process&& other) noexcept : Process()
	{

	}*/

	Process::~Process()
	{
		clearHandle();
	}

	bool Process::open(int pId, int tId, int pAccess, int tAccess)
	{
		if (ProcessMonitor::getEntry<THREADENTRY32>(ProcessMonitor::FINDT_BY_PID, tId).th32ThreadID == -1 ||
			ProcessMonitor::getEntry<PROCESSENTRY32W>(ProcessMonitor::FINDP_BY_PID, pId).th32ProcessID == -1) return false;

		if (tAccess == -1) tAccess = pAccess;

		clearHandle();

		pi.dwProcessId = pId;
		pi.dwThreadId = tId;
		pi.hProcess = OpenProcess(pAccess, FALSE, pId);
		pi.hThread = OpenThread(tAccess, FALSE, tId);

		return isOpen();
	}

	bool Process::run(const WCHAR* exetutable, int creationFlag)
	{
		return CreateProcessW(exetutable, 0, 0, 0, 0, creationFlag, 0, 0, &si, &pi);
	}

	bool Process::hollowing(const WCHAR* name)
	{
		if (isOpen()) {
			return false;
		}

		return true;
	}

	bool Process::suspend()
	{
		if (!isOpen()) {
			return false;
		}

		return SuspendThread(pi.hThread) != -1;
	}

	bool Process::resume()
	{
		if (!isOpen()) {
			return false;
		}

		return ResumeThread(pi.hThread) != -1;
	}

	bool Process::kill()
	{
		if (!isOpen()) {
			return false;
		}

		bool res = TerminateProcess(pi.hProcess, 1);
		close();
		return res;
	}

	void Process::close()
	{
		if (pi.hProcess != nullptr) CloseHandle(pi.hProcess);
		if (pi.hThread != nullptr) CloseHandle(pi.hThread);
		clearHandle();
	}
	void Process::clearHandle()
	{
		core::zeromem(&si, sizeof si);
		core::zeromem(&pi, sizeof pi);
		si.cb = sizeof STARTUPINFO;
		pi.dwProcessId = pi.dwThreadId = -1;
	}
}