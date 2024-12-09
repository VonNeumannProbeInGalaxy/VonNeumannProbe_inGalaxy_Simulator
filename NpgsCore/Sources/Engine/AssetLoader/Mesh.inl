#pragma once

#include "Mesh.h"

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

_ASSET_END
_NPGS_END
