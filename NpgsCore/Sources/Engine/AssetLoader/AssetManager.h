#pragma once

#include <concepts>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
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

class TypeErasedDeleter
{
public:
	template <typename T>
	TypeErasedDeleter(T*)
		: _Deleter([](void* Ptr) -> void { delete static_cast<T*>(Ptr); })
	{
	}

	void operator()(void* Ptr) const
	{
		_Deleter(Ptr);
	}

private:
	void (*_Deleter)(void*);
};

template <typename AssetType>
concept MoveOnlyType = std::movable<AssetType> && !std::copyable<AssetType>;

class NPGS_API AssetManager
{
public:
	AssetManager() = default;
	~AssetManager() = default;

	template<typename AssetType>
	requires MoveOnlyType<AssetType>
	static void AddAsset(const std::string& Name, AssetType&& Asset);

	template<typename AssetType>
	requires MoveOnlyType<AssetType>
	static AssetType* GetAsset(const std::string& Name);

	template<typename AssetType>
	requires MoveOnlyType<AssetType>
	static std::vector<AssetType*> GetAssets();

	static void RemoveAsset(const std::string& Name);
	static void ClearAssets();

private:
	using ManagedAsset = std::unique_ptr<void, TypeErasedDeleter>;
	static std::unordered_map<std::string, ManagedAsset> _kAssets;
};

_ASSET_END
_NPGS_END

#include "AssetManager.inl"
