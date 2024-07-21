#pragma once

#include <memory>
#include <spdlog/spdlog.h>

#include "Engine/Core/Base.h"

// Macro function use pascal style
// Those macro make function call easily
// -------------------------------------
#ifdef _DEBUG
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

#define NpgsCoreCritical(...)
#define NpgsCoreError(...)
#define NpgsCoreInfo(...)
#define NpgsCoreTrace(...)
#define NpgsCoreWarn(...)

#define NpgsCritical(...)
#define NpgsError(...)
#define NpgsInfo(...)
#define NpgsTrace(...)
#define NpgsWarn(...)

#endif // !_DEBUG

_NPGS_BEGIN

class Logger {
public:
    static void Init();

    static std::shared_ptr<spdlog::logger>& GetCoreLogger() {
        return _kCoreLogger;
    }

    static std::shared_ptr<spdlog::logger>& GetClientLogger() {
        return _kClientLogger;
    }

private:
    Logger() {};
    ~Logger() = default;

    static std::shared_ptr<spdlog::logger> _kCoreLogger;
    static std::shared_ptr<spdlog::logger> _kClientLogger;
};

_NPGS_END
