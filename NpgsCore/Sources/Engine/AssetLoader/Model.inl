#include "Model.h"

_NPGS_BEGIN
_ASSET_BEGIN

inline const std::vector<std::unique_ptr<Mesh>>& Model::GetMeshes() const
{
	return _Meshes;
}

_ASSET_END
_NPGS_END