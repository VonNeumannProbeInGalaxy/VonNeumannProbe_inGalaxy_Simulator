#include "Utilities.h"
#include <boost/multiprecision/cpp_int.hpp>

_NPGS_BEGIN

inline float ConvertToFloat(auto& MultiPrecision) {
    return MultiPrecision.convert_to<float>();
}

_NPGS_END
