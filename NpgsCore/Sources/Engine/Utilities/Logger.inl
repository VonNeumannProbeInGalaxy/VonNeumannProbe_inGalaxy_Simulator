#pragma once

#include "Logger.h"

_NPGS_BEGIN
_UTIL_BEGIN

NPGS_INLINE std::shared_ptr<spdlog::logger>& Logger::GetCoreLogger()
{
	return _kCoreLogger;
}

NPGS_INLINE std::shared_ptr<spdlog::logger>& Logger::GetClientLogger()
{
	return _kClientLogger;
}

_UTIL_END
_NPGS_END
