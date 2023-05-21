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

	int ProcessMonitor::getPid(const WCHAR* processName)
	{
		return getEntry<PROCESSENTRY32W>(FINDP_BY_NAME, processName).th32ProcessID;
	}

	int ProcessMonitor::getTid(const WCHAR* processName)
	{
		return getEntry<THREADENTRY32>(FINDT_BY_PARENT_PID, getPid(processName)).th32ThreadID;
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

	Process::Process(const WCHAR* processName, int pAccess, int tAccess) : Process()
	{
		
	}

	Process::Process(Process&& other) noexcept : Process()
	{
		swap(*this, other);
	}

	Process& Process::operator=(Process&& other) noexcept
	{
		if (this == &other) {
			return *this;
		}
		Process process(core::move(other));
		swap(*this, process);
		return *this;
	}

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
		pi.hProcess = API(KERNEL32, OpenProcess)(pAccess, FALSE, pId);
		pi.hThread = API(KERNEL32, OpenThread)(tAccess, FALSE, tId);

		return isOpen();
	}

	bool Process::run(const WCHAR* exetutable, WCHAR* args, int creationFlag)
	{
		bool res = static_cast<bool>(API(KERNEL32, CreateProcessW)(exetutable, args, 0, 0, 0, creationFlag, 0, 0, &si, &pi));
		if (!res) {
			clearHandle();
		}
		return res;
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
		return isOpen() ? API(KERNEL32, SuspendThread)(pi.hThread) != -1 : false;
	}

	bool Process::resume()
	{
		return  isOpen() ? API(KERNEL32, ResumeThread)(pi.hThread) != -1 : false;
	}

	bool Process::kill()
	{
		if (!isOpen()) {
			return false;
		}

		bool res = API(KERNEL32, TerminateProcess)(pi.hProcess, 1);
		close();
		return res;
	}

	void Process::close()
	{
		if (pi.hProcess != nullptr) API(KERNEL32, CloseHandle)(pi.hProcess);
		if (pi.hThread != nullptr) API(KERNEL32, CloseHandle)(pi.hThread);
		clearHandle();
	}
	void Process::clearHandle()
	{
		core::zeromem(&si, sizeof si);
		core::zeromem(&pi, sizeof pi);
		si.cb = sizeof STARTUPINFO;
		pi.dwProcessId = pi.dwThreadId = -1;
	}
	void Process::swap(Process& a, Process& b) noexcept
	{
		core::swap(a.si, b.si);
		core::swap(a.pi, b.pi);
	}
}