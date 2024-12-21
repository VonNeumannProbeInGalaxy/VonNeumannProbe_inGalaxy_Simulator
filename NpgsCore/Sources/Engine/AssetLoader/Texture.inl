#pragma once

#include "Texture.h"

_NPGS_BEGIN
_ASSET_BEGIN

NPGS_INLINE void FTexture::BindTextureUnit(GLuint Unit) const
{
	glBindTextureUnit(Unit, _Textures[0]);
}

NPGS_INLINE void FTexture::BindTextureUnit(const FShader& DrawShader, const std::string& UniformName, GLuint Unit) const
{
	glBindTextureUnit(Unit, _Textures[0]);
	DrawShader.SetUniform1i(UniformName, static_cast<GLint>(Unit));
}

NPGS_INLINE GLuint FTexture::GetTexture() const
{
	return _Textures[0];
}

NPGS_INLINE FTexture::ETextureType FTexture::GetTextureType() const
{
	return _Type;
}

_ASSET_END
_NPGS_END
