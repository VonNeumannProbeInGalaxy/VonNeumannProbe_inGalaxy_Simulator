#pragma once

#include "Model.h"

_NPGS_BEGIN
_RUNTIME_BEGIN
_ASSET_BEGIN

NPGS_INLINE const std::vector<std::unique_ptr<FMesh>>& FModel::GetMeshes() const
{
    return _Meshes;
}

_ASSET_END
_RUNTIME_END
_NPGS_END
