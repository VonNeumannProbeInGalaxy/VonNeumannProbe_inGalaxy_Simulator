#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_UTIL_BEGIN

class NPGS_API Logger
{
public:
	static void Init();
	static std::shared_ptr<spdlog::logger>& GetCoreLogger();
	static std::shared_ptr<spdlog::logger>& GetClientLogger();

private:
	Logger() {};
	~Logger() = default;

	static std::shared_ptr<spdlog::logger> _kCoreLogger;
	static std::shared_ptr<spdlog::logger> _kClientLogger;
};

_UTIL_END
_NPGS_END

#include "Logger.inl"

#ifdef ENABLE_CONSOLE_LOGGER
// Core logger
// -----------
#define NpgsCoreCritical(...) ::Npgs::Util::Logger::GetCoreLogger()->critical(__VA_ARGS__)
#define NpgsCoreError(...)    ::Npgs::Util::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define NpgsCoreInfo(...)     ::Npgs::Util::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define NpgsCoreTrace(...)    ::Npgs::Util::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#define NpgsCoreWarn(...)     ::Npgs::Util::Logger::GetCoreLogger()->warn(__VA_ARGS__)

// Client logger
// -------------
#define NpgsCritical(...)     ::Npgs::Util::Logger::GetClientLogger()->critical(__VA_ARGS__)
#define NpgsError(...)        ::Npgs::Util::Logger::GetClientLogger()->error(__VA_ARGS__)
#define NpgsInfo(...)         ::Npgs::Util::Logger::GetClientLogger()->info(__VA_ARGS__)
#define NpgsTrace(...)        ::Npgs::Util::Logger::GetClientLogger()->trace(__VA_ARGS__)
#define NpgsWarn(...)         ::Npgs::Util::Logger::GetClientLogger()->warn(__VA_ARGS__)

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
