#pragma once

// NpgsAssert
// ----------

#ifdef _DEBUG
#define NPGS_ASSERT_ENABLE
#endif // _DEBUG

#ifdef NPGS_ASSERT_ENABLE
#include <iostream>

#define NpgsAssert(Expr, ...)                                                                                 \
if (!(Expr)) {                                                                                                \
    std::cerr << "Assertion failed: " << #Expr << " in " << __FILE__ << " at line " << __LINE__ << std::endl; \
    std::cerr << "Message: " << __VA_ARGS__ << std::endl;                                                     \
    __debugbreak();                                                                                           \
}

#else

#define NpgsAssert(Expr, ...)

#endif // NPGS_ASSERT_ENABLE
