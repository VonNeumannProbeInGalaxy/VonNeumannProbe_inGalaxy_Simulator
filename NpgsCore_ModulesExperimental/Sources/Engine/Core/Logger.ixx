module;

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include "Engine/Core/Base.h"

export module Core.Logger;

import <memory>;
import <string>;

_NPGS_BEGIN

export class Logger {
public:
    static void Init() {
        spdlog::set_pattern("%^[%T] %n: %v%$");

        _kCoreLogger = spdlog::stdout_color_mt("Npgs");
        _kClientLogger = spdlog::stdout_color_mt("App");

        _kCoreLogger->set_level(spdlog::level::trace);
        _kClientLogger->set_level(spdlog::level::trace);
    }

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

//#ifdef _DEBUG
//// Core logger
//// -----------
//export inline void NpgsCoreCritical(auto&&... Args) {
//    ::Npgs::Logger::GetCoreLogger()->critical(fmt::format(std::forward<decltype(Args)>(Args)...));
//}
//
//export inline void NpgsCoreError(auto&&... Args) {
//    ::Npgs::Logger::GetCoreLogger()->error(fmt::format(std::forward<decltype(Args)>(Args)...));
//}
//
//export inline void NpgsCoreInfo(auto&&... Args) {
//    ::Npgs::Logger::GetCoreLogger()->info(fmt::format(std::forward<decltype(Args)>(Args)...));
//}
//
//export inline void NpgsCoreTrace(auto&&... Args) {
//    ::Npgs::Logger::GetCoreLogger()->trace(fmt::format(std::forward<decltype(Args)>(Args)...));
//}
//
//export inline void NpgsCoreWarn(auto&&... Args) {
//    ::Npgs::Logger::GetCoreLogger()->warn(fmt::format(std::forward<decltype(Args)>(Args)...));
//}
//
//// Client logger
//// -------------
//export inline void NpgsCritical(auto&&... Args) {
//    ::Npgs::Logger::GetClientLogger()->critical(fmt::format(std::forward<decltype(Args)>(Args)...));
//}
//
//export inline void NpgsError(auto&&... Args) {
//    ::Npgs::Logger::GetClientLogger()->error(fmt::format(std::forward<decltype(Args)>(Args)...));
//}
//
//export inline void NpgsInfo(auto&&... Args) {
//    ::Npgs::Logger::GetClientLogger()->info(fmt::format(std::forward<decltype(Args)>(Args)...));
//}
//
//export inline void NpgsTrace(auto&&... Args) {
//    ::Npgs::Logger::GetClientLogger()->trace(fmt::format(std::forward<decltype(Args)>(Args)...));
//}
//
//export inline void NpgsWarn(auto&&... Args) {
//    ::Npgs::Logger::GetClientLogger()->warn(fmt::format(std::forward<decltype(Args)>(Args)...));
//}
//
//#else
//
//export inline void NpgsCoreCritical(...) {}
//export inline void NpgsCoreError(...) {}
//export inline void NpgsCoreInfo(...) {}
//export inline void NpgsCoreTrace(...) {}
//export inline void NpgsCoreWarn(...) {}
//
//export inline void NpgsCritical(...) {}
//export inline void NpgsError(...) {}
//export inline void NpgsInfo(...) {}
//export inline void NpgsTrace(...) {}
//export inline void NpgsWarn(...) {}
//
//#endif // _DEBUG
