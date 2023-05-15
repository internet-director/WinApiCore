#include "pch.h"
#include "process.h"

namespace core {

	Process::Process()
	{
		close();
	}

	Process::Process(const HANDLE handle) : Process()
	{
		this->handle = handle;
	}

	Process::Process(const WCHAR* processName) : Process()
	{
		pe = getPEntry([processName](const PROCESSENTRY32W pe) {
			return lstrcmpW(processName, pe.szExeFile) == 0;
			});
	}

	Process::~Process()
	{
		close();
	}

	int Process::getPid(int pid)
	{
		return getPEntry([pid](const PROCESSENTRY32W pe) { 
			return pe.th32ProcessID == pid; }
		).th32ProcessID;
	}

	int Process::getPid(const WCHAR* processName)
	{
		return getPEntry([processName](const PROCESSENTRY32W pe) { 
			return wcscmp(processName, pe.szExeFile) == 0; }
		).th32ProcessID;
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

		TerminateProcess(handle, 1);
		close();
		return true;
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
	}

}