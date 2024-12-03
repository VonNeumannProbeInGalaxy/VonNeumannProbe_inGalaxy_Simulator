#pragma once

#include <string>

// @enum AssetType 资产类型
// @brief 枚举不同类型的资产。
enum class AssetType
{
	kBinaryShader, // 二进制着色器程序
	kFont,         // 字体
	kModel,        // 模型
	kShader,       // 着色器
	kTexture       // 纹理
};

// @brief 获取指定类型资产的文件路径。
// @param Type 资产类型。
// @param Filename 文件名。
// @return 资产文件的路径。
std::string GetAssetFilepath(AssetType Type, const std::string& Filename);
