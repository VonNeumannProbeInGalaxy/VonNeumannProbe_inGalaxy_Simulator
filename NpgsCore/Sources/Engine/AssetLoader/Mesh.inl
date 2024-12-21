#pragma once

#include "Mesh.h"

#include <algorithm>

_NPGS_BEGIN
_ASSET_BEGIN

NPGS_INLINE GLuint Mesh::GetVertexArray() const
{
	return _VertexArray;
}

NPGS_INLINE const std::vector<GLuint>& Mesh::GetIndices() const
{
	return _Indices;
}

NPGS_INLINE std::ptrdiff_t Mesh::GetTextureCount(const std::string& TypeName) const
{
	return std::count_if(_TextureInfos.begin(), _TextureInfos.end(), [&TypeName](const TextureInfo& Info) -> bool
	{
		return Info.Handle->TypeName == TypeName;
	});
}

_ASSET_END
_NPGS_END
