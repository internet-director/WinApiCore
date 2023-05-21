#pragma once
#include <core/config.h>

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

	template <bool>
	struct _Select { // Select between aliases that extract either their first or second parameter
		template <class _Ty1, class>
		using _Apply = _Ty1;
	};

	template <>
	struct _Select<false> {
		template <class, class _Ty2>
		using _Apply = _Ty2;
	};


	template <class _Ty>
	constexpr remove_reference_t<_Ty>&& move(_Ty&& _Arg) noexcept {
		return static_cast<remove_reference_t<_Ty>&&>(_Arg);
	}

	template<typename T> void swap(T& t1, T& t2) noexcept {
		T temp = core::move(t1);
		t1 = core::move(t2);
		t2 = core::move(temp);
	}
}