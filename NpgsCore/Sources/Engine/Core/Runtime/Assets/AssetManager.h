#pragma once

#include <concepts>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Engine/Core/Base/Base.h"
#include "Engine/Core/Runtime/Assets/CommaSeparatedValues.hpp"
#include "Engine/Core/Runtime/Assets/GetAssetFullPath.h"
#include "Engine/Core/Runtime/Assets/Mesh.h"
#include "Engine/Core/Runtime/Assets/Model.h"
#include "Engine/Core/Runtime/Assets/Shader.h"
#include "Engine/Core/Runtime/Assets/Texture.h"

_NPGS_BEGIN
_RUNTIME_BEGIN
_ASSET_BEGIN

class FTypeErasedDeleter
{
public:
    template <typename OriginalType>
    FTypeErasedDeleter(OriginalType*)
        : _Deleter([](void* Ptr) -> void { delete static_cast<OriginalType*>(Ptr); })
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
concept CMoveOnlyType = std::movable<AssetType> && !std::copyable<AssetType>;

class FAssetManager
{
public:
    template <typename AssetType>
    requires CMoveOnlyType<AssetType>
    void AddAsset(const std::string& Name, AssetType&& Asset);

    template <typename AssetType>
    requires CMoveOnlyType<AssetType>
    AssetType* GetAsset(const std::string& Name);

    template <typename AssetType>
    requires CMoveOnlyType<AssetType>
    std::vector<AssetType*> GetAssets();

    void RemoveAsset(const std::string& Name);
    void ClearAssets();

    static FAssetManager* GetInstance();

private:
    explicit FAssetManager()            = default;
    FAssetManager(const FAssetManager&) = delete;
    FAssetManager(FAssetManager&&)      = delete;
    ~FAssetManager();

    FAssetManager& operator=(const FAssetManager&) = delete;
    FAssetManager& operator=(FAssetManager&&)      = delete;

private:
    using FManagedAsset = std::unique_ptr<void, FTypeErasedDeleter>;
    std::unordered_map<std::string, FManagedAsset> _Assets;
};

_ASSET_END
_RUNTIME_END
_NPGS_END

#include "AssetManager.inl"
