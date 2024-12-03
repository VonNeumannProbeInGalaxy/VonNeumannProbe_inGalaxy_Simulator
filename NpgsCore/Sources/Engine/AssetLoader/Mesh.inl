#include "Mesh.h"

_NPGS_BEGIN
_ASSET_BEGIN

inline GLuint Mesh::GetVertexArray() const
{
	return _VertexArray;
}

inline const std::vector<GLuint>& Mesh::GetIndices() const
{
	return _Indices;
}

_ASSET_END
_NPGS_END
