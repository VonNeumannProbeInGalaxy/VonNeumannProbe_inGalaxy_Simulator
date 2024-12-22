#pragma once

#include "Utilities.h"

_NPGS_BEGIN

template <typename EnumType>
inline constexpr EnumType operator|(EnumType Lhs, EnumType Rhs)
{
	return static_cast<EnumType>(
		static_cast<std::underlying_type_t<EnumType>>(Lhs) |
		static_cast<std::underlying_type_t<EnumType>>(Rhs)
	);
}

_UTIL_BEGIN

_UTIL_END
_NPGS_END
