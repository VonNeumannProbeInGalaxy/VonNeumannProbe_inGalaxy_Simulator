#pragma once

#include <concepts>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Engine/Core/Base.h"

_NPGS_BEGIN

template <typename T>
concept Copyable = std::copyable<T>;

class AssetManager {
public:
    AssetManager() = default;
    ~AssetManager() = default;

public:
    template<typename T>
    requires Copyable<T>
    static void AddAsset(const std::string& Name, const std::shared_ptr<T>& Asset) {
        _Assets[Name] = Asset;
    }

    template<typename T>
    requires Copyable<T>
    static std::shared_ptr<T> GetAsset(const std::string& Name) {
        return std::static_pointer_cast<T>(_Assets[Name]);
    }

    template<typename T>
    requires Copyable<T>
    static std::vector<std::shared_ptr<T>> GetAssets() {
        std::vector<std::shared_ptr<T>> Assets;
        for (auto& Asset : _Assets) {
            Assets.push_back(std::static_pointer_cast<T>(Asset.second));
        }

        return Assets;
    }

    static void RemoveAsset(const std::string& Name) {
        _Assets.erase(Name);
    }

    static void ClearAssets() {
        _Assets.clear();
    }

private:
    static std::map<std::string, std::shared_ptr<void>> _Assets;
};

_NPGS_END
