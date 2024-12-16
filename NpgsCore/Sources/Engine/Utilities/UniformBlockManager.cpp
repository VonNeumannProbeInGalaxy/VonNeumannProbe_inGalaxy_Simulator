#include "UniformBlockManager.h"

_NPGS_BEGIN
_UTIL_BEGIN

UniformBlockManager::UniformBlockManager(Asset::Shader* Shader, const std::string& BlockName, GLuint BindingPoint,
										 const std::vector<std::string>& MemberNames, Asset::Shader::UniformBlockLayout Layout)
	: _Shader(Shader), _BlockName(BlockName)
{
	if (!_Shader->HasUniformBlock(_BlockName))
	{
		_Shader->CreateUniformBlock(_BlockName, BindingPoint, MemberNames, Layout);
	}
}

UniformBlockManager::UniformBlockManager(Asset::Shader* Shader, const std::string& BlockName)
	: _Shader(Shader), _BlockName(BlockName)
{
	if (!_Shader->HasUniformBlock(_BlockName))
	{
		std::println("Warning: UniformBlock \"{}\" does not exist.", _BlockName);
	}
}

_UTIL_END
_NPGS_END
