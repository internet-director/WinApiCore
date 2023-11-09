#include "pch.h"
#include "NtApi.h"

static SYSTEM_INFO var;
BOOLEAN RtlpNotAllowingMultipleActivation = FALSE;
HANDLE BaseDllHandle = NULL;
HANDLE BaseNamedObjectDirectory = NULL;

namespace core {
	void RtlActivateActivationContextUnsafeFast(
			wtype::PRTL_CALLER_ALLOCATED_ACTIVATION_CONTEXT_STACK_FRAME Frame,
			wtype::PACTIVATION_CONTEXT ActivationContext
		)
	{
		const auto Teb = Wobf::GetTEB();
		const wtype::PRTL_ACTIVATION_CONTEXT_STACK_FRAME pStackFrame =
			(wtype::PRTL_ACTIVATION_CONTEXT_STACK_FRAME)Teb->ActivationContextStack.ActiveFrame;
		//ASSERT(Frame->Format == RTL_CALLER_ALLOCATED_ACTIVATION_CONTEXT_STACK_FRAME_FORMAT_WHISTLER);
		//ASSERT(Frame->Size >= sizeof(RTL_CALLER_ALLOCATED_ACTIVATION_CONTEXT_STACK_FRAME));

		if ((((pStackFrame == NULL) && (ActivationContext == NULL)) ||
			(pStackFrame && (pStackFrame->ActivationContext == ActivationContext)))
			&& !RtlpNotAllowingMultipleActivation)
		{
			Frame->Frame.Flags |= RTL_ACTIVATION_CONTEXT_STACK_FRAME_FLAG_NOT_REALLY_ACTIVATED;
		}
		else
		{
			Frame->Frame.Previous = static_cast<wtype::PRTL_ACTIVATION_CONTEXT_STACK_FRAME>(Teb->ActivationContextStack.ActiveFrame);
			Frame->Frame.ActivationContext = ActivationContext;
			Frame->Frame.Flags = 0;
			Teb->ActivationContextStack.ActiveFrame = &Frame->Frame;
		}
	}
	void RtlDeactivateActivationContextUnsafeFast(
			wtype::PRTL_CALLER_ALLOCATED_ACTIVATION_CONTEXT_STACK_FRAME Frame
		)
	{
		const auto Teb = Wobf::GetTEB();

		if (!RtlpNotAllowingMultipleActivation &&
			((Frame->Frame.Flags & RTL_ACTIVATION_CONTEXT_STACK_FRAME_FLAG_NOT_REALLY_ACTIVATED) != 0))
		{
			return;
		}
		else if (Teb->ActivationContextStack.ActiveFrame != &Frame->Frame)
		{
			EXCEPTION_RECORD ExceptionRecord;
			ULONG InterveningFrameCount = 0;

			wtype::PRTL_ACTIVATION_CONTEXT_STACK_FRAME SearchFrame = 
				static_cast<wtype::PRTL_ACTIVATION_CONTEXT_STACK_FRAME>(Teb->ActivationContextStack.ActiveFrame);
			const wtype::PRTL_ACTIVATION_CONTEXT_STACK_FRAME Previous = Frame->Frame.Previous;

			while ((SearchFrame != NULL) && (SearchFrame != Previous)) {
				InterveningFrameCount++;
				SearchFrame = SearchFrame->Previous;
			}

			ExceptionRecord.ExceptionRecord = NULL;
			ExceptionRecord.NumberParameters = 3;
			ExceptionRecord.ExceptionInformation[0] = InterveningFrameCount;
			ExceptionRecord.ExceptionInformation[1] = (ULONG_PTR)&Frame->Frame;
			ExceptionRecord.ExceptionInformation[2] = (ULONG_PTR)Teb->ActivationContextStack.ActiveFrame;

			if (SearchFrame != NULL) {
				if (InterveningFrameCount == 0) {
					ExceptionRecord.ExceptionCode = STATUS_SXS_MULTIPLE_DEACTIVATION;
				}
				else {
					ExceptionRecord.ExceptionCode = STATUS_SXS_EARLY_DEACTIVATION;
				}

				ExceptionRecord.ExceptionFlags = 0;
			}
			else {
				ExceptionRecord.ExceptionCode = STATUS_SXS_INVALID_DEACTIVATION;
				ExceptionRecord.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
			}

			RtlRaiseException(&ExceptionRecord);
		}

		Teb->ActivationContextStack.ActiveFrame = Frame->Frame.Previous;

	}
	PLARGE_INTEGER NtApi::BaseFormatTimeOut(OUT PLARGE_INTEGER TimeOut, IN DWORD Milliseconds)
	{
		if ((LONG)Milliseconds == -1) {
			return(NULL);
		}
		TimeOut->QuadPart = UInt32x32To64(Milliseconds, 10000);
		TimeOut->QuadPart *= -1;
		return TimeOut;
	}

