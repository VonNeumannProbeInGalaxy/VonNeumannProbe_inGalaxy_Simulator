#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "Engine/Core/Base.h"

_NPGS_BEGIN

class ThreadPool {
public:
    static ThreadPool* GetInstance(int ThreadCount);
    static void Init(int ThreadCount);
    static void Destroy();

    template <typename Func, typename... Args>
    decltype(auto) Commit(Func&& Pred, Args&&... Params);

    void Terminate();

private:
    ThreadPool(std::size_t ThreadCount);
    ~ThreadPool() {}

private:
    static ThreadPool*                _Instance;
    static std::once_flag             _Once;
    std::vector<std::thread>          _Threads;
    std::queue<std::function<void()>> _Tasks;
    std::mutex                        _Mutex;
    std::condition_variable           _Condition;
    bool                              _Terminate;
};

_NPGS_END
