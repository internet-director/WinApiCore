#pragma once
#include <core/config.h>

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