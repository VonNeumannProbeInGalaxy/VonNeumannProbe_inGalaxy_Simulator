#pragma once

#include <utility>

// NPGS_API NPGS_INLINE
// --------------------
#ifndef _DEBUG
#define RELEASE_FORCE_INLINE
#endif // !_DEBUG

// #define MSVC_ATTRIBUTE_FORCE_INLINE

#ifdef _WIN64
#   ifdef _MSVC_LANG
#       ifdef NPGS_DLL_EXPORT
#           ifdef NPGS_DLL_EXPORT_BUILD
#               define NPGS_API __declspec(dllexport)
#           else
#               define NPGS_API __declspec(dllimport)
#           endif // NPGS_EXPORT
#       else
#           define NPGS_API
#       endif // NPGS_DLL_BUILD
#       ifdef RELEASE_FORCE_INLINE
#           define NPGS_INLINE __forceinline
#       else
#           ifdef MSVC_ATTRIBUTE_FORCE_INLINE
#               define NPGS_INLINE [[msvc::forceinline]] inline
#           else
#               define NPGS_INLINE inline
#           endif // MSVC_ATTRIBUTE_FORCE_INLINE
#       endif // RELEASE_FORCE_INLINE
#   else
#       error NPGS can only build on Visual Studio with MSVC
#   endif // _MSVC_LANG
#else
#   error NPGS only support 64-bit Windows
#endif // _WIN64

// Basic namespace defines
// -----------------------
#define _ASSET_BEGIN namespace Asset {
#define _ASSET_END }
#define _ASTRO_BEGIN namespace Astro {
#define _ASTRO_END }
#define _INTELLI_BEGIN namespace Intelli {
#define _INTELLI_END }
#define _MODULE_BEGIN namespace Module {
#define _MODULE_END }
#define _NPGS_BEGIN namespace Npgs {
#define _NPGS_END }
#define _THIRD_PARTY_BEGIN namespace ThirdParty {
#define _THIRD_PARTY_END }
#define _UTIL_BEGIN namespace Util {
#define _UTIL_END }

// Bit operator function
// ---------------------
#define Bit(x) (1ULL << x)

// NpgsBind
// --------
#define NpgsBindMemberFunc(Func) [this](auto&&... Args) -> decltype(auto) \
{                                                                         \
    return this->Func(std::forward<decltype(Args)>(Args)...);             \
}

#define NpgsBind(Func) [](auto&&... Args) -> decltype(auto) \
{                                                           \
    return Func(std::forward<decltype(Args)>(Args)...);     \
}
