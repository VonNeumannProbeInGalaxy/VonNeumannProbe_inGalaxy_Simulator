#pragma once

#include "AssetManager.h"

_NPGS_BEGIN
_ASSET_BEGIN

template<typename AssetType>
requires MoveOnlyType<AssetType>
NPGS_INLINE void AssetManager::AddAsset(const std::string& Name, AssetType&& Asset)
{
	_kAssets[Name] = std::unique_ptr<void, VoidDeleter>(new AssetType(std::forward<AssetType>(Asset)), VoidDeleter());
}

template<typename AssetType>
requires MoveOnlyType<AssetType>
NPGS_INLINE AssetType* AssetManager::GetAsset(const std::string& Name)
{
	return static_cast<AssetType*>(_kAssets[Name].get());
}

NPGS_INLINE void AssetManager::RemoveAsset(const std::string& Name)
{
	_kAssets.erase(Name);
}

NPGS_INLINE void AssetManager::ClearAssets()
{
	_kAssets.clear();
}

_ASSET_END
_NPGS_END
