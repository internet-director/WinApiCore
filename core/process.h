#pragma once
#include "framework.h"

namespace core {
	class PROCESS_EXPORT Process {
		HANDLE handle;
		PROCESSENTRY32W pe;

	public:
		constexpr static auto FIND_BY_PID = [](const PROCESSENTRY32W& pe, int pid) {
			return pe.th32ProcessID == pid; 
		};
		constexpr static auto FIND_BY_NAME = [](const PROCESSENTRY32W pe, const WCHAR* processName) {
			return lstrcmpW(processName, pe.szExeFile) == 0;
		};

		Process();
		Process(int pid);
		Process(const HANDLE handle);
		Process(const WCHAR* processName);
		~Process();

		int getPid() const;
		const WCHAR* getName() const;
		static int getPid(int pid);
		static int getPid(const WCHAR* processName);

		template<class Compare, class Compared>
		static PROCESSENTRY32W getPEntry(const Compare& pred, Compared comp) {
			PROCESSENTRY32W pe;
			initPEntry(pe);

			HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (INVALID_HANDLE_VALUE == hSnapshot) {
				return pe;
			}

			BOOL hResult = Process32FirstW(hSnapshot, &pe);

			do {
				if (pred(pe, comp)) {
					hResult = TRUE;
					break;
				}
			} while ((hResult = Process32NextW(hSnapshot, &pe)));

			CloseHandle(hSnapshot);
			if (hResult == FALSE) {
				initPEntry(pe);
			}
			return pe;
		}

		/*
		@return false if handle opened or failed, true if done
		*/
		bool open(int accecc = PROCESS_ALL_ACCESS);
		/*
		@return false if handle dont opened of failed, true if done
		*/
		bool kill();
		/*
		close handle if opened
		*/
		void close();

		/*
		fill PROCESSENTRY32W seroes, init dwSize, set pid as -1
		*/
		static void initPEntry(PROCESSENTRY32W& pe);
	};

}