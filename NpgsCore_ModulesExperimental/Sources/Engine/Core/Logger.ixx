module;

#include <spdlog/spdlog.h>
#include "Engine/Core/Base.h"

export module Core.Logger;

import <memory>;

_NPGS_BEGIN

export class Logger {
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
