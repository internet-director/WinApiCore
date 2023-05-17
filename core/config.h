#ifndef _CONFIG_HDR
#define _CONFIG_HDR
#include <Windows.h>

typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

#ifndef _WIN64
typedef uint32_t size_t;
#else
typedef uint64_t size_t;
#endif

// These macros must exactly match those in the Windows SDK's intsafe.h.
#define INT8_MIN         (-127i8 - 1)
#define INT16_MIN        (-32767i16 - 1)
#define INT32_MIN        (-2147483647i32 - 1)
#define INT64_MIN        (-9223372036854775807i64 - 1)
#define INT8_MAX         127i8
#define INT16_MAX        32767i16
#define INT32_MAX        2147483647i32
#define INT64_MAX        9223372036854775807i64
#define UINT8_MAX        0xffui8
#define UINT16_MAX       0xffffui16
#define UINT32_MAX       0xffffffffui32
#define UINT64_MAX       0xffffffffffffffffui64


/*-------------------------------custom macroses-------------------------------*/

#define END L"\r\n"
#define __countof(X) sizeof(X) / sizeof(X[0])

/*-----------------------------export dll settinds-----------------------------*/

#define MEM_EXPORT __declspec(dllexport)
#define THREAD_EXPORT __declspec(dllexport)
#define PROCESS_EXPORT __declspec(dllexport)

/*-------------------------------normal standart-------------------------------*/

namespace core {
	template <class, class>
	constexpr bool is_same_v = false;
	template <class _Ty>
	constexpr bool is_same_v<_Ty, _Ty> = true;

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

	template <class _Ty>
	constexpr remove_reference_t<_Ty>&& move(_Ty&& _Arg) noexcept {
		return static_cast<remove_reference_t<_Ty>&&>(_Arg);
	}
}

#endif