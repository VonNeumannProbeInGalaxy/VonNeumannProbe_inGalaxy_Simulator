#pragma once

#include "Texture.h"

_NPGS_BEGIN
_ASSET_BEGIN

NPGS_INLINE void Texture::BindTextureUnit(GLuint Unit) const
{
	glBindTextureUnit(Unit, _Textures[0]);
}

NPGS_INLINE void Texture::BindTextureUnit(const Shader& DrawShader, const std::string& UniformName, GLuint Unit) const
{
	glBindTextureUnit(Unit, _Textures[0]);
	DrawShader.SetUniform1i(UniformName, static_cast<GLint>(Unit));
}

NPGS_INLINE GLuint Texture::GetTexture() const
{
	return _Textures[0];
}

NPGS_INLINE Texture::TextureType Texture::GetTextureType() const
{
	return _Type;
}

_ASSET_END
_NPGS_END
