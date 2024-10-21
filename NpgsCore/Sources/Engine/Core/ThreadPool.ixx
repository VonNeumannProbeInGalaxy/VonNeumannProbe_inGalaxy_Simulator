module;

#include "Engine/Core/Base.h"

export module Core.ThreadPool;

import <condition_variable>;
import <functional>;
import <future>;
import <memory>;
import <mutex>;
import <queue>;
import <thread>;
import <type_traits>;
import <utility>;
import <vector>;

_NPGS_BEGIN

export class ThreadPool {
public:
    template <typename Func, typename... Args>
    auto Commit(Func&& Pred, Args&&... Params) {
        using ReturnType = std::invoke_result_t<Func, Args...>;
        auto Task = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<Func>(Pred), std::forward<Args>(Params)...));
        std::future<ReturnType> Future = Task->get_future();
        {
            std::unique_lock<std::mutex> Mutex(_Mutex);
            _Tasks.emplace([Task]() -> void { (*Task)(); });
        }
        _Condition.notify_one();
        return Future;
    }

    void Terminate();

    static ThreadPool* GetInstance();
    static void Init();
    static void Destroy();
    static void ChangeHyperThread();
    static int  GetMaxThreadCount();
    static int  GetPhysicalCoreCount();

private:
    explicit ThreadPool();
    ~ThreadPool() {}

    static void SetThreadAffinity(std::thread& Thread, std::size_t CoreId);

private:
    std::vector<std::thread>          _Threads;
    std::queue<std::function<void()>> _Tasks;
    std::mutex                        _Mutex;
    std::condition_variable           _Condition;
    bool                              _Terminate;

    static ThreadPool*                _kInstance;
    static std::once_flag             _kOnce;
    static int                        _kMaxThreadCount;
    static int                        _kPhysicalCoreCount;
    static int                        _kHyperThreadIndex;
};

_NPGS_END
