#pragma once

#include "ThreadPool.h"

_NPGS_BEGIN

template <typename Func, typename... Args>
inline auto FThreadPool::Commit(Func&& Pred, Args&&... Params)
{
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

NPGS_INLINE void FThreadPool::ChangeHyperThread()
{
	_kHyperThreadIndex = 1 - _kHyperThreadIndex;
}

NPGS_INLINE int FThreadPool::GetMaxThreadCount() const
{
	return _kMaxThreadCount;
}

_NPGS_END
