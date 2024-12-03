#include "Texture.h"

#include <cstdlib>
#include <filesystem>
#include <print>
#include <vector>

#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include "Engine/AssetLoader/GetAssetFilepath.h"

_NPGS_BEGIN
_ASSET_BEGIN

Texture::Texture(Type CreateType, const std::string& Filepath, GLboolean bSrgb, GLboolean bFlipVertically, GLboolean bAutoFillFilepath)
	: _Texture(0), _TextureType(CreateType)
{
	ImageData Data{};

	switch (_TextureType)
	{
	case Type::k2D:
	{
		Data = LoadImage(Filepath, bSrgb, bFlipVertically, bAutoFillFilepath);

		glCreateTextures(GL_TEXTURE_2D, 1, &_Texture);

		glTextureStorage2D(_Texture, 1, Data.InternalFormat, Data.Width, Data.Height);
		glTextureSubImage2D(_Texture, 0, 0, 0, Data.Width, Data.Height, Data.Format, GL_UNSIGNED_BYTE, Data.Data);
		glGenerateTextureMipmap(_Texture);

		glTextureParameteri(_Texture, GL_TEXTURE_WRAP_S, Data.TexWrapS);
		glTextureParameteri(_Texture, GL_TEXTURE_WRAP_T, Data.TexWrapT);
		glTextureParameteri(_Texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(_Texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(Data.Data);

		break;
	}
	case Type::kCubeMap:
	{
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &_Texture);

		std::vector<std::string> FaceNames{ "Right", "Left", "Top", "Bottom", "Front", "Back" };
		std::vector<std::string> Images(6);

		std::string Directory = GetAssetFilepath(AssetType::kTexture, Filepath);

		for (const auto& Entry : std::filesystem::directory_iterator(Directory))
		{
			if (Entry.is_regular_file())
			{
				std::string Stem      = Entry.path().stem().string();
				std::string Extension = Entry.path().extension().string();

				for (GLuint i = 0; i != FaceNames.size(); ++i)
				{
					if (Stem.find(FaceNames[i]) != std::string::npos)
					{
						Images[i] = Stem + Extension;
						break;
					}
				}
			}
		}

		for (GLuint i = 0; i != Images.size(); ++i)
		{
			Data = LoadImage(Directory + '/' + Images[i], bSrgb, bFlipVertically, GL_FALSE);

			if (i == 0)
			{
				glTextureStorage2D(_Texture, 1, Data.InternalFormat, Data.Width, Data.Height);
			}

			glTextureSubImage3D(_Texture, 0, 0, 0, i, Data.Width, Data.Height, 1, Data.Format, GL_UNSIGNED_BYTE, Data.Data);

			glTextureParameteri(_Texture, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTextureParameteri(_Texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(_Texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTextureParameteri(_Texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTextureParameteri(_Texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(Data.Data);
		}

		break;
	}
	}
}

Texture::Texture(Type CreateType, const FT_Face& Face) : _Texture(0), _TextureType(CreateType)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &_Texture);
	glTextureStorage2D(_Texture, 1, GL_R8, Face->glyph->bitmap.width, Face->glyph->bitmap.rows);
	glTextureSubImage2D(_Texture, 0, 0, 0, Face->glyph->bitmap.width, Face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, Face->glyph->bitmap.buffer);

	glTextureParameteri(_Texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(_Texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(_Texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(_Texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture::Texture(Type CreateType, GLsizei Width, GLsizei Height, GLenum InternalFormat, GLenum Attachment, GLuint Framebuffer)
	: _Texture(0), _TextureType(CreateType)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &_Texture);
	glTextureParameteri(_Texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(_Texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureStorage2D(_Texture, 1, InternalFormat, Width, Height);
	glNamedFramebufferTexture(Framebuffer, Attachment, _Texture, 0);
}

Texture::Texture(Type CreateType, GLsizei Width, GLsizei Height, GLenum InternalFormat, GLsizei Samples, GLenum Attachment, GLboolean bFixedSampleLocations, GLuint Framebuffer)
	: _Texture(0), _TextureType(CreateType)
{
	glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &_Texture);
	glTextureStorage2DMultisample(_Texture, Samples, InternalFormat, Width, Height, bFixedSampleLocations);
	glNamedFramebufferTexture(Framebuffer, Attachment, _Texture, 0);
}

Texture::Texture(Type CreateType, GLsizei Width, GLsizei Height) : _Texture(0), _TextureType(CreateType)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &_Texture);
	glTextureStorage2D(_Texture, 1, GL_DEPTH_COMPONENT24, Width, Height);
	glTextureParameteri(_Texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(_Texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(_Texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTextureParameteri(_Texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glm::vec4 BorderColor(1.0f);
	glTextureParameterfv(_Texture, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(BorderColor));
}

Texture::~Texture()
{
	glDeleteTextures(1, &_Texture);
}

GLvoid Texture::BindTextureUnit(const Shader& ActivatedShader, const std::string& UniformName, GLuint Unit) const
{
	glBindTextureUnit(Unit, _Texture);
	ActivatedShader.SetUniform1i(UniformName, static_cast<GLint>(Unit));
}

Texture::ImageData Texture::LoadImage(const std::string& ImageFilename, GLboolean bSrgb, GLboolean bFlipVertically, GLboolean bAutoFillFilepath) const
{
	GLint ImageWidth    = 0;
	GLint ImageHeight   = 0;
	GLint ImageChannels = 0;

	std::string ImageFilepath;
	if (bAutoFillFilepath)
	{
		ImageFilepath = GetAssetFilepath(AssetType::kTexture, ImageFilename);
	}
	else
	{
		ImageFilepath = ImageFilename;
	}

	stbi_set_flip_vertically_on_load(bFlipVertically);
	GLubyte* ImageData = stbi_load(ImageFilepath.c_str(), &ImageWidth, &ImageHeight, &ImageChannels, 0);

	if (!ImageData)
	{
		std::println("Fatal error: Can not open image file: \"{}\": No such fire or directory.", ImageFilepath);
		std::system("pause");
		std::exit(EXIT_FAILURE);
	}

	GLenum ImageFormat    = 0;
	GLenum InternalFormat = 0;
	GLenum TexWrapS       = 0;
	GLenum TexWrapT       = 0;

	switch (ImageChannels)
	{
	case 3:
		ImageFormat    = GL_RGB;
		InternalFormat = bSrgb ? GL_SRGB8 : GL_RGB16;
		TexWrapS       = GL_REPEAT;
		TexWrapT       = GL_REPEAT;
		break;
	case 4:
		ImageFormat    = GL_RGBA;
		InternalFormat = bSrgb ? GL_SRGB8_ALPHA8 : GL_RGBA16;
		TexWrapS       = GL_CLAMP_TO_EDGE;
		TexWrapT       = GL_CLAMP_TO_EDGE;
		break;
	default:
		break;
	}

	return { ImageData, ImageWidth, ImageHeight, ImageFormat, InternalFormat, TexWrapS, TexWrapT };
}

_ASSET_END
_NPGS_END

// stb_image implementation
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
