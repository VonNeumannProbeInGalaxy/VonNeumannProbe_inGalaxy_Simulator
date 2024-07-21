#include "ThreadPool.h"

_NPGS_BEGIN

ThreadPool* ThreadPool::_Instance = nullptr;
std::once_flag ThreadPool::_Once;

_NPGS_END
