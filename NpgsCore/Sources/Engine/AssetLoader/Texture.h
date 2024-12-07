#pragma once

#include <string>

#include <freetype/freetype.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Engine/AssetLoader/Shader.h"

_NPGS_BEGIN
_ASSET_BEGIN

class Texture
{
public:
	enum class TextureType
	{
		k2D, kAttachment, kDepthMap, kCharacter, kCubeMap
	};

	struct ImageData
	{
		GLubyte* Data;
		GLsizei  Width;
		GLsizei  Height;
		GLenum   Format;
		GLenum   InternalFormat;
		GLenum   TexWrapS;
		GLenum   TexWrapT;
	};

public:
	Texture(TextureType CreateType, const std::string& Filepath, bool bSrgb = false, bool bFlipVertically = true, bool bAutoFillFilepath = true);
	Texture(TextureType CreateType, const FT_Face& Face);
	Texture(TextureType CreateType, GLsizei Width, GLsizei Height, GLenum InternalFormat, GLenum Attachment, GLuint Framebuffer);
	Texture(TextureType CreateType, GLsizei Width, GLsizei Height, GLenum InternalFormat, GLsizei Samples,
			GLenum Attachment, bool bFixedSampleLocations, GLuint Framebuffer);
	Texture(TextureType CreateType, GLsizei Width, GLsizei Height);
	Texture(const Texture&) = delete;
	Texture(Texture&& Other) noexcept;

	~Texture();

	Texture& operator=(const Texture&) = delete;
	Texture& operator=(Texture&& Other) noexcept;

	void BindTextureUnit(const Shader& ActivatedShader, const std::string& UniformName, GLuint Unit) const;
	GLuint GetTexture() const;
	TextureType GetTextureType() const;

private:
	ImageData LoadImage(const std::string& ImageFilename, bool bSrgb, bool bFlipVertically, bool bAutoFillFilepath) const;

private:
	GLuint _Texture;
	TextureType _Type;
};

_ASSET_END
_NPGS_END

#include "Texture.inl"
