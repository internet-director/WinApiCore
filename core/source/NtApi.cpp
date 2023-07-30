#include "pch.h"
#include "NtApi.h"

namespace core {
	bool NtApi::TerminateProcess(HANDLE hProcess, UINT uExitCode)
	{
		if (hProcess == nullptr) return false;
		NTSTATUS Status = SYS(NtTerminateProcess)(hProcess, (NTSTATUS)uExitCode);
		return NT_SUCCESS(Status);
	}
	bool NtApi::TerminateThread(HANDLE hProcess, UINT uExitCode)
	{
		if (hProcess == nullptr) return false;
		NTSTATUS Status = SYS(NtTerminateThread)(hProcess, (NTSTATUS)uExitCode);
		return NT_SUCCESS(Status);
	}
	DWORD NtApi::SuspendThread(HANDLE hThread)
	{
		NTSTATUS Status;
		DWORD PreviousSuspendCount = 0;

		Status = SYS(NtSuspendThread)(hThread, &PreviousSuspendCount);

		return NT_SUCCESS(Status) ? PreviousSuspendCount : -1;
	}
	DWORD NtApi::ResumeThread(HANDLE hThread)
	{
		NTSTATUS Status;
		DWORD PreviousSuspendCount = 0;

		Status = SYS(NtResumeThread)(hThread, &PreviousSuspendCount);

		return NT_SUCCESS(Status) ? PreviousSuspendCount : -1;
	}
	bool NtApi::CloseHandle(HANDLE h)
	{
		return NT_SUCCESS(SYS(NtClose)(h));
	}
	void NtApi::RtlInitUnicodeString(PUNICODE_STRING DestinationString, PCWSTR SourceString)
	{
		DestinationString->Length = core::strlen(SourceString) * 2;
		DestinationString->MaximumLength = DestinationString->Length + 2;
		DestinationString->Buffer = const_cast<PWCHAR>(SourceString);
	}
	HANDLE NtApi::GetProcessHeap()
	{
		return core::Wobf::GetPEB()->ProcessHeap;
	}
	HANDLE NtApi::GetCurrentProcess()
	{
		// return -1;
		return API(KERNEL32, GetCurrentProcess)();
	}
	DWORD NtApi::GetLastError()
	{
		return core::Wobf::GetTEB()->LastErrorValue;
	}
	HANDLE NtApi::CreateFileW(_In_ LPCWSTR lpFileName, _In_ DWORD dwDesiredAccess, _In_ DWORD dwShareMode,
		_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes, _In_ DWORD dwCreationDisposition, _In_ DWORD dwFlagsAndAttributes, _In_opt_ HANDLE hTemplateFile
	)
	{
		ULONG EaSize = 0;
		ULONG CreateFlags = 0;
		ULONG CreateDisposition;
		bool EndsInSlash;
		UNICODE_STRING FileName;
		RTL_RELATIVE_NAME_U RelativeName;
		PFILE_FULL_EA_INFORMATION EaBuffer = nullptr;

		NtApi::RtlInitUnicodeString(&FileName, lpFileName);

		if (FileName.Length > 1 && lpFileName[(FileName.Length >> 1) - 1] == (WCHAR)'\\') {
			EndsInSlash = TRUE;
		}
		else {
			EndsInSlash = FALSE;
		}

		// CreateDisposition
		{
			switch (dwCreationDisposition) {
			case CREATE_NEW:
				CreateDisposition = FILE_CREATE;
				break;
			case CREATE_ALWAYS:
				CreateDisposition = FILE_OVERWRITE_IF;
				break;
			case OPEN_EXISTING:
				CreateDisposition = FILE_OPEN;
				break;
			case OPEN_ALWAYS:
				CreateDisposition = FILE_OPEN_IF;
				break;
			case TRUNCATE_EXISTING:
				CreateDisposition = FILE_OPEN;
				if (!(dwDesiredAccess & GENERIC_WRITE)) {
					NtApi::BaseSetLastNTError(STATUS_INVALID_PARAMETER);
					return INVALID_HANDLE_VALUE;
				}
				break;
			default:
				NtApi::BaseSetLastNTError(STATUS_INVALID_PARAMETER);
				return INVALID_HANDLE_VALUE;
			}
		}

		// CreateFlags
		{
			CreateFlags |= (dwFlagsAndAttributes & FILE_FLAG_NO_BUFFERING ? FILE_NO_INTERMEDIATE_BUFFERING : 0);
			CreateFlags |= (dwFlagsAndAttributes & FILE_FLAG_WRITE_THROUGH ? FILE_WRITE_THROUGH : 0);
			CreateFlags |= (dwFlagsAndAttributes & FILE_FLAG_OVERLAPPED ? 0 : FILE_SYNCHRONOUS_IO_NONALERT);
			CreateFlags |= (dwFlagsAndAttributes & FILE_FLAG_SEQUENTIAL_SCAN ? FILE_SEQUENTIAL_ONLY : 0);
			CreateFlags |= (dwFlagsAndAttributes & FILE_FLAG_RANDOM_ACCESS ? FILE_RANDOM_ACCESS : 0);
			CreateFlags |= (dwFlagsAndAttributes & FILE_FLAG_BACKUP_SEMANTICS ? FILE_OPEN_FOR_BACKUP_INTENT : 0);
		
			if (dwFlagsAndAttributes & FILE_FLAG_DELETE_ON_CLOSE) {
				CreateFlags |= FILE_DELETE_ON_CLOSE;
				dwDesiredAccess |= DELETE;
			}

			if (dwFlagsAndAttributes & FILE_FLAG_OPEN_REPARSE_POINT) {
				CreateFlags |= FILE_OPEN_REPARSE_POINT;
			}

			if (dwFlagsAndAttributes & FILE_FLAG_OPEN_NO_RECALL) {
				CreateFlags |= FILE_OPEN_NO_RECALL;
			}

			//
			// Backup semantics allow directories to be opened
			//

			if (!(dwFlagsAndAttributes & FILE_FLAG_BACKUP_SEMANTICS)) {
				CreateFlags |= FILE_NON_DIRECTORY_FILE;
			}
			else {

				//
				// Backup intent was specified... Now look to see if we are to allow
				// directory creation
				//

				if ((dwFlagsAndAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
					(dwFlagsAndAttributes & FILE_FLAG_POSIX_SEMANTICS) &&
					(CreateDisposition == FILE_CREATE)) {
					CreateFlags |= FILE_DIRECTORY_FILE;
				}
			}
		}

		UNICODE_STRING uFileName;
		core::RtlInitUnicodeString(&uFileName, lpFileName);

		HANDLE result = nullptr;
		IO_STATUS_BLOCK IoStatusBlock;

		core::zeromem(&IoStatusBlock, sizeof IoStatusBlock);
		OBJECT_ATTRIBUTES FileObjectAttributes;
		InitializeObjectAttributes(&FileObjectAttributes, 
			&uFileName, 
			dwFlagsAndAttributes & FILE_FLAG_POSIX_SEMANTICS ? 0 : OBJ_CASE_INSENSITIVE,
			NULL, 
			NULL);

		NTSTATUS status = SYS(NtCreateFile)(&result,
			dwDesiredAccess | SYNCHRONIZE | FILE_READ_ATTRIBUTES, 
			&FileObjectAttributes, 
			&IoStatusBlock, 
			nullptr,
			dwFlagsAndAttributes & (FILE_ATTRIBUTE_VALID_FLAGS & ~FILE_ATTRIBUTE_DIRECTORY),
			dwShareMode, 
			CreateDisposition,
			CreateFlags,
			EaBuffer,
			EaSize);

		if (!NT_SUCCESS(status)) {
			NtApi::BaseSetLastNTError(status);
			if (status == STATUS_OBJECT_NAME_COLLISION) {
				NtApi::SetLastError(ERROR_FILE_EXISTS);
			}
			else if (status == STATUS_FILE_IS_A_DIRECTORY) {
				if (EndsInSlash) {
					NtApi::SetLastError(ERROR_PATH_NOT_FOUND);
				}
				else {
					NtApi::SetLastError(ERROR_ACCESS_DENIED);
				}
			}
			return INVALID_HANDLE_VALUE;
		}

		return result;
	}
	bool NtApi::WriteFile(_In_ HANDLE hFile, LPCVOID lpBuffer, _In_ DWORD nNumberOfBytesToWrite,
		_Out_opt_ LPDWORD lpNumberOfBytesWritten, _Inout_opt_ LPOVERLAPPED lpOverlapped) 
	{
		NTSTATUS Status;
		IO_STATUS_BLOCK IoStatusBlock;
		wtype::PPEB Peb;

		if (lpNumberOfBytesWritten != nullptr) {
			*lpNumberOfBytesWritten = 0;
		}
		else {
			return false;
		}

		Peb = Wobf::GetPEB();
		switch (HandleToUlong(hFile)) {
		case STD_INPUT_HANDLE:  hFile = Peb->ProcessParameters->StandardInput;
			break;
		case STD_OUTPUT_HANDLE: hFile = Peb->ProcessParameters->StandardOutput;
			break;
		case STD_ERROR_HANDLE:  hFile = Peb->ProcessParameters->StandardError;
			break;
		}

		if (CONSOLE_HANDLE(hFile)) {
			return API(KERNEL32, WriteConsoleA)(hFile,
				(LPVOID)lpBuffer,
				nNumberOfBytesToWrite,
				lpNumberOfBytesWritten,
				lpOverlapped
			);
		}

		if (lpOverlapped != nullptr) {
			LARGE_INTEGER Li;

			lpOverlapped->Internal = (DWORD)STATUS_PENDING;
			Li.LowPart = lpOverlapped->Offset;
			Li.HighPart = lpOverlapped->OffsetHigh;
			Status = SYS(NtWriteFile)(
				hFile,
				lpOverlapped->hEvent,
				NULL,
				(ULONG_PTR)lpOverlapped->hEvent & 1 ? NULL : lpOverlapped,
				(PIO_STATUS_BLOCK)&lpOverlapped->Internal,
				(PVOID)lpBuffer,
				nNumberOfBytesToWrite,
				&Li,
				NULL
			);

			if (!NT_ERROR(Status) && Status != STATUS_PENDING) {
				if (lpNumberOfBytesWritten != nullptr) {
					*lpNumberOfBytesWritten = (DWORD)lpOverlapped->InternalHigh;
				}
				return true;
			}
			else {
				NtApi::BaseSetLastNTError(Status);
				return false;
			}
		}
		else {
			Status = SYS(NtWriteFile)(
				hFile,
				NULL,
				NULL,
				NULL,
				&IoStatusBlock,
				(PVOID)lpBuffer,
				nNumberOfBytesToWrite,
				NULL,
				NULL
			);

			if (Status == STATUS_PENDING) {
				// Operation must complete before return & IoStatusBlock destroyed
				Status = SYS(NtWaitForSingleObject)(hFile, FALSE, NULL);
				if (NT_SUCCESS(Status)) {
					Status = IoStatusBlock.Status;
				}
			}

			if (NT_SUCCESS(Status)) {
				*lpNumberOfBytesWritten = (DWORD)IoStatusBlock.Information;
				return TRUE;
			}
			else {
				if (NT_WARNING(Status)) {
					*lpNumberOfBytesWritten = (DWORD)IoStatusBlock.Information;
				}
				NtApi::BaseSetLastNTError(Status);
				return false;
			}
		}
		return true;
	}

	HANDLE NtApi::OpenProcTh(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwId, bool isThread)
	{
		NTSTATUS Status;
		OBJECT_ATTRIBUTES Obja;
		HANDLE Handle = nullptr;
		CLIENT_ID ClientId;
		core::zeromem(&ClientId, sizeof CLIENT_ID);

		if (isThread) ClientId.UniqueThread = reinterpret_cast<HANDLE>(dwId);
		else ClientId.UniqueProcess = reinterpret_cast<HANDLE>(dwId);

		InitializeObjectAttributes(
			&Obja,
			NULL,
			(bInheritHandle ? OBJ_INHERIT : 0),
			NULL,
			NULL
		);
		if (isThread) {
			Status = SYS(NtOpenThread)(
				&Handle,
				(ACCESS_MASK)dwDesiredAccess,
				&Obja,
				&ClientId
				);
		}
		else {
			Status = SYS(NtOpenProcess)(
				&Handle,
				(ACCESS_MASK)dwDesiredAccess,
				&Obja,
				&ClientId
				);
		}

		return NT_SUCCESS(Status) ? Handle : nullptr;
	}
	void NtApi::SetLastError(DWORD dwErrCode)
	{
		Wobf::GetTEB()->LastErrorValue = (LONG)dwErrCode;
	}
	void NtApi::BaseSetLastNTError(NTSTATUS status) {
		//Wobf::GetTEB()->LastStatusValue = status;
		SetLastError(SYS(RtlNtStatusToDosError)(status));
	}

	bool NtApi::IsWow64Process(HANDLE hProcess, PBOOL Wow64Process)
	{
		if (Wow64Process == nullptr) { 
			return false; 
		}

		ULONG_PTR Peb32 = 0;

		NTSTATUS NtStatus = SYS(NtQueryInformationProcess)(
			hProcess,
			ProcessWow64Information,
			&Peb32,
			sizeof Peb32,
			NULL
		);

		if (!NT_SUCCESS(NtStatus)) {
			core::BaseSetLastNTError(NtStatus);
		}
		else {
			(Peb32 == 0) ? (*Wow64Process = FALSE) : (*Wow64Process = TRUE);
		}

		return (NT_SUCCESS(NtStatus));
	}
}