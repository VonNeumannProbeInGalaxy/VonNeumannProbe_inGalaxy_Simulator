#include "Mesh.h"

inline GLuint Mesh::GetVertexArray() const
{
	return _VertexArray;
}

inline const std::vector<GLuint>& Mesh::GetIndices() const
{
	return _Indices;
}
