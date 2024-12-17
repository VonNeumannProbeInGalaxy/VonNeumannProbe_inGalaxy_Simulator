#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

#include "Engine/Core/Base.h"

_NPGS_BEGIN

class NPGS_API ThreadPool
{
public:
	template <typename Func, typename... Args>
	auto Commit(Func&& Pred, Args&&... Params);

	void Terminate();
	void ChangeHyperThread();
	int GetMaxThreadCount() const;
	int GetPhysicalCoreCount() const;

	static ThreadPool* GetInstance();

private:
	explicit ThreadPool();
	ThreadPool(const ThreadPool&) = delete;
	ThreadPool(ThreadPool&&)      = delete;
	~ThreadPool();

	ThreadPool& operator=(const ThreadPool&) = delete;
	ThreadPool& operator=(ThreadPool&&)      = delete;

	void SetThreadAffinity(std::thread& Thread, std::size_t CoreId) const;

private:
	std::vector<std::thread>          _Threads;
	std::queue<std::function<void()>> _Tasks;
	std::mutex                        _Mutex;
	std::condition_variable           _Condition;
	bool                              _Terminate;
	int                               _kMaxThreadCount;
	int                               _kPhysicalCoreCount;
	int                               _kHyperThreadIndex;
};

_NPGS_END

#include "ThreadPool.inl"
