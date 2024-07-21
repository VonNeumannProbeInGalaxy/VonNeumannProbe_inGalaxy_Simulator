#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "Engine/Core/Base.h"

_NPGS_BEGIN

class ThreadPool {
public:
    static ThreadPool* GetInstance(int ThrdAmount) {
        std::call_once(_Once, Init, ThrdAmount);
        return _Instance;
    }

    static void Init(int ThrdAmount) {
        if (_Instance == nullptr) {
            _Instance = new ThreadPool(ThrdAmount);
        }
    }

    static void Destroy() {
        if (_Instance != nullptr) {
            _Instance->Terminate();
            delete _Instance;
            _Instance = nullptr;
        }
    }

    template <typename Func, typename... Args>
    decltype(auto) Commit(Func&& Pred, Args&&... Params) {
        using ReturnType = std::invoke_result_t<Func, Args...>;
        auto Task = std::make_shared<std::packaged_task<ReturnType()>>(std::bind(std::forward<Func>(Pred), std::forward<Args>(Params)...));
        std::future<ReturnType> Future = Task->get_future();
        {
            std::unique_lock<std::mutex> Mutex(_Mutex);
            _Tasks.emplace([Task]() -> void { (*Task)(); });
        }
        _Condition.notify_one();
        return Future;
    }

    void Terminate() {
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

private:
    ThreadPool(std::size_t ThrdAmount) : _Terminate(false) {
        for (std::size_t i = 0; i != ThrdAmount; ++i) {
            _Threads.emplace_back(
                [this]() -> void {
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
                }
            );
        }
    }

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
