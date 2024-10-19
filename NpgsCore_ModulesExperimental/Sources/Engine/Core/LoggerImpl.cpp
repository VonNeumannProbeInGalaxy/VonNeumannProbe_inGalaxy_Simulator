module;

#include "Engine/Core/Base.h"

module Core.Logger;

_NPGS_BEGIN

std::shared_ptr<spdlog::logger> Logger::_kCoreLogger   = nullptr;
std::shared_ptr<spdlog::logger> Logger::_kClientLogger = nullptr;

_NPGS_END
