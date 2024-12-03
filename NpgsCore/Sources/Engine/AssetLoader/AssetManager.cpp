#include "AssetManager.h"

#include "Engine/Core/Assert.h"

_NPGS_BEGIN
_ASSET_BEGIN

std::unordered_map<std::string, std::shared_ptr<void>> AssetManager::_kAssets;

_ASSET_END
_NPGS_END
