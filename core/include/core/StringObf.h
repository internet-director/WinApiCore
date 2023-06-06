#pragma once
#include <core/config.h>
#include <core/hash.h>

class StringObf
{
};


struct conststr
{
	const char* const p;
	size_t sz;
	template<size_t N>
	constexpr conststr(const char(&a)[N]) : p(a), sz(N - 1) {}
};

template<typename T, size_t size>
struct Array {
	T arr[size];
	constexpr Array() = default;
	constexpr T& operator[](size_t index) noexcept {
		return arr[index];
	}
	constexpr const T& operator[](size_t index) const noexcept {
		return arr[index];
	}
};

template<conststr str, size_t sz = (str.sz + 2) / 3>
constexpr Array<uint32_t, sz> check() {
	size_t lsz = sz;
	Array<uint32_t, sz> arr;
	for (size_t i = 0; i < sz; i++) {
		if (i == sz - 1) lsz = str.sz % 3;
		arr[i] = core::hash32::calculate(str.p + i * sz, lsz);
	}
	return arr;
}


constexpr auto hash = check<conststr("tesssst")>();

static_assert(hash[0] == core::hash32::calculate("tes", 3), "ERR");