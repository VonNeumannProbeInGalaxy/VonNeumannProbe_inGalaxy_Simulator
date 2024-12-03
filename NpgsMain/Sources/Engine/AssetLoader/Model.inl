#include "Model.h"

inline const std::vector<std::unique_ptr<Mesh>>& Model::GetMeshes() const
{
	return _Meshes;
}