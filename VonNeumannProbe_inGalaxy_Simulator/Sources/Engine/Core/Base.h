#pragma once

// NPGS_API
// ---------
#ifdef _WIN64
#   ifdef _MSVC_LANG
#       ifdef NPGS_DLL_BUILD
#           ifdef NPGS_EXPORT
#               define NPGS_API __declspec(dllexport)
#           else
#               define NPGS_API __declspec(dllimport)
#           endif // NPGS_EXPORT
#       else
#           define NPGS_API
#       endif // NPGS_DLL_BUILD
#   else
#       error NPGS can only build on Visual Studio with MSVC
#   endif // _MSVC_LANG
#else
#   error NPGS only support 64-bit Windows
#endif // _WIN64

// Basic namespace defines
// -----------------------
#define _NPGS_BEGIN namespace Npgs {
#define _NPGS_END   }

// Bit operator function
// ---------------------
#define Bit(x) (1 << x)

// NpgsBind
// ---------
#define NpgsBind(Func) [this](auto&&... Args) -> decltype(auto) { \
    return this->Func(std::forward<decltype(Args)>(Args)...);     \
}