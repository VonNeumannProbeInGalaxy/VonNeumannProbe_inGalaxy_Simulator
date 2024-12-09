#pragma once

#include "Texture.h"

_NPGS_BEGIN
_ASSET_BEGIN

NPGS_INLINE GLuint Texture::GetTexture() const
{
	return _Texture;
}

NPGS_INLINE Texture::TextureType Texture::GetTextureType() const
{
	return _Type;
}

_ASSET_END
_NPGS_END
