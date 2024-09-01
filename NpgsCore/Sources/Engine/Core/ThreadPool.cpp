#include "ThreadPool.h"

#define NOMINMAX
#include <Windows.h>

_NPGS_BEGIN

static int GetPhysicalCoreCount();
static void SetThreadAffinity(std::thread& Thread, std::size_t CoreId);

ThreadPool* ThreadPool::GetInstance() {
    std::call_once(_kOnce, Init);
    return _kInstance;
}

void ThreadPool::Init() {
    if (_kInstance == nullptr) {
        _kPhysicalCoreCount = GetPhysicalCoreCount();
        _kInstance = new ThreadPool();
    }
}

void ThreadPool::Destroy() {
    if (_kInstance != nullptr) {
        _kInstance->Terminate();
        delete _kInstance;
        _kInstance = nullptr;
    }
}

int ThreadPool::GetMaxThreadCount() {
    return _kPhysicalCoreCount;
}

void ThreadPool::Terminate() {
    {
        std::unique_lock<std::mutex> Mutex(_Mutex);
        _Terminate = true;
    }
    _Condition.notify_all();
    for (auto& Thread : _Threads) {
        if (Thread.joinable()) {
            Thread.join();
        }
    }
}

ThreadPool::ThreadPool() : _Terminate(false) {
    for (std::size_t i = 0; i != _kPhysicalCoreCount; ++i) {
        _Threads.emplace_back([this]() -> void {
            while (true) {
                std::function<void()> Task;
                {
                    std::unique_lock<std::mutex> Mutex(_Mutex);
                    _Condition.wait(Mutex, [this]() -> bool { return !_Tasks.empty() || _Terminate; });
                    if (_Terminate && _Tasks.empty()) {
                        return;
                    }

                    Task = std::move(_Tasks.front());
                    _Tasks.pop();
                }
                Task();
            }
        });

        SetThreadAffinity(_Threads.back(), i);
    }
}

ThreadPool* ThreadPool::_kInstance = nullptr;
std::once_flag ThreadPool::_kOnce;
int ThreadPool::_kPhysicalCoreCount = 0;

int GetPhysicalCoreCount() {
    DWORD Length = 0;
    GetLogicalProcessorInformationEx(RelationProcessorCore, nullptr, &Length);
    std::vector<std::uint8_t> Buffer(Length);
    auto* BufferPtr = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(Buffer.data());
    GetLogicalProcessorInformationEx(RelationProcessorCore, BufferPtr, &Length);

    int CoreCount = 0;
    while (Length > 0) {
        if (BufferPtr->Relationship == RelationProcessorCore) {
            ++CoreCount;
        }

        Length -= BufferPtr->Size;
        BufferPtr = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(reinterpret_cast<std::uint8_t*>(BufferPtr) + BufferPtr->Size);
    }

    return CoreCount;
}

void SetThreadAffinity(std::thread& Thread, std::size_t CoreId) {
    HANDLE Handle = Thread.native_handle();
    DWORD_PTR Mask = static_cast<DWORD_PTR>(Bit(CoreId * 2));
    SetThreadAffinityMask(Handle, Mask);
}

_NPGS_END
