#pragma once
#include <Windows.h>
#include <tlhelp32.h>
#include <LMaccess.h>
#include <LMalert.h>
#include <lmcons.h>
#include <LM.h>
#include <winternl.h>
#include <wdmguid.h>
#include <winnt.h>
#include <DbgHelp.h>
#include "ntdll_header.h"


namespace core {
	typedef signed char        int8_t;
	typedef short              int16_t;
	typedef int                int32_t;
	typedef long long          int64_t;
	typedef unsigned char      uint8_t;
	typedef unsigned short     uint16_t;
	typedef unsigned int       uint32_t;
	typedef unsigned long long uint64_t;
}

typedef core::int8_t	int8_t;
typedef core::int16_t	int16_t;
typedef core::int32_t	int32_t;
typedef core::int64_t	int64_t;
typedef core::uint8_t	uint8_t;
typedef core::uint16_t	uint16_t;
typedef core::uint32_t	uint32_t;
typedef core::uint64_t	uint64_t;

#ifndef _WIN64
typedef core::uint32_t size_t;
#else
typedef core::uint64_t size_t;
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
#define _INLINE_VAR inline
#define __countof(X) sizeof(X) / sizeof(X[0])


namespace core {
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
	_INLINE_VAR constexpr bool _Always_false = false;

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