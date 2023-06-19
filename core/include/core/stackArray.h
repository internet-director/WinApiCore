#pragma once
#include <core/mem.h>

namespace core {
	template <typename T, size_t sz>
	struct STACK_EXPORT Array {
		T arr[sz];

		constexpr Array() = default;
		constexpr void fill(const T& obj) {
			core::fill(arr, obj, sz);
		}
		constexpr bool empty() const noexcept {
			return false;
		}
		constexpr size_t size() const noexcept {
			return sz;
		}
		constexpr T& operator[](size_t index) noexcept {
			return arr[index];
		}
		constexpr const T& operator[](size_t index) const noexcept {
			return arr[index];
		}
	};
}

namespace core {
	struct ConstexprString
	{
		const char* const p;
		size_t sz;
		template<size_t N>
		constexpr ConstexprString(const char(&a)[N]) : p(a), sz(N - 1) {}
	};
}