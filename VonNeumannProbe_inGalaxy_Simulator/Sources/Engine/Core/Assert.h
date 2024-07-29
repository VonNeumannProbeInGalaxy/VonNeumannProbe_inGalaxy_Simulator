#pragma once

// Assert
// ------

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

#define NpgsStaticAssert(Expr, ...) static_assert(Expr, __VA_ARGS__)

#else

#define NpgsAssert(Expr, ...)
#define NpgsStaticAssert(Expr, ...)

#endif // NPGS_ASSERT_ENABLE
