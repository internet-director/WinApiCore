#pragma once
#include <core/config.h>

struct Exception {
	explicit constexpr Exception() = default;
	explicit constexpr Exception(const char* message): message{message}  {}

	const char* message = nullptr;
};