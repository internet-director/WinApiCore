#include "pch.h"
#include "process.h"

namespace core {

	Process::Process()
	{
		close();
	}

	Process::Process(int pid): Process()
	{
		pe = getEntry<PROCESSENTRY32W>(FINDP_BY_PID, pid);
		te = getEntry<THREADENTRY32>(FINDT_BY_PARENT_PID, pid);
	}

	Process::Process(const HANDLE handle) : Process()
	{
		this->pHandle = handle;
	}

	Process::Process(const WCHAR* processName) : Process()
	{
		pe = getEntry<PROCESSENTRY32W>(FINDP_BY_NAME, processName);
		te = getEntry<THREADENTRY32>(FINDT_BY_PARENT_PID, pe.th32ProcessID);
	}

	Process::~Process()
	{
		close();
	}

	int Process::getPid(const WCHAR* processName)
	{
		return getEntry<PROCESSENTRY32W>(FINDP_BY_NAME, processName).th32ProcessID;
	}

	int Process::getPid() const
	{
		return pe.th32ProcessID;
	}

	const WCHAR* Process::getName() const
	{
		if (pe.dwSize == 0) {
			return nullptr;
		}
		return pe.szExeFile;
	}

	bool Process::open(int pAccess, int tAccess)
	{
		if (tAccess == -1) tAccess = pAccess;

		if (pHandle == nullptr) {
			pHandle = OpenProcess(pAccess, FALSE, pe.th32ProcessID);
		}
		if (tHandle == nullptr) {
			tHandle = OpenThread(tAccess, FALSE, te.th32ThreadID);
		}

		return pHandle != nullptr && 
			tHandle != nullptr;
	}

	bool Process::suspend()
	{
		if (pHandle == nullptr) {
			return false;
		}

		return SuspendThread(tHandle) != -1;
	}

	bool Process::resume()
	{
		if (pHandle == nullptr) {
			return false;
		}

		return ResumeThread(tHandle) != -1;
	}

	bool Process::kill()
	{
		if (pHandle == nullptr) {
			return false;
		}

		bool res = TerminateProcess(pHandle, 1);
		close();
		return res;
	}

	void Process::close()
	{
		if (pHandle != nullptr) CloseHandle(pHandle);
		if (tHandle != nullptr) CloseHandle(tHandle);
		pHandle = tHandle = nullptr;
		initEntry(pe);
		initEntry(te);
	}
}