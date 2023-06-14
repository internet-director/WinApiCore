#pragma once
#include <core/config.h>
#include <core/hash.h>
#include <core/stackArray.h>

template<core::ConstexprString str, size_t sz = (str.sz + 2) / 3>
constexpr core::Array<uint32_t, sz> check() {
	size_t lsz = sz;
	core::Array<uint32_t, sz> arr;
	for (size_t i = 0; i < sz; i++) {
		if (i == sz - 1) lsz = str.sz % 3;
		arr[i] = core::hash32::calculate(str.p + i * sz, lsz);
	}
	return arr;
}


constexpr auto hash = check<core::ConstexprString("tesssst")>();

static_assert(hash[0] == core::hash32::calculate("tes", 3), "ERR");