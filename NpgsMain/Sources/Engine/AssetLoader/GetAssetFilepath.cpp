#pragma warning(disable : 4715)

#include "GetAssetFilepath.h"
#include <cassert>

std::string GetAssetFilepath(AssetType Type, const std::string& Filename)
{
	std::string RootFolderName = Type == AssetType::kBinaryShader ? "" : "Assets/";
#ifdef _RELEASE
	RootFolderName = std::string("../") + RootFolderName;
#endif // _RELEASE

	auto AssetFolderName = [Type]() -> std::string
	{
		switch (Type)
		{
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
			assert(GL_FALSE);
		}
	}();

	return RootFolderName + AssetFolderName + Filename;
}
