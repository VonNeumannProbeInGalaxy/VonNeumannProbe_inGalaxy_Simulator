#pragma once

#include "AssetManager.h"

_NPGS_BEGIN
_ASSET_BEGIN

template<typename AssetType>
requires MoveOnlyType<AssetType>
inline void AssetManager::AddAsset(const std::string& Name, AssetType&& Asset)
{
	_kAssets.emplace(Name, ManagedAsset(
		static_cast<void*>(new AssetType(std::move(Asset))),
		TypeErasedDeleter(static_cast<AssetType*>(nullptr))
	));
}

template<typename AssetType>
requires MoveOnlyType<AssetType>
inline AssetType* AssetManager::GetAsset(const std::string& Name)
{
	auto it = _kAssets.find(Name);
	if (it != _kAssets.end())
	{
		return static_cast<AssetType*>(it->second.get());
	}

	return nullptr;
}

template<typename AssetType>
requires MoveOnlyType<AssetType>
inline std::vector<AssetType*> AssetManager::GetAssets()
{
	std::vector<AssetType*> Result;
	for (const auto& [Name, Asset] : _kAssets)
	{
		if (auto* AssetPtr = dynamic_cast<AssetType*>(Asset.get()))
		{
			Result.emplace_back(AssetPtr);
		}
	}

	return Result;
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
