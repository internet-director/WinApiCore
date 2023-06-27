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
		return API(KERNEL32, GetProcessHeap)();
		return reinterpret_cast<HANDLE>(size_t(core::_wobf.GetPEB()->ProcessParameters) - 10520);
	}

	HANDLE NtApi::CreateFileW(_In_ LPCWSTR lpFileName, _In_ DWORD dwDesiredAccess, _In_ DWORD dwShareMode,
		_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes, _In_ DWORD dwCreationDisposition, _In_ DWORD dwFlagsAndAttributes, _In_opt_ HANDLE hTemplateFile
	)
	{
		ULONG EaSize = 0;
		ULONG CreateFlags = 0;
		ULONG CreateDisposition;
		RTL_RELATIVE_NAME_U RelativeName;
		PFILE_FULL_EA_INFORMATION EaBuffer = nullptr;

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
					debug("STATUS_INVALID_PARAMETER");
					return INVALID_HANDLE_VALUE;
				}
				break;
			default:
				debug("STATUS_INVALID_PARAMETER");
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

		return NT_SUCCESS(status) ? result : INVALID_HANDLE_VALUE;
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
}