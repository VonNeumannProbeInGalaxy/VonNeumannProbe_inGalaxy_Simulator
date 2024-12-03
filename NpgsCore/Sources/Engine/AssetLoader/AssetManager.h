#pragma once

#include <concepts>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Engine/AssetLoader/Csv.hpp"
#include "Engine/AssetLoader/GetAssetFilepath.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_ASSET_BEGIN

template <typename AssetType>
concept Copyable = std::copyable<AssetType>;

class NPGS_API AssetManager
{
public:
	AssetManager() = default;
	~AssetManager() = default;

	template<typename AssetType>
	requires Copyable<AssetType>
	static void AddAsset(const std::string& Name, const std::shared_ptr<AssetType>& Asset)
	{
		_kAssets[Name] = Asset;
	}

	template<typename AssetType>
	requires Copyable<AssetType>
	static std::shared_ptr<AssetType> GetAsset(const std::string& Name)
	{
		return std::static_pointer_cast<AssetType>(_kAssets[Name]);
	}

	template<typename AssetType>
	requires Copyable<AssetType>
	static std::vector<std::shared_ptr<AssetType>> GetAssets()
	{
		std::vector<std::shared_ptr<AssetType>> Assets;
		for (auto& Asset : _kAssets)
		{
			Assets.emplace_back(std::static_pointer_cast<AssetType>(Asset.second));
		}

		return Assets;
	}

	static void RemoveAsset(const std::string& Name)
	{
		_kAssets.erase(Name);
	}

	static void ClearAssets()
	{
		_kAssets.clear();
	}

private:
	static std::unordered_map<std::string, std::shared_ptr<void>> _kAssets;
};

_ASSET_END
_NPGS_END
