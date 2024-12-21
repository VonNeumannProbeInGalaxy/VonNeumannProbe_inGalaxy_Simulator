#pragma once

#include <string>

#include <freetype/freetype.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Engine/AssetLoader/Shader.h"

_NPGS_BEGIN
_ASSET_BEGIN

class FTexture
{
public:
	enum class ETextureType
	{
		k2D, kAttachment, kDepthMap, kCharacter, kCubeMap
	};

	struct FImageData
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
	FTexture(ETextureType CreateType, const std::string& Filename, bool bSrgb = false,
			 bool bFlipVertically = true, bool bAutoFillFilePath = true);

	FTexture(const FT_Face& Face);
	FTexture(GLsizei Width, GLsizei Height, GLenum InternalFormat, GLenum Attachment, GLuint Framebuffer);

	FTexture(GLsizei Width, GLsizei Height, GLenum InternalFormat, GLenum Attachment,
			 GLsizei Samples, GLboolean bFixedSampleLocations, GLuint Framebuffer);

	FTexture(GLsizei Width, GLsizei Height);
	FTexture(const FTexture&) = delete;
	FTexture(FTexture&& Other) noexcept;

	~FTexture();

	FTexture& operator=(const FTexture&) = delete;
	FTexture& operator=(FTexture&& Other) noexcept;

	void BindTextureUnit(GLuint Unit) const;
	void BindTextureUnit(const FShader& DrawShader, const std::string& UniformName, GLuint Unit) const;
	GLuint GetTexture() const;
	ETextureType GetTextureType() const;

private:
	GLubyte* Create2dTexture(const std::string& Filename, bool bSrgb, bool bFlipVertically, bool bAutoFillFilePath);
	GLubyte* CreateCubeMap(const std::string& Filename, bool bSrgb, bool bFlipVertically);
	FImageData LoadImage(const std::string& Filename, bool bSrgb, bool bFlipVertically) const;

private:
	std::vector<GLuint> _Textures;
	ETextureType        _Type;
};

_ASSET_END
_NPGS_END

#include "Texture.inl"
