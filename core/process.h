#pragma once
#include "framework.h"

namespace core {
	class PROCESS_EXPORT Process {
		HANDLE handle;
		PROCESSENTRY32W pe;

	public:
		Process();
		Process(const HANDLE handle);
		Process(const WCHAR* processName);
		~Process();

		int getPid() const;
		const WCHAR* getName() const;
		static int getPid(int pid);
		static int getPid(const WCHAR* processName);

		template<class Compare>
		static PROCESSENTRY32W getPEntry(const Compare& comp) {
			PROCESSENTRY32W pe;
			initPEntry(pe);

			HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (INVALID_HANDLE_VALUE == hSnapshot) {
				return pe;
			}

			BOOL hResult = Process32FirstW(hSnapshot, &pe);

			do {
				if (comp(pe)) {
					hResult = FALSE;
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

		static void initPEntry(PROCESSENTRY32W& pe);
	};

}