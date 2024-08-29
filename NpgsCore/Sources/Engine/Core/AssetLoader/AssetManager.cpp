#include "AssetManager.h"

_NPGS_BEGIN
_ASSETS_BEGIN

std::string GetAssetFilepath(AssetType Type, const std::string& Filename) {
    std::string RootFolderName = Type == AssetType::kBinaryShader ? "" : "Assets/";
#ifdef _RELEASE
    RootFolderName = std::string("../") + RootFolderName;
#endif // _RELEASE

    auto AssetFolderName = [Type]() -> std::string {
        switch (Type) {
        case AssetType::kBinaryShader:
            return "Cache/Shaders/";
        case AssetType::kFont:
            return "Fonts/";
        case AssetType::kModel:
            return "Models/";
        case AssetType::kShader:
            return "Shaders/";
        case AssetType::kTexture:
            return "Textures/";
        default:
            assert(false);
        }
    }();

    return RootFolderName + AssetFolderName + Filename;
}

std::unordered_map<std::string, std::shared_ptr<void>> AssetManager::_kAssets;

_ASSETS_END
_NPGS_END
