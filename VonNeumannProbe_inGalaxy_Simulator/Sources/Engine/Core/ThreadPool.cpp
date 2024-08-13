#include "ThreadPool.h"

#include <future>
#include <type_traits>
#include <utility>

_NPGS_BEGIN

ThreadPool* ThreadPool::GetInstance(int ThreadCount) {
    std::call_once(_Once, Init, ThreadCount);
    return _Instance;
}

void ThreadPool::Init(int ThreadCount) {
    if (_Instance == nullptr) {
        _Instance = new ThreadPool(ThreadCount);
    }
}

void ThreadPool::Destroy() {
    if (_Instance != nullptr) {
        _Instance->Terminate();
        delete _Instance;
        _Instance = nullptr;
    }
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

ThreadPool::ThreadPool(std::size_t ThreadCount) : _Terminate(false) {
    for (std::size_t i = 0; i != ThreadCount; ++i) {
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
    }
}

ThreadPool* ThreadPool::_Instance = nullptr;
std::once_flag ThreadPool::_Once;

_NPGS_END
