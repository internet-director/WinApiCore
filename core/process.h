#pragma once
#include "framework.h"

namespace core {
	class PROCESS_EXPORT Process {
		HANDLE pHandle;
		HANDLE tHandle;
		THREADENTRY32 te;
		PROCESSENTRY32W pe;

	public:
		/*
		* search by process pid
		*/
		constexpr static auto FINDT_BY_PID = [](const THREADENTRY32& te, int pid) {
			return te.th32OwnerProcessID == pid;
		};
		constexpr static auto FINDP_BY_PID = [](const PROCESSENTRY32W& pe, int pid) {
			return pe.th32ProcessID == pid;
		};
		constexpr static auto FINDP_BY_NAME = [](const PROCESSENTRY32W pe, const WCHAR* processName) {
			return lstrcmpW(processName, pe.szExeFile) == 0;
		};

		Process();
		Process(int pid);
		Process(const HANDLE handle);
		Process(const WCHAR* processName);
		~Process();

		int getPid() const;
		const WCHAR* getName() const;
		static int getPid(const WCHAR* processName);

		template<typename T, class Compare, class Compared>
		static T getEntry(const Compare& pred, Compared comp)
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

			HANDLE hSnapshot = CreateToolhelp32Snapshot(code, 0);

			if (INVALID_HANDLE_VALUE == hSnapshot) 
			{
				return pe;
			}

			if constexpr (core::is_same_v<T, THREADENTRY32>) {
				hResult = Thread32First(hSnapshot, &pe);
			}
			else {
				hResult = Process32FirstW(hSnapshot, &pe);
			}

			do 
			{
				if (pred(pe, comp))
				{
					hResult = TRUE;
					break;
				}

				if constexpr (core::is_same_v<T, THREADENTRY32>) {
					hResult = Thread32Next(hSnapshot, &pe);
				}
				else {
					hResult = Process32NextW(hSnapshot, &pe);
				}
			} while (hResult);

			CloseHandle(hSnapshot);
			if (hResult == FALSE) 
			{
				initEntry(pe);
			}
			return pe;
		}

		/*
		* open process handle, if tAccess dont set, use pAccess for all
		* @return false if handle opened or failed, true if done
		*/
		bool open(int pAccess = PROCESS_ALL_ACCESS, int tAccess = -1);

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

		/*
		* close handle if opened
		*/
		void close();

		/*
		* fill THREADENTRY32 seroes, init dwSize, set pid as -1
		*/
		static void initTEntry(THREADENTRY32& te) { initEntry(te); }

		/*
		* fill PROCESSENTRY32W seroes, init dwSize, set pid as -1
		*/
		static void initPEntry(PROCESSENTRY32W& pe) { initEntry(pe); }

		template<typename T>
		static void initEntry(T& te)
			requires(core::is_same_v<T, THREADENTRY32> || core::is_same_v<T, PROCESSENTRY32W>) {
			core::memset(&te, 0, sizeof(te));
			te.dwSize = sizeof(te);

			if constexpr (core::is_same_v<T, THREADENTRY32>) {
				te.th32ThreadID = -1;
			}
			else {
				te.th32ProcessID = -1;
			}

		}
	};

}