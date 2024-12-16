#pragma once

#include "Engine/AssetLoader/Shader.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_UTIL_BEGIN

class UniformBlockManager
{
public:
	UniformBlockManager(Asset::Shader* Shader, const std::string& BlockName, GLuint BindingPoint,
						const std::vector<std::string>& MemberNames,
						Asset::Shader::UniformBlockLayout Layout = Asset::Shader::UniformBlockLayout::kShared);

	UniformBlockManager(Asset::Shader* Shader, const std::string& BlockName);

	template<typename T>
	Asset::Shader::UniformBlockUpdater<T> Get(const std::string& MemberName);

private:
	Asset::Shader* _Shader;
	std::string _BlockName;
};

_UTIL_END
_NPGS_END

#include "UniformBlockManager.inl"
