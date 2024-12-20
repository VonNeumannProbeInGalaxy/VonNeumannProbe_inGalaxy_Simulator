#pragma once

#include <concepts>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Engine/AssetLoader/Csv.hpp"
#include "Engine/AssetLoader/GetAssetFullPath.h"
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

class AssetManager
{
public:
	template<typename AssetType>
	requires MoveOnlyType<AssetType>
	void AddAsset(const std::string& Name, AssetType&& Asset);

	template<typename AssetType>
	requires MoveOnlyType<AssetType>
	AssetType* GetAsset(const std::string& Name);

	template<typename AssetType>
	requires MoveOnlyType<AssetType>
	std::vector<AssetType*> GetAssets();

	void RemoveAsset(const std::string& Name);
	void ClearAssets();

	static AssetManager* GetInstance();

private:
	explicit AssetManager()           = default;
	AssetManager(const AssetManager&) = delete;
	AssetManager(AssetManager&&)      = delete;
	~AssetManager();

	AssetManager& operator=(const AssetManager&) = delete;
	AssetManager& operator=(AssetManager&&)      = delete;

private:
	using ManagedAsset = std::unique_ptr<void, TypeErasedDeleter>;
	std::unordered_map<std::string, ManagedAsset> _Assets;
};

_ASSET_END
_NPGS_END

#include "AssetManager.inl"
