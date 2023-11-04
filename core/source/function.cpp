#include "pch.h"
#include <core/function.h>

char const* core::bad_function_call::what() const noexcept {
    return "bad function call";
}