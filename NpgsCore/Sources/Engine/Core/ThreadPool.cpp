#include "ThreadPool.h"

_NPGS_BEGIN

ThreadPool* ThreadPool::GetInstance() {
    return _kInstance;
}

ThreadPool* ThreadPool::GetInstance(int ThreadCount) {
    std::call_once(_kOnce, Init, ThreadCount);
    return _kInstance;
}

void ThreadPool::Init(int ThreadCount) {
    if (_kInstance == nullptr) {
        _kInstance = new ThreadPool(ThreadCount);
    }
}

void ThreadPool::Destroy() {
    if (_kInstance != nullptr) {
        _kInstance->Terminate();
        delete _kInstance;
        _kInstance = nullptr;
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

ThreadPool* ThreadPool::_kInstance = nullptr;
std::once_flag ThreadPool::_kOnce;

_NPGS_END
