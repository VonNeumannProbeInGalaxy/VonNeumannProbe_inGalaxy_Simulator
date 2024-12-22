#pragma once

#include "Engine/Core/Base.h"

_NPGS_BEGIN

template <typename EnumType>
constexpr EnumType operator|(EnumType Lhs, EnumType Rhs);

_UTIL_BEGIN

bool Equal(double Lhs, double Rhs);
bool Equal(float Lhs, float Rhs);

_UTIL_END
_NPGS_END

#include "Utilities.inl"
