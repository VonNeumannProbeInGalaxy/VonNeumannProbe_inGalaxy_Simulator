#include "Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>

_NPGS_BEGIN
_UTIL_BEGIN

void FLogger::Init()
{
	spdlog::set_pattern("%^[%T] %n: %v%$");

	_kCoreLogger   = spdlog::stdout_color_mt("Npgs");
	_kClientLogger = spdlog::stdout_color_mt("App");

	_kCoreLogger->set_level(spdlog::level::trace);
	_kClientLogger->set_level(spdlog::level::trace);
}

std::shared_ptr<spdlog::logger> FLogger::_kCoreLogger   = nullptr;
std::shared_ptr<spdlog::logger> FLogger::_kClientLogger = nullptr;

_UTIL_END
_NPGS_END
