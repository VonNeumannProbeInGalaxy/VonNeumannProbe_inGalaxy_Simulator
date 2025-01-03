#pragma once

#include "Mesh.h"

#include <algorithm>

_NPGS_BEGIN
_ASSET_BEGIN

NPGS_INLINE GLuint FMesh::GetVertexArray() const
{
	return _VertexArray;
}

NPGS_INLINE const std::vector<GLuint>& FMesh::GetIndices() const
{
	return _Indices;
}

NPGS_INLINE std::ptrdiff_t FMesh::GetTextureCount(const std::string& TypeName) const
{
	return std::count_if(_TextureInfos.begin(), _TextureInfos.end(), [&TypeName](const FTextureInfo& Info) -> bool
	{
		return Info.Handle->TypeName == TypeName;
	});
}

_ASSET_END
_NPGS_END
