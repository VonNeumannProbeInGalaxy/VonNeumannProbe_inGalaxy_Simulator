#pragma once

#include "UniformBlockManager.h"

_NPGS_BEGIN
_UTIL_BEGIN

template<typename T>
NPGS_INLINE Asset::Shader::UniformBlockUpdater<T> UniformBlockManager::Get(const std::string& MemberName)
{
	return _Shader->GetUniformBlockUpdater<T>(_BlockName, MemberName);
}

_UTIL_END
_NPGS_END
