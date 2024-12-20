#pragma once

#include <string>
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_ASSET_BEGIN

enum class AssetType
{
	kBinaryShader, // 二进制着色器程序
	kDataTable,    // 数据表
	kFont,         // 字体
	kModel,        // 模型
	kShader,       // 着色器
	kTexture       // 纹理
};

std::string GetAssetFullPath(AssetType Type, const std::string& Filename);

_ASSET_END
_NPGS_END
