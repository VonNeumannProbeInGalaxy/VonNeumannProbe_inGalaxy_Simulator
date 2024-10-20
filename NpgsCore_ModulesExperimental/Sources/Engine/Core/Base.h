#pragma once

#include <utility>

// NPGS_API
// ---------
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
#   else
#       error NPGS can only build on Visual Studio with MSVC
#   endif // _MSVC_LANG
#else
#   error NPGS only support 64-bit Windows
#endif // _WIN64

// Basic namespace defines
// -----------------------
#define _NPGS_BEGIN namespace Npgs {
#define _NPGS_END }
#define _ASTRO_BEGIN namespace Astro {
#define _ASTRO_END }
#define _MODULES_BEGIN namespace Modules {
#define _MODULES_END }
#define _ASSETS_BEGIN namespace Assets {
#define _ASSETS_END }
#define _THIRD_PARTY_BEGIN namespace ThirdParty {
#define _THIRD_PARTY_END }

// Bit operator function
// ---------------------
#define Bit(x) (1ULL << x)

// NpgsBind
// --------
#define NpgsBindMemberFunc(Func) [this](auto&&... Args) -> decltype(auto) { \
    return this->Func(std::forward<decltype(Args)>(Args)...);               \
}

#define NpgsBind(Func) [](auto&&... Args) -> decltype(auto) { \
    return Func(std::forward<decltype(Args)>(Args)...);       \
}

// NpgsLogger
// ----------
#ifdef ENABLE_LOGGER
// Core logger
// -----------
#define NpgsCoreCritical(...) ::Npgs::Logger::GetCoreLogger()->critical(__VA_ARGS__)
#define NpgsCoreError(...)    ::Npgs::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define NpgsCoreInfo(...)     ::Npgs::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define NpgsCoreTrace(...)    ::Npgs::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#define NpgsCoreWarn(...)     ::Npgs::Logger::GetCoreLogger()->warn(__VA_ARGS__)

// Client logger
// -------------
#define NpgsCritical(...)     ::Npgs::Logger::GetClientLogger()->critical(__VA_ARGS__)
#define NpgsError(...)        ::Npgs::Logger::GetClientLogger()->error(__VA_ARGS__)
#define NpgsInfo(...)         ::Npgs::Logger::GetClientLogger()->info(__VA_ARGS__)
#define NpgsTrace(...)        ::Npgs::Logger::GetClientLogger()->trace(__VA_ARGS__)
#define NpgsWarn(...)         ::Npgs::Logger::GetClientLogger()->warn(__VA_ARGS__)

#else

#define NpgsCoreCritical(...) static_cast<void>(0)
#define NpgsCoreError(...)    static_cast<void>(0)
#define NpgsCoreInfo(...)     static_cast<void>(0)
#define NpgsCoreTrace(...)    static_cast<void>(0)
#define NpgsCoreWarn(...)     static_cast<void>(0)

#define NpgsCritical(...)     static_cast<void>(0)
#define NpgsError(...)        static_cast<void>(0)
#define NpgsInfo(...)         static_cast<void>(0)
#define NpgsTrace(...)        static_cast<void>(0)
#define NpgsWarn(...)         static_cast<void>(0)

#endif // !_DEBUG