	HANDLE NtApi::CreateMutexW(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCWSTR lpName)
	{
		NTSTATUS Status;
		OBJECT_ATTRIBUTES Obja;
		HANDLE Handle = nullptr;
		UNICODE_STRING name;
		if (lpName != nullptr) {
			NtApi::RtlInitUnicodeString(&name, lpName);

			InitializeObjectAttributes(&Obja, &name, 0, NULL, NULL);
		}
		else {
			InitializeObjectAttributes(&Obja, NULL, 0, NULL, NULL);
		}

		Status = SYS(NtCreateMutant)(
			&Handle,
			MUTANT_ALL_ACCESS,
			&Obja,
			(BOOLEAN)bInitialOwner
		);

		if (NT_SUCCESS(Status)) {
			if (Status == STATUS_OBJECT_NAME_EXISTS) {
				NtApi::SetLastError(ERROR_ALREADY_EXISTS);
			}
			else {
				NtApi::SetLastError(0);
			}
			return Handle;
		}
		else {
			NtApi::BaseSetLastNTError(Status);
			return NULL;
		}
	}

	bool NtApi::ReleaseMutex(HANDLE hMutex)
	{
		NTSTATUS Status;

		Status = SYS(NtReleaseMutant)(hMutex, NULL);
		if (!NT_SUCCESS(Status)) {
			NtApi::BaseSetLastNTError(Status);
			return false;
		}
		return true;
	}

	DWORD NtApi::WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds)
	{
		return NtApi::WaitForSingleObjectEx(hHandle, dwMilliseconds, FALSE);
	}
	DWORD NtApi::WaitForSingleObjectEx(HANDLE hHandle, DWORD dwMilliseconds, BOOL bAlertable)
	{
		NTSTATUS Status;
		LARGE_INTEGER TimeOut;
		PLARGE_INTEGER pTimeOut;
		wtype::RTL_CALLER_ALLOCATED_ACTIVATION_CONTEXT_STACK_FRAME Frame = { sizeof(Frame), RTL_CALLER_ALLOCATED_ACTIVATION_CONTEXT_STACK_FRAME_FORMAT_WHISTLER };

		//core::RtlActivateActivationContextUnsafeFast(&Frame, NULL); // make the process default activation context active so that APCs are delivered under it
		//__try {

			auto Peb = Wobf::GetPEB();
			switch (HandleToUlong(hHandle)) {
			case STD_INPUT_HANDLE:  hHandle = Peb->ProcessParameters->StandardInput;
				break;
			case STD_OUTPUT_HANDLE: hHandle = Peb->ProcessParameters->StandardOutput;
				break;
			case STD_ERROR_HANDLE:  hHandle = Peb->ProcessParameters->StandardError;
				break;
			}

			/*if (CONSOLE_HANDLE(hHandle) && VerifyConsoleIoHandle(hHandle)) {
				hHandle = GetConsoleInputWaitHandle();
			}*/

			pTimeOut = NtApi::BaseFormatTimeOut(&TimeOut, dwMilliseconds);
		rewait:
			Status = SYS(NtWaitForSingleObject)(hHandle, (BOOLEAN)bAlertable, pTimeOut);
			if (!NT_SUCCESS(Status)) {
				NtApi::BaseSetLastNTError(Status);
				Status = (NTSTATUS)-1;
			}
			else {
				if (bAlertable && Status == STATUS_ALERTED) {
					goto rewait;
				}
			}
		/* }
		__finally {
			//deactivation
		}*/
		//core::RtlDeactivateActivationContextUnsafeFast(&Frame);

		return (DWORD)Status;
	}
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
		return reinterpret_cast<HANDLE>(-1);
		//return API(KERNEL32, GetCurrentProcess)();
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

	LPSYSTEM_INFO WINAPI NtApi::GetSystemInfo()
	{
		NTSTATUS Status;
		wtype::SYSTEM_BASIC_INFORMATION BasicInfo;
		wtype::SYSTEM_PROCESSOR_INFORMATION ProcessorInfo;


		Status = SYS(NtQuerySystemInformation)(
			SystemBasicInformation,
			&BasicInfo,
			sizeof(BasicInfo),
			NULL
		);
		if (!NT_SUCCESS(Status)) {
			core::zeromem(&var, sizeof var);
			return &var;
		}

		Status = SYS(NtQuerySystemInformation)(
			static_cast<SYSTEM_INFORMATION_CLASS>(wtype::SystemProcessorInformation),
			&ProcessorInfo,
			sizeof(ProcessorInfo),
			NULL
		);
		if (!NT_SUCCESS(Status)) {
			core::zeromem(&var, sizeof var);
			return &var;
		}

		core::NtApi::GetSystemInfoInternal(
			&BasicInfo,
			&ProcessorInfo,
			&var);

		return &var;
	}

	void core::NtApi::GetSystemInfoInternal(
			IN wtype::PSYSTEM_BASIC_INFORMATION BasicInfo,
			IN wtype::PSYSTEM_PROCESSOR_INFORMATION ProcessorInfo,
			OUT LPSYSTEM_INFO lpSystemInfo
		)
	{
		core::zeromem(lpSystemInfo, sizeof SYSTEM_INFO);

		lpSystemInfo->wProcessorArchitecture = ProcessorInfo->ProcessorArchitecture;
		lpSystemInfo->wReserved = 0;
		lpSystemInfo->dwPageSize = BasicInfo->PageSize;
		lpSystemInfo->lpMinimumApplicationAddress = (LPVOID)BasicInfo->MinimumUserModeAddress;
		lpSystemInfo->lpMaximumApplicationAddress = (LPVOID)BasicInfo->MaximumUserModeAddress;
		lpSystemInfo->dwActiveProcessorMask = BasicInfo->ActiveProcessorsAffinityMask;
		lpSystemInfo->dwNumberOfProcessors = BasicInfo->NumberOfProcessors;
		lpSystemInfo->wProcessorLevel = ProcessorInfo->ProcessorLevel;
		lpSystemInfo->wProcessorRevision = ProcessorInfo->ProcessorRevision;

		if (ProcessorInfo->ProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL) {
			if (ProcessorInfo->ProcessorLevel == 3) {
				lpSystemInfo->dwProcessorType = PROCESSOR_INTEL_386;
			}
			else
				if (ProcessorInfo->ProcessorLevel == 4) {
					lpSystemInfo->dwProcessorType = PROCESSOR_INTEL_486;
				}
				else {
					lpSystemInfo->dwProcessorType = PROCESSOR_INTEL_PENTIUM;
				}
		}
		else
			if (ProcessorInfo->ProcessorArchitecture == PROCESSOR_ARCHITECTURE_MIPS) {
				lpSystemInfo->dwProcessorType = PROCESSOR_MIPS_R4000;
			}
			else
				if (ProcessorInfo->ProcessorArchitecture == PROCESSOR_ARCHITECTURE_ALPHA) {
					lpSystemInfo->dwProcessorType = PROCESSOR_ALPHA_21064;
				}
				else
					if (ProcessorInfo->ProcessorArchitecture == PROCESSOR_ARCHITECTURE_PPC) {
						lpSystemInfo->dwProcessorType = 604;  // backward compatibility
					}
					else
						if (ProcessorInfo->ProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64) {
							lpSystemInfo->dwProcessorType = PROCESSOR_INTEL_IA64;
						}
						else {
							lpSystemInfo->dwProcessorType = 0;
						}

		lpSystemInfo->dwAllocationGranularity = BasicInfo->AllocationGranularity;

		if (GetProcessVersion(0) < 0x30033) {
			lpSystemInfo->wProcessorLevel = 0;
			lpSystemInfo->wProcessorRevision = 0;
		}

		return;
	}
}