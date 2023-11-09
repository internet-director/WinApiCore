#pragma once
#include <core/config.h>

namespace core {
    template <class _Ty, _Ty _Val>
        struct integral_constant {
        static constexpr _Ty value = _Val;

        using value_type = _Ty;
        using type = integral_constant;

        constexpr operator value_type() const noexcept {
            return value;
        }

        _NODISCARD constexpr value_type operator()() const noexcept {
            return value;
        }
    };

    template <bool _Val>
        using bool_constant = integral_constant<bool, _Val>;

	template <class, class>
	constexpr bool is_same_v = false;
	template <class _Ty>
	constexpr bool is_same_v<_Ty, _Ty> = true;

	template <class T>
	constexpr bool is_char_v = is_same_v<char, T> || is_same_v<wchar_t, T>;

	template <class _Ty>
	struct remove_reference {
		using type = _Ty;
		using _Const_thru_ref_type = const _Ty;
	};

	template <class _Ty>
	struct remove_reference<_Ty&> {
		using type = _Ty;
		using _Const_thru_ref_type = const _Ty&;
	};

	template <class _Ty>
	struct remove_reference<_Ty&&> {
		using type = _Ty;
		using _Const_thru_ref_type = const _Ty&&;
	};

	template <class _Ty>
	using remove_reference_t = typename remove_reference<_Ty>::type;


	template <class>
	// false value attached to a dependent name (for static_assert)
	constexpr bool _Always_false = false;

	template <class... _Types>
	using void_t = void;

	template <class _Ty, class = void>
	struct _Add_reference { // add reference (non-referenceable type)
		using _Lvalue = _Ty;
		using _Rvalue = _Ty;
	};

	template <class _Ty>
	struct _Add_reference<_Ty, void_t<_Ty&>> { // (referenceable type)
		using _Lvalue = _Ty&;
		using _Rvalue = _Ty&&;
	};

	template <class _Ty>
	struct add_lvalue_reference {
		using type = typename _Add_reference<_Ty>::_Lvalue;
	};

	template <class _Ty>
	using add_lvalue_reference_t = typename _Add_reference<_Ty>::_Lvalue;

	template <class _Ty>
	struct add_rvalue_reference {
		using type = typename _Add_reference<_Ty>::_Rvalue;
	};

	template <class _Ty>
	using add_rvalue_reference_t = typename _Add_reference<_Ty>::_Rvalue;

	template <class _Ty>
	add_rvalue_reference_t<_Ty> declval() noexcept {
		static_assert(_Always_false<_Ty>, "Calling declval is ill-formed, see N4892 [declval]/2.");
	}

	template <typename T>
	struct decay {
		template <typename U> static U impl(U);
		using type = decltype(impl(declval<T>()));
	};

	template<typename T>
	using decay_t = typename decay<T>::type;
    using max_align_t = double;


    template <class _Ty, size_t _Len>
    union _Align_type {
        _Ty _Val;
        char _Pad[_Len];
    };

    template <size_t _Len, size_t _Align, class _Ty, bool _Ok>
    struct _Aligned;

    template <size_t _Len, size_t _Align, class _Ty>
    struct _Aligned<_Len, _Align, _Ty, true> {
        using type = _Align_type<_Ty, _Len>;
    };

    template <size_t _Len, size_t _Align>
    struct _Aligned<_Len, _Align, double, false> {
        using type = _Align_type<max_align_t, _Len>;
    };


    template <size_t _Len, size_t _Align>
    struct _Aligned<_Len, _Align, int, false> {
        using _Next = double;
        static constexpr bool _Fits = _Align <= alignof(_Next);
        using type = typename _Aligned<_Len, _Align, _Next, _Fits>::type;
    };

    template <size_t _Len, size_t _Align>
    struct _Aligned<_Len, _Align, short, false> {
        using _Next = int;
        static constexpr bool _Fits = _Align <= alignof(_Next);
        using type = typename _Aligned<_Len, _Align, _Next, _Fits>::type;
    };

    template <size_t _Len, size_t _Align>
    struct _Aligned<_Len, _Align, char, false> {
        using _Next = short;
        static constexpr bool _Fits = _Align <= alignof(_Next);
        using type = typename _Aligned<_Len, _Align, _Next, _Fits>::type;
    };

    template <size_t _Len, size_t _Align = alignof(max_align_t)>
    struct _Aligned_storage {
        using _Next = char;
        static constexpr bool _Fits = _Align <= alignof(_Next);
        using type = typename _Aligned<_Len, _Align, _Next, _Fits>::type;
    };

    template <size_t _Len, size_t _Align = alignof(max_align_t)>
    using _Aligned_storage_t = typename _Aligned_storage<_Len, _Align>::type;

    template <size_t _Len, size_t _Align = alignof(max_align_t)>
        struct aligned_storage {
        using type = _Aligned_storage_t<_Len, _Align>;
    };

    template <size_t _Len, size_t _Align = alignof(max_align_t)>
    using aligned_storage_t = _Aligned_storage_t<_Len, _Align>;

    template <class _Ty>
    inline constexpr bool is_trivially_move_constructible_v = __is_trivially_constructible(_Ty, _Ty);

    template <class _To, class _From>
        struct is_trivially_assignable : bool_constant<__is_trivially_assignable(_To, _From)> {
    };

    template <class _To, class _From>
    inline constexpr bool is_trivially_assignable_v = __is_trivially_assignable(_To, _From);

    template <class _Ty>
        struct is_trivially_copy_assignable
        : bool_constant<__is_trivially_assignable(add_lvalue_reference_t<_Ty>, add_lvalue_reference_t<const _Ty>)> {
    };

    template <class _Ty>
    inline constexpr bool is_trivially_copy_assignable_v =
        __is_trivially_assignable(add_lvalue_reference_t<_Ty>, add_lvalue_reference_t<const _Ty>);

    template <class _Ty>
        struct is_trivially_move_assignable : bool_constant<__is_trivially_assignable(add_lvalue_reference_t<_Ty>, _Ty)> {
    };

    template <class _Ty>
    inline constexpr bool is_trivially_move_assignable_v = __is_trivially_assignable(add_lvalue_reference_t<_Ty>, _Ty);

    template <class _Ty>
        struct is_trivially_destructible : bool_constant<__is_trivially_destructible(_Ty)> {
    };

    template <class _Ty>
    inline constexpr bool is_trivially_destructible_v = __is_trivially_destructible(_Ty);

    template <class _Ty, class... _Args>
        struct is_nothrow_constructible : bool_constant<__is_nothrow_constructible(_Ty, _Args...)> {
    };

    template <class _Ty, class... _Args>
    inline constexpr bool is_nothrow_constructible_v = __is_nothrow_constructible(_Ty, _Args...);

    template <class _Ty>
        struct is_nothrow_copy_constructible
        : bool_constant<__is_nothrow_constructible(_Ty, add_lvalue_reference_t<const _Ty>)> {
    };

    template <class _Ty>
    inline constexpr bool is_nothrow_copy_constructible_v =
        __is_nothrow_constructible(_Ty, add_lvalue_reference_t<const _Ty>);

    template <class _Ty>
        struct is_nothrow_default_constructible : bool_constant<__is_nothrow_constructible(_Ty)> {
    };

    template <class _Ty>
    inline constexpr bool is_nothrow_default_constructible_v = __is_nothrow_constructible(_Ty);

    template <class _Ty>
        struct is_nothrow_move_constructible : bool_constant<__is_nothrow_constructible(_Ty, _Ty)> {
    };

    template <class _Ty>
    constexpr _Ty&& forward(remove_reference_t<_Ty>& _Arg) noexcept {
        return static_cast<_Ty&&>(_Arg);
    }

    template <class _Ty>
    constexpr remove_reference_t<_Ty>&& move(_Ty&& _Arg) noexcept {
        return static_cast<remove_reference_t<_Ty>&&>(_Arg);
    }

	template<typename T> void swap(T& t1, T& t2) noexcept {
		T temp = core::move(t1);
		t1 = core::move(t2);
		t2 = core::move(temp);
	}

	template<auto F>
	using function_t = core::decay_t<decltype(F)>;
}

// winapi types, so as not to conflict with the default definition
namespace wtype {
    using PPVOID = PVOID*;
#define GDI_HANDLE_BUFFER_SIZE32  34
#define GDI_HANDLE_BUFFER_SIZE64  60

#if !defined(_IA64_) && !defined(_AMD64_)
#define GDI_HANDLE_BUFFER_SIZE      GDI_HANDLE_BUFFER_SIZE32
#else
#define GDI_HANDLE_BUFFER_SIZE      GDI_HANDLE_BUFFER_SIZE64
#endif

    typedef ULONG GDI_HANDLE_BUFFER32[GDI_HANDLE_BUFFER_SIZE32];
    typedef ULONG GDI_HANDLE_BUFFER64[GDI_HANDLE_BUFFER_SIZE64];
    typedef ULONG GDI_HANDLE_BUFFER[GDI_HANDLE_BUFFER_SIZE];

    typedef struct _PEB_FREE_BLOCK {
        struct _PEB_FREE_BLOCK* Next;
        ULONG Size;
    } PEB_FREE_BLOCK, * PPEB_FREE_BLOCK;

#define PEBTEB_POINTER(x) x
#define PEBTEB_STRUCT(x)  x

    typedef struct _RTL_DRIVE_LETTER_CURDIR {
        USHORT Flags;
        USHORT Length;
        ULONG TimeStamp;
        STRING DosPath;
    } RTL_DRIVE_LETTER_CURDIR, * PRTL_DRIVE_LETTER_CURDIR;

    typedef struct _CURDIR {
        UNICODE_STRING DosPath;
        HANDLE Handle;
    } CURDIR, * PCURDIR;

#define RTL_MAX_DRIVE_LETTERS 32
#define RTL_DRIVE_LETTER_VALID (USHORT)0x0001

    typedef struct _RTL_USER_PROCESS_PARAMETERS {
        ULONG MaximumLength;
        ULONG Length;

        ULONG Flags;
        ULONG DebugFlags;

        HANDLE ConsoleHandle;
        ULONG  ConsoleFlags;
        HANDLE StandardInput;
        HANDLE StandardOutput;
        HANDLE StandardError;

        CURDIR CurrentDirectory;        // ProcessParameters
        UNICODE_STRING DllPath;         // ProcessParameters
        UNICODE_STRING ImagePathName;   // ProcessParameters
        UNICODE_STRING CommandLine;     // ProcessParameters
        PVOID Environment;              // NtAllocateVirtualMemory

        ULONG StartingX;
        ULONG StartingY;
        ULONG CountX;
        ULONG CountY;
        ULONG CountCharsX;
        ULONG CountCharsY;
        ULONG FillAttribute;

        ULONG WindowFlags;
        ULONG ShowWindowFlags;
        UNICODE_STRING WindowTitle;     // ProcessParameters
        UNICODE_STRING DesktopInfo;     // ProcessParameters
        UNICODE_STRING ShellInfo;       // ProcessParameters
        UNICODE_STRING RuntimeData;     // ProcessParameters
        RTL_DRIVE_LETTER_CURDIR CurrentDirectores[RTL_MAX_DRIVE_LETTERS];
    } RTL_USER_PROCESS_PARAMETERS, * PRTL_USER_PROCESS_PARAMETERS;

#define RTL_CRITSECT_TYPE 0
#define RTL_RESOURCE_TYPE 1

    typedef struct _RTL_CRITICAL_SECTION {
        PRTL_CRITICAL_SECTION_DEBUG DebugInfo;

        //
        //  The following three fields control entering and exiting the critical
        //  section for the resource
        //

        LONG LockCount;
        LONG RecursionCount;
        HANDLE OwningThread;        // from the thread's ClientId->UniqueThread
        HANDLE LockSemaphore;
        ULONG_PTR SpinCount;        // force size on 64-bit systems when packed
    } RTL_CRITICAL_SECTION, * PRTL_CRITICAL_SECTION;

    typedef struct PEBTEB_STRUCT(_PEB) {
        BOOLEAN InheritedAddressSpace;      // These four fields cannot change unless the
        BOOLEAN ReadImageFileExecOptions;   //
        BOOLEAN BeingDebugged;              //
        BOOLEAN SpareBool;                  //
        PEBTEB_POINTER(HANDLE) Mutant;      // INITIAL_PEB structure is also updated.

        PEBTEB_POINTER(PVOID) ImageBaseAddress;
        PEBTEB_POINTER(PPEB_LDR_DATA) Ldr;
        PEBTEB_POINTER(wtype::_RTL_USER_PROCESS_PARAMETERS*) ProcessParameters;
        PEBTEB_POINTER(PVOID) SubSystemData;
        PEBTEB_POINTER(PVOID) ProcessHeap;
        PEBTEB_POINTER(wtype::_RTL_CRITICAL_SECTION*) FastPebLock;
        PEBTEB_POINTER(PVOID) SparePtr1;
        PEBTEB_POINTER(PVOID) SparePtr2;
        ULONG EnvironmentUpdateCount;
        PEBTEB_POINTER(PVOID) KernelCallbackTable;
        ULONG SystemReserved[1];

        struct {
            ULONG ExecuteOptions : 2;
            ULONG SpareBits : 30;
        };


        PEBTEB_POINTER(PPEB_FREE_BLOCK) FreeList;
        ULONG TlsExpansionCounter;
        PEBTEB_POINTER(PVOID) TlsBitmap;
        ULONG TlsBitmapBits[2];
        PEBTEB_POINTER(PVOID) ReadOnlySharedMemoryBase;
        PEBTEB_POINTER(PVOID) ReadOnlySharedMemoryHeap;
        PEBTEB_POINTER(PPVOID) ReadOnlyStaticServerData;
        PEBTEB_POINTER(PVOID) AnsiCodePageData;
        PEBTEB_POINTER(PVOID) OemCodePageData;
        PEBTEB_POINTER(PVOID) UnicodeCaseTableData;

        //
        // Useful information for LdrpInitialize
        ULONG NumberOfProcessors;
        ULONG NtGlobalFlag;

        //
        // Passed up from MmCreatePeb from Session Manager registry key
        //

        LARGE_INTEGER CriticalSectionTimeout;
        PEBTEB_POINTER(SIZE_T) HeapSegmentReserve;
        PEBTEB_POINTER(SIZE_T) HeapSegmentCommit;
        PEBTEB_POINTER(SIZE_T) HeapDeCommitTotalFreeThreshold;
        PEBTEB_POINTER(SIZE_T) HeapDeCommitFreeBlockThreshold;

        //
        // Where heap manager keeps track of all heaps created for a process
        // Fields initialized by MmCreatePeb.  ProcessHeaps is initialized
        // to point to the first free byte after the PEB and MaximumNumberOfHeaps
        // is computed from the page size used to hold the PEB, less the fixed
        // size of this data structure.
        //

        ULONG NumberOfHeaps;
        ULONG MaximumNumberOfHeaps;
        PEBTEB_POINTER(PPVOID) ProcessHeaps;

        //
        //
        PEBTEB_POINTER(PVOID) GdiSharedHandleTable;
        PEBTEB_POINTER(PVOID) ProcessStarterHelper;
        ULONG GdiDCAttributeList;
        PEBTEB_POINTER(struct _RTL_CRITICAL_SECTION*) LoaderLock;

        //
        // Following fields filled in by MmCreatePeb from system values and/or
        // image header.
        //

        ULONG OSMajorVersion;
        ULONG OSMinorVersion;
        USHORT OSBuildNumber;
        USHORT OSCSDVersion;
        ULONG OSPlatformId;
        ULONG ImageSubsystem;
        ULONG ImageSubsystemMajorVersion;
        ULONG ImageSubsystemMinorVersion;
        PEBTEB_POINTER(ULONG_PTR) ImageProcessAffinityMask;
        PEBTEB_STRUCT(GDI_HANDLE_BUFFER) GdiHandleBuffer;
        PEBTEB_POINTER(PPS_POST_PROCESS_INIT_ROUTINE) PostProcessInitRoutine;

        PEBTEB_POINTER(PVOID) TlsExpansionBitmap;
        ULONG TlsExpansionBitmapBits[32];   // TLS_EXPANSION_SLOTS bits

        //
        // Id of the Hydra session in which this process is running
        //
        ULONG SessionId;

        //
        // Filled in by LdrpInstallAppcompatBackend
        //
        ULARGE_INTEGER AppCompatFlags;

        //
        // ntuser appcompat flags
        //
        ULARGE_INTEGER AppCompatFlagsUser;

        //
        // Filled in by LdrpInstallAppcompatBackend
        //
        PEBTEB_POINTER(PVOID) pShimData;

        //
        // Filled in by LdrQueryImageFileExecutionOptions
        //
        PEBTEB_POINTER(PVOID) AppCompatInfo;

        //
        // Used by GetVersionExW as the szCSDVersion string
        //
        PEBTEB_STRUCT(UNICODE_STRING) CSDVersion;

        //
        // Fusion stuff
        //
        PEBTEB_POINTER(const struct _ACTIVATION_CONTEXT_DATA*) ActivationContextData;
        PEBTEB_POINTER(struct _ASSEMBLY_STORAGE_MAP*) ProcessAssemblyStorageMap;
        PEBTEB_POINTER(const struct _ACTIVATION_CONTEXT_DATA*) SystemDefaultActivationContextData;
        PEBTEB_POINTER(struct _ASSEMBLY_STORAGE_MAP*) SystemAssemblyStorageMap;

        //
        // Enforced minimum initial commit stack
        //
        PEBTEB_POINTER(SIZE_T) MinimumStackCommit;

        //
        // Fiber local storage.
        //

        PEBTEB_POINTER(PPVOID) FlsCallback;
        PEBTEB_STRUCT(LIST_ENTRY) FlsListHead;
        PEBTEB_POINTER(PVOID) FlsBitmap;
        ULONG FlsBitmapBits[FLS_MAXIMUM_AVAILABLE / (sizeof(ULONG) * 8)];
        ULONG FlsHighIndex;
    } PEBTEB_STRUCT(PEB), * PEBTEB_STRUCT(PPEB);

#define STATIC_UNICODE_BUFFER_LENGTH 261
#define WIN32_CLIENT_INFO_LENGTH 62
#define WIN32_CLIENT_INFO_SPIN_COUNT 1

#define GDI_BATCH_BUFFER_SIZE 310

    typedef struct _GDI_TEB_BATCH {
        ULONG Offset;
        ULONG HDC;
        ULONG Buffer[GDI_BATCH_BUFFER_SIZE];
    } GDI_TEB_BATCH, * PGDI_TEB_BATCH;


    //
    // Wx86 thread state information
    //

    typedef struct _Wx86ThreadState {
        PULONG  CallBx86Eip;
        PVOID   DeallocationCpu;
        BOOLEAN UseKnownWx86Dll;
        char    OleStubInvoked;
    } WX86THREAD, * PWX86THREAD;
    //
    //  Fusion/sxs thread state information
    //

#define ACTIVATION_CONTEXT_STACK_FLAG_QUERIES_DISABLED (0x00000001)

    typedef struct _ACTIVATION_CONTEXT_STACK {
        ULONG Flags;
        ULONG NextCookieSequenceNumber;
        PVOID ActiveFrame;
        LIST_ENTRY FrameListCache;

#if NT_SXS_PERF_COUNTERS_ENABLED
        struct _ACTIVATION_CONTEXT_STACK_PERF_COUNTERS {
            ULONGLONG Activations;
            ULONGLONG ActivationCycles;
            ULONGLONG Deactivations;
            ULONGLONG DeactivationCycles;
        } Counters;
#endif // NT_SXS_PERF_COUNTERS_ENABLED
    } ACTIVATION_CONTEXT_STACK, * PACTIVATION_CONTEXT_STACK;

    typedef const ACTIVATION_CONTEXT_STACK* PCACTIVATION_CONTEXT_STACK;

#define TEB_ACTIVE_FRAME_CONTEXT_FLAG_EXTENDED (0x00000001)

    typedef struct _TEB_ACTIVE_FRAME_CONTEXT {
        ULONG Flags;
        PCSTR FrameName;
    } TEB_ACTIVE_FRAME_CONTEXT, * PTEB_ACTIVE_FRAME_CONTEXT;

    typedef const struct _TEB_ACTIVE_FRAME_CONTEXT* PCTEB_ACTIVE_FRAME_CONTEXT;

    typedef struct _TEB_ACTIVE_FRAME_CONTEXT_EX {
        TEB_ACTIVE_FRAME_CONTEXT BasicContext;
        PCSTR SourceLocation; // e.g. "Z:\foo\bar\baz.c"
    } TEB_ACTIVE_FRAME_CONTEXT_EX, * PTEB_ACTIVE_FRAME_CONTEXT_EX;

    typedef const struct _TEB_ACTIVE_FRAME_CONTEXT_EX* PCTEB_ACTIVE_FRAME_CONTEXT_EX;

#define TEB_ACTIVE_FRAME_FLAG_EXTENDED (0x00000001)

    typedef struct _TEB_ACTIVE_FRAME {
        ULONG Flags;
        struct _TEB_ACTIVE_FRAME* Previous;
        PCTEB_ACTIVE_FRAME_CONTEXT Context;
    } TEB_ACTIVE_FRAME, * PTEB_ACTIVE_FRAME;

    typedef const struct _TEB_ACTIVE_FRAME* PCTEB_ACTIVE_FRAME;

    typedef struct _TEB_ACTIVE_FRAME_EX {
        TEB_ACTIVE_FRAME BasicFrame;
        PVOID ExtensionIdentifier; // use address of your DLL Main or something unique to your mapping in the address space
    } TEB_ACTIVE_FRAME_EX, * PTEB_ACTIVE_FRAME_EX;

    typedef const struct _TEB_ACTIVE_FRAME_EX* PCTEB_ACTIVE_FRAME_EX;

    typedef struct _TEB {
        NT_TIB NtTib;
        PVOID  EnvironmentPointer;
        CLIENT_ID ClientId;
        PVOID ActiveRpcHandle;
        PVOID ThreadLocalStoragePointer;
#if defined(PEBTEB_BITS)
        PVOID ProcessEnvironmentBlock;
#else
        PPEB ProcessEnvironmentBlock;
#endif
        ULONG LastErrorValue;
        ULONG CountOfOwnedCriticalSections;
        PVOID CsrClientThread;
        PVOID Win32ThreadInfo;          // PtiCurrent
        ULONG User32Reserved[26];       // user32.dll items
        ULONG UserReserved[5];          // Winsrv SwitchStack
        PVOID WOW32Reserved;            // used by WOW
        LCID CurrentLocale;
        ULONG FpSoftwareStatusRegister; // offset known by outsiders!
        PVOID SystemReserved1[54];      // Used by FP emulator
        NTSTATUS ExceptionCode;         // for RaiseUserException
        ACTIVATION_CONTEXT_STACK ActivationContextStack;   // Fusion activation stack
        // sizeof(PVOID) is a way to express processor-dependence, more generally than #ifdef _WIN64
        UCHAR SpareBytes1[48 - sizeof(PVOID) - sizeof(ACTIVATION_CONTEXT_STACK)];
        GDI_TEB_BATCH GdiTebBatch;      // Gdi batching
        CLIENT_ID RealClientId;
        HANDLE GdiCachedProcessHandle;
        ULONG GdiClientPID;
        ULONG GdiClientTID;
        PVOID GdiThreadLocalInfo;
        ULONG_PTR Win32ClientInfo[WIN32_CLIENT_INFO_LENGTH]; // User32 Client Info
        PVOID glDispatchTable[233];     // OpenGL
        ULONG_PTR glReserved1[29];      // OpenGL
        PVOID glReserved2;              // OpenGL
        PVOID glSectionInfo;            // OpenGL
        PVOID glSection;                // OpenGL
        PVOID glTable;                  // OpenGL
        PVOID glCurrentRC;              // OpenGL
        PVOID glContext;                // OpenGL
        ULONG LastStatusValue;
        UNICODE_STRING StaticUnicodeString;
        WCHAR StaticUnicodeBuffer[STATIC_UNICODE_BUFFER_LENGTH];
        PVOID DeallocationStack;
        PVOID TlsSlots[TLS_MINIMUM_AVAILABLE];
        LIST_ENTRY TlsLinks;
        PVOID Vdm;
        PVOID ReservedForNtRpc;
        PVOID DbgSsReserved[2];
        ULONG HardErrorsAreDisabled;
        PVOID Instrumentation[16];
        PVOID WinSockData;              // WinSock
        ULONG GdiBatchCount;
        BOOLEAN InDbgPrint;
        BOOLEAN FreeStackOnTermination;
        BOOLEAN HasFiberData;
        BOOLEAN IdealProcessor;
        ULONG Spare3;
        PVOID ReservedForPerf;
        PVOID ReservedForOle;
        ULONG WaitingOnLoaderLock;
        WX86THREAD Wx86Thread;
        PPVOID TlsExpansionSlots;
#if (defined(_IA64_) && !defined(PEBTEB_BITS)) \
    || ((defined(_IA64_) || defined(_X86_)) && defined(PEBTEB_BITS) && PEBTEB_BITS == 64)
        //
        // These are in the native ia64 TEB, and the TEB64 for ia64 and x86.
        //
        PVOID DeallocationBStore;
        PVOID BStoreLimit;
#endif
        LCID ImpersonationLocale;       // Current locale of impersonated user
        ULONG IsImpersonating;          // Thread impersonation status
        PVOID NlsCache;                 // NLS thread cache
        PVOID pShimData;                // Per thread data used in the shim
        ULONG HeapVirtualAffinity;
        HANDLE CurrentTransactionHandle;// reserved for TxF transaction context
        PTEB_ACTIVE_FRAME ActiveFrame;
    } TEB, *PTEB;

    typedef struct _SYSTEM_PROCESSOR_INFORMATION {
        USHORT ProcessorArchitecture;
        USHORT ProcessorLevel;
        USHORT ProcessorRevision;
        USHORT Reserved;
        ULONG ProcessorFeatureBits;
    } SYSTEM_PROCESSOR_INFORMATION, * PSYSTEM_PROCESSOR_INFORMATION;

    typedef enum _SYSTEM_INFORMATION_CLASS {
        SystemBasicInformation,
        SystemProcessorInformation,             // obsolete...delete
        SystemPerformanceInformation,
        SystemTimeOfDayInformation,
        SystemPathInformation,
        SystemProcessInformation,
        SystemCallCountInformation,
        SystemDeviceInformation,
        SystemProcessorPerformanceInformation,
        SystemFlagsInformation,
        SystemCallTimeInformation,
        SystemModuleInformation,
        SystemLocksInformation,
        SystemStackTraceInformation,
        SystemPagedPoolInformation,
        SystemNonPagedPoolInformation,
        SystemHandleInformation,
        SystemObjectInformation,
        SystemPageFileInformation,
        SystemVdmInstemulInformation,
        SystemVdmBopInformation,
        SystemFileCacheInformation,
        SystemPoolTagInformation,
        SystemInterruptInformation,
        SystemDpcBehaviorInformation,
        SystemFullMemoryInformation,
        SystemLoadGdiDriverInformation,
        SystemUnloadGdiDriverInformation,
        SystemTimeAdjustmentInformation,
        SystemSummaryMemoryInformation,
        SystemMirrorMemoryInformation,
        SystemPerformanceTraceInformation,
        SystemObsolete0,
        SystemExceptionInformation,
        SystemCrashDumpStateInformation,
        SystemKernelDebuggerInformation,
        SystemContextSwitchInformation,
        SystemRegistryQuotaInformation,
        SystemExtendServiceTableInformation,
        SystemPrioritySeperation,
        SystemVerifierAddDriverInformation,
        SystemVerifierRemoveDriverInformation,
        SystemProcessorIdleInformation,
        SystemLegacyDriverInformation,
        SystemCurrentTimeZoneInformation,
        SystemLookasideInformation,
        SystemTimeSlipNotification,
        SystemSessionCreate,
        SystemSessionDetach,
        SystemSessionInformation,
        SystemRangeStartInformation,
        SystemVerifierInformation,
        SystemVerifierThunkExtend,
        SystemSessionProcessInformation,
        SystemLoadGdiDriverInSystemSpace,
        SystemNumaProcessorMap,
        SystemPrefetcherInformation,
        SystemExtendedProcessInformation,
        SystemRecommendedSharedDataAlignment,
        SystemComPlusPackage,
        SystemNumaAvailableMemory,
        SystemProcessorPowerInformation,
        SystemEmulationBasicInformation,
        SystemEmulationProcessorInformation,
        SystemExtendedHandleInformation,
        SystemLostDelayedWriteInformation
    } SYSTEM_INFORMATION_CLASS;


    typedef struct _SYSTEM_BASIC_INFORMATION {
        ULONG Reserved;
        ULONG TimerResolution;
        ULONG PageSize;
        ULONG NumberOfPhysicalPages;
        ULONG LowestPhysicalPageNumber;
        ULONG HighestPhysicalPageNumber;
        ULONG AllocationGranularity;
        ULONG_PTR MinimumUserModeAddress;
        ULONG_PTR MaximumUserModeAddress;
        ULONG_PTR ActiveProcessorsAffinityMask;
        CCHAR NumberOfProcessors;
    } SYSTEM_BASIC_INFORMATION, * PSYSTEM_BASIC_INFORMATION;

    struct _ACTIVATION_CONTEXT;

    using PACTIVATION_CONTEXT = _ACTIVATION_CONTEXT*;

    typedef
        VOID(NTAPI* PACTIVATION_CONTEXT_NOTIFY_ROUTINE)(
            IN ULONG NotificationType,
            IN PACTIVATION_CONTEXT ActivationContext,
            IN const VOID* ActivationContextData,
            IN PVOID NotificationContext,
            IN PVOID NotificationData,
            IN OUT PBOOLEAN DisableThisNotification
            );

    typedef struct _ASSEMBLY_STORAGE_MAP_ENTRY {
        ULONG Flags;
        UNICODE_STRING DosPath;         // stored with a trailing unicode null
        HANDLE Handle;                  // open file handle on the directory to lock it down
    } ASSEMBLY_STORAGE_MAP_ENTRY, * PASSEMBLY_STORAGE_MAP_ENTRY;

#define ASSEMBLY_STORAGE_MAP_ASSEMBLY_ARRAY_IS_HEAP_ALLOCATED (0x00000001)

    typedef struct _ASSEMBLY_STORAGE_MAP {
        ULONG Flags;
        ULONG AssemblyCount;
        PASSEMBLY_STORAGE_MAP_ENTRY* AssemblyArray;
    } ASSEMBLY_STORAGE_MAP, * PASSEMBLY_STORAGE_MAP;

    typedef struct _ACTIVATION_CONTEXT {
        LONG RefCount;
        ULONG Flags;
        PVOID ActivationContextData;
        PACTIVATION_CONTEXT_NOTIFY_ROUTINE NotificationRoutine;
        PVOID NotificationContext;
        ULONG SentNotifications[8];
        ULONG DisabledNotifications[8];
        ASSEMBLY_STORAGE_MAP StorageMap;
        PASSEMBLY_STORAGE_MAP_ENTRY InlineStorageMapEntries[32];
    } ACTIVATION_CONTEXT;

    typedef struct _RTL_ACTIVATION_CONTEXT_STACK_FRAME {
        struct _RTL_ACTIVATION_CONTEXT_STACK_FRAME* Previous;
        PACTIVATION_CONTEXT ActivationContext;
        ULONG Flags;
    } RTL_ACTIVATION_CONTEXT_STACK_FRAME, * PRTL_ACTIVATION_CONTEXT_STACK_FRAME;

    typedef const struct _RTL_ACTIVATION_CONTEXT_STACK_FRAME* PCRTL_ACTIVATION_CONTEXT_STACK_FRAME;

#define RTL_CALLER_ALLOCATED_ACTIVATION_CONTEXT_STACK_FRAME_FORMAT_WHISTLER (1)

    typedef struct _RTL_CALLER_ALLOCATED_ACTIVATION_CONTEXT_STACK_FRAME {
        SIZE_T Size;
        ULONG Format;
        RTL_ACTIVATION_CONTEXT_STACK_FRAME Frame;
    } RTL_CALLER_ALLOCATED_ACTIVATION_CONTEXT_STACK_FRAME, * PRTL_CALLER_ALLOCATED_ACTIVATION_CONTEXT_STACK_FRAME;
}