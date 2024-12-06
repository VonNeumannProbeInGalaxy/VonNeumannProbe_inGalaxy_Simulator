#pragma once

#include <concepts>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Engine/AssetLoader/Csv.hpp"
#include "Engine/AssetLoader/GetAssetFilepath.h"
#include "Engine/AssetLoader/Mesh.h"
#include "Engine/AssetLoader/Model.h"
#include "Engine/AssetLoader/Shader.h"
#include "Engine/AssetLoader/Texture.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_ASSET_BEGIN

struct VoidDeleter
{
	template <typename T>
	void operator()(T* Pointer)
	{
		delete Pointer;
	}
};

template <typename AssetType>
concept Copyable = std::copyable<AssetType>;

class NPGS_API AssetManager
{
public:
	AssetManager() = default;
	~AssetManager() = default;

	template<typename AssetType>
	requires Copyable<AssetType>
	static void AddAsset(const std::string& Name, const AssetType& Asset);

	template<typename AssetType>
	requires Copyable<AssetType>
	static AssetType* GetAsset(const std::string& Name);

	template<typename AssetType>
	requires Copyable<AssetType>
	static std::vector<std::unique_ptr<AssetType>> GetAssets();

	static void RemoveAsset(const std::string& Name);

	static void ClearAssets();

private:
	static std::unordered_map<std::string, std::unique_ptr<void, VoidDeleter>> _kAssets;
};

_ASSET_END
_NPGS_END

#include "AssetManager.inl"
