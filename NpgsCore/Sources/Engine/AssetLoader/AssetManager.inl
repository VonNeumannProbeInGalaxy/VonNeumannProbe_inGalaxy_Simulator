#pragma once

#include "AssetManager.h"

_NPGS_BEGIN
_ASSET_BEGIN

template<typename AssetType>
requires Copyable<AssetType>
inline void AssetManager::AddAsset(const std::string& Name, const AssetType& Asset)
{
	_kAssets[Name] = std::unique_ptr<void, VoidDeleter>(std::make_unique<AssetType>(Asset).release(), VoidDeleter());
}

template<typename AssetType>
requires Copyable<AssetType>
inline AssetType* AssetManager::GetAsset(const std::string& Name)
{
	return static_cast<AssetType*>(_kAssets[Name].get());
}

inline void AssetManager::RemoveAsset(const std::string& Name)
{
	_kAssets.erase(Name);
}

inline void AssetManager::ClearAssets()
{
	_kAssets.clear();
}

_ASSET_END
_NPGS_END
