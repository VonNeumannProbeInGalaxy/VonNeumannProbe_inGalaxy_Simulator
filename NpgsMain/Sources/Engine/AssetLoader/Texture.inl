#include "Texture.h"

inline GLuint Texture::GetTexture() const
{
	return _Texture;
}

inline Texture::Type Texture::GetTextureType() const
{
	return _TextureType;
}
