#include "AssetManager.h"

#include "Engine/Core/Assert.h"

_NPGS_BEGIN
_ASSET_BEGIN

template<typename AssetType>
requires MoveOnlyType<AssetType>
std::vector<std::unique_ptr<AssetType>> AssetManager::GetAssets()
{
	std::vector<std::unique_ptr<AssetType>> Assets;
	for (auto& Asset : _kAssets)
	{
		Assets.emplace_back(std::make_unique<AssetType>(*static_cast<AssetType*>(Asset.second.get())));
	}

	return Assets;
}

std::unordered_map<std::string, std::unique_ptr<void, VoidDeleter>> AssetManager::_kAssets;

_ASSET_END
_NPGS_END
