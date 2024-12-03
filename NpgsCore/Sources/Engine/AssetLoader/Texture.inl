#include "Texture.h"

_NPGS_BEGIN
_ASSET_BEGIN

inline GLuint Texture::GetTexture() const
{
	return _Texture;
}

inline Texture::Type Texture::GetTextureType() const
{
	return _TextureType;
}

_ASSET_END
_NPGS_END
