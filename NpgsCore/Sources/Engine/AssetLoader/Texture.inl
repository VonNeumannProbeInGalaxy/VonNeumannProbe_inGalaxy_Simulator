#pragma once

#include "Texture.h"

_NPGS_BEGIN
_ASSET_BEGIN

inline GLuint Texture::GetTexture() const
{
	return _Texture;
}

inline Texture::TextureType Texture::GetTextureType() const
{
	return _Type;
}

_ASSET_END
_NPGS_END
