#pragma once
#include <core/config.h>
#include <core/types.h>
#include <core/wobf/wobf.h>
#include <core/mem.h>

namespace core {
	typedef struct BASE_RELOCATION_BLOCK {
		DWORD PageAddress;
		DWORD BlockSize;
	} BASE_RELOCATION_BLOCK, * PBASE_RELOCATION_BLOCK;

	typedef struct BASE_RELOCATION_ENTRY {
		USHORT Offset : 12;
		USHORT Type : 4;
	} BASE_RELOCATION_ENTRY, * PBASE_RELOCATION_ENTRY;

#define CountRelocationEntries(dwBlockSize)		\
	(dwBlockSize -								\
	sizeof(BASE_RELOCATION_BLOCK)) /			\
	sizeof(BASE_RELOCATION_ENTRY)


	class PROCESS_MONITOR_EXPORT ProcessMonitor {
		THREADENTRY32 te;
		PROCESSENTRY32W pe;
	public:
		/*
		* search thread by process pid
		*/
		constexpr static auto FINDT_BY_PARENT_PID = [](const THREADENTRY32& te, int pid) {
			return te.th32OwnerProcessID == pid;
		};
		/*
		* search thread by thread pid
		*/
		constexpr static auto FINDT_BY_PID = [](const THREADENTRY32& te, int pid) {
			return te.th32ThreadID == pid;
		};
		/*
		* search process by pid
		*/
		constexpr static auto FINDP_BY_PID = [](const PROCESSENTRY32W& pe, int pid) {
			return pe.th32ProcessID == pid;
		};
		/*
		* search process by name
		*/
		constexpr static auto FINDP_BY_NAME = [](const PROCESSENTRY32W pe, const WCHAR* processName) {
			return API(KERNEL32, lstrcmpW)(processName, pe.szExeFile) == 0;
		};

		ProcessMonitor() noexcept;
		ProcessMonitor(int pid);
		ProcessMonitor(const WCHAR* processName);

		int getPid() const noexcept { return pe.th32ProcessID; }
		int getTid() const noexcept { return te.th32ThreadID; }
		static int getPid(const WCHAR* processName);
		static int getTid(const WCHAR* processName);
		const WCHAR* getName() const;
		static const WCHAR* getName(const WCHAR* processName);

		bool isExist() { return getPid() != -1; }

		/*
		* @return THREADENTRY32/PROCESSENTRY32W on predicate and additional variable,
		* predicate is a function with THREADENTRY32/PROCESSENTRY32W and an additional variable
		*/
		template<typename T, class Predicate, class Additional>
		static T getEntry(const Predicate& pred, Additional comp)
			requires(core::is_same_v<T, THREADENTRY32> || core::is_same_v<T, PROCESSENTRY32W>)
		{
			T pe;
			BOOL hResult;
			initEntry(pe);

			int code = TH32CS_SNAPPROCESS;

			if constexpr (core::is_same_v<T, THREADENTRY32>)
			{
				code = TH32CS_SNAPTHREAD;
			}

			HANDLE hSnapshot = API(KERNEL32, CreateToolhelp32Snapshot)(code, 0);

			if (INVALID_HANDLE_VALUE == hSnapshot)
			{
				return pe;
			}

			if constexpr (core::is_same_v<T, THREADENTRY32>) {
				hResult = API(KERNEL32, Thread32First)(hSnapshot, &pe);
			}
			else {
				hResult = API(KERNEL32, Process32FirstW)(hSnapshot, &pe);
			}

			do
			{
				if (pred(pe, comp))
				{
					hResult = TRUE;
					break;
				}

				if constexpr (core::is_same_v<T, THREADENTRY32>) {
					hResult = API(KERNEL32, Thread32Next)(hSnapshot, &pe);
				}
				else {
					hResult = API(KERNEL32, Process32NextW)(hSnapshot, &pe);
				}
			} while (hResult);

			API(KERNEL32, CloseHandle)(hSnapshot);
			if (hResult == FALSE)
			{
				initEntry(pe);
			}
			return pe;
		}

		/*
		* fill THREADENTRY32 seroes, init dwSize, set pid as -1
		*/
		static void initTEntry(THREADENTRY32& te) noexcept { initEntry(te); }

		/*
		* fill PROCESSENTRY32W seroes, init dwSize, set pid as -1
		*/
		static void initPEntry(PROCESSENTRY32W& pe) noexcept { initEntry(pe); }

		void clear() noexcept;

	private:
		template<typename T>
		static void initEntry(T& te) noexcept(core::is_same_v<T, THREADENTRY32> || core::is_same_v<T, PROCESSENTRY32W>) {
			core::memset(&te, 0, sizeof te);
			te.dwSize = sizeof te;

			if constexpr (core::is_same_v<T, THREADENTRY32>) {
				te.th32ThreadID = -1;
			}
			else {
				te.th32ProcessID = -1;
			}
		}
	};

	class PROCESS_EXPORT Process {
		bool _isSuspended;
		STARTUPINFOW si;
		PROCESS_INFORMATION pi;

	public:
		Process();
		Process(int pid, int pAccess = -1, int tAccess = -1);
		// ub - run new process or search existing?
		//Process(const WCHAR* processName, int pAccess = -1, int tAccess = -1);
		Process(Process&& other) noexcept;
		Process& operator=(Process&& other) noexcept;
		~Process();

		int getPid() const noexcept { return pi.dwProcessId; }
		int getTid() const noexcept { return pi.dwThreadId; }

		/*
		* reopen process handle, if tAccess dont set, use pAccess for all
		* @return false if handle opened or failed, true if done
		*/
		bool open(int pId, int tId, int pAccess = PROCESS_ALL_ACCESS, int tAccess = -1);

		bool run(const WCHAR* exetutable, WCHAR* args = nullptr, int creationFlag = 0);

		bool isOpen() const noexcept { return pi.hProcess != nullptr && pi.hThread != nullptr; }

		bool is64() const;

		bool isSuspended() const noexcept {
			return _isSuspended;
		}

		bool hollowing(const WCHAR* name);

		/*
		* handle must be THREAD_SUSPEND_RESUME
		* return true if process suspended, false in other
		*/
		bool suspend();

		/*
		* handle must be THREAD_SUSPEND_RESUME
		* return true if process resumed, false in other
		*/
		bool resume();

		/*
		* @return false if handle dont opened of failed, true if done
		*/
		bool kill();

		bool wait(size_t tm = INFINITE);

		/*
		* close handle if opened
		*/
		void close();

		constexpr static PIMAGE_NT_HEADERS GetNTHeaders(size_t dwImageBase)
		{
			return (PIMAGE_NT_HEADERS)(dwImageBase + ((PIMAGE_DOS_HEADER)dwImageBase)->e_lfanew);
		}

		constexpr bool static GetLoadedImage(size_t base, PLOADED_IMAGE pImage) {
			PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)base;
			PIMAGE_NT_HEADERS pNTHeaders = GetNTHeaders(base);

			pImage->FileHeader = (PIMAGE_NT_HEADERS)(base + pDosHeader->e_lfanew);
			pImage->NumberOfSections = pImage->FileHeader->FileHeader.NumberOfSections;
			pImage->Sections = (PIMAGE_SECTION_HEADER)(base + pDosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS));
			return false;
		}

	private:
		void clearLocalVariable();

		void swap(Process& a, Process& b) noexcept;
	};
}