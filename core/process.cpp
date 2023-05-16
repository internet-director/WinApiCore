#include "pch.h"
#include "process.h"

namespace core {

	Process::Process()
	{
		close();
	}

	Process::Process(int pid): Process()
	{
		pe = getPEntry(FIND_BY_PID, pid);
	}

	Process::Process(const HANDLE handle) : Process()
	{
		this->handle = handle;
	}

	Process::Process(const WCHAR* processName) : Process()
	{
		pe = getPEntry(FIND_BY_NAME, processName);
	}

	Process::~Process()
	{
		close();
	}

	int Process::getPid(int pid)
	{
		return getPEntry(FIND_BY_PID, pid).th32ProcessID;
	}

	int Process::getPid(const WCHAR* processName)
	{
		return getPEntry(FIND_BY_NAME, processName).th32ProcessID;
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

	bool Process::open(int access)
	{
		if (handle != nullptr) {
			return false;
		}

		handle = OpenProcess(access, FALSE, pe.th32ProcessID);
		return handle != nullptr;
	}

	bool Process::kill()
	{
		if (handle == nullptr) {
			return false;
		}

		bool res = TerminateProcess(handle, 1);
		close();
		return res;
	}

	void Process::close()
	{
		if (handle != nullptr) CloseHandle(handle);
		handle = nullptr;
		initPEntry(pe);
	}

	void Process::initPEntry(PROCESSENTRY32W& pe)
	{
		core::memset(&pe, 0, sizeof(pe));
		pe.dwSize = sizeof(pe);
		pe.th32ProcessID = -1;
	}
}