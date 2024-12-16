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
		GLubyte* Data{};
		GLsizei Width{};
		GLsizei Height{};
		GLenum Format{};
		GLenum InternalFormat{};
		GLenum TexWrapS{};
		GLenum TexWrapT{};
	};

public:
	Texture(TextureType CreateType, const std::string& Filename, bool bSrgb = false, bool bFlipVertically = true, bool bAutoFillFilepath = true);
	Texture(const FT_Face& Face);
	Texture(GLsizei Width, GLsizei Height, GLenum InternalFormat, GLenum Attachment, GLuint Framebuffer);
	Texture(GLsizei Width, GLsizei Height, GLenum InternalFormat, GLenum Attachment, GLsizei Samples,
			GLboolean bFixedSampleLocations, GLuint Framebuffer);
	Texture(GLsizei Width, GLsizei Height);
	Texture(const Texture&) = delete;
	Texture(Texture&& Other) noexcept;

	~Texture();

	Texture& operator=(const Texture&) = delete;
	Texture& operator=(Texture&& Other) noexcept;

	void BindTextureUnit(GLuint Unit) const;
	void BindTextureUnit(const Shader& DrawShader, const std::string& UniformName, GLuint Unit) const;
	GLuint GetTexture() const;
	TextureType GetTextureType() const;

private:
	ImageData Create2dTexture(const std::string& Filename, bool bSrgb, bool bFlipVertically, bool bAutoFillFilepath);
	ImageData CreateCubeMap(const std::string& Filename, bool bSrgb, bool bFlipVertically);
	ImageData LoadImage(const std::string& Filename, bool bSrgb, bool bFlipVertically, bool bAutoFillFilepath) const;

private:
	GLuint _Texture;
	TextureType _Type;
};

_ASSET_END
_NPGS_END

#include "Texture.inl"
