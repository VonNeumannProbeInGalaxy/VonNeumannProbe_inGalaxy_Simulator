#include "Utilities.h"

#include <cmath>
#include <limits>

_NPGS_BEGIN
_UTIL_BEGIN

bool Equal(double Lhs, double Rhs) {
    return std::abs(Lhs - Rhs) <= std::numeric_limits<double>::epsilon();
}

bool Equal(float Lhs, float Rhs) {
    return std::abs(Lhs - Rhs) <= std::numeric_limits<float>::epsilon();
}

_UTIL_END
_NPGS_END
