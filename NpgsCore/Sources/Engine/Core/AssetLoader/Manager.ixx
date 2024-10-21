module;

#include "Engine/Core/Base.h"

export module Asset.Manager;

import <concepts>;
import <memory>;
import <string>;
import <unordered_map>;
import <vector>;

import Asset.Csv;

_NPGS_BEGIN
_ASSETS_BEGIN

export enum class AssetType {
    kBinaryShader,
    kFont,
    kModel,
    kShader,
    kTexture
};

export std::string GetAssetFilepath(AssetType Type, const std::string& Filename);

template <typename AssetType>
concept Copyable = std::copyable<AssetType>;

export class AssetManager {
public:
    AssetManager() = default;
    ~AssetManager() = default;

public:
    template<typename AssetType>
    requires Copyable<AssetType>
    static void AddAsset(const std::string& Name, const std::shared_ptr<AssetType>& Asset) {
        _kAssets[Name] = Asset;
    }

    template<typename AssetType>
    requires Copyable<AssetType>
    static std::shared_ptr<AssetType> GetAsset(const std::string& Name) {
        return std::static_pointer_cast<AssetType>(_kAssets[Name]);
    }

    template<typename AssetType>
    requires Copyable<AssetType>
    static std::vector<std::shared_ptr<AssetType>> GetAssets() {
        std::vector<std::shared_ptr<AssetType>> Assets;
        for (auto& Asset : _kAssets) {
            Assets.emplace_back(std::static_pointer_cast<AssetType>(Asset.second));
        }

        return Assets;
    }

    static void RemoveAsset(const std::string& Name) {
        _kAssets.erase(Name);
    }

    static void ClearAssets() {
        _kAssets.clear();
    }

private:
    static std::unordered_map<std::string, std::shared_ptr<void>> _kAssets;
};

_ASSETS_END
_NPGS_END
