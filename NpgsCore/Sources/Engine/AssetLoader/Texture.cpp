#include "Texture.h"

#include <cstdlib>
#include <filesystem>
#include <print>
#include <vector>

#include <gli/gli.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include "Engine/AssetLoader/GetAssetFilepath.h"

_NPGS_BEGIN
_ASSET_BEGIN

Texture::Texture(TextureType CreateType, const std::string& Filename, bool bSrgb, bool bFlipVertically, bool bAutoFillFilepath)
	: _Texture(0), _Type(CreateType)
{
	GLubyte* Data = nullptr;

	switch (_Type)
	{
	case TextureType::k2D:
		Data = Create2dTexture(Filename, bSrgb, bFlipVertically, bAutoFillFilepath);
		break;
	case TextureType::kCubeMap:
		Data = CreateCubeMap(Filename, bSrgb, bFlipVertically);
		break;
	default:
		break;
	}

	if (Data != nullptr)
	{
		stbi_image_free(Data);
	}
}

Texture::Texture(const FT_Face& Face)
	: _Texture(0), _Type(TextureType::kCharacter)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &_Texture);
	glTextureStorage2D(_Texture, 1, GL_R8, Face->glyph->bitmap.width, Face->glyph->bitmap.rows);
	glTextureSubImage2D(_Texture, 0, 0, 0, Face->glyph->bitmap.width, Face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, Face->glyph->bitmap.buffer);

	glTextureParameteri(_Texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(_Texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(_Texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(_Texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture::Texture(GLsizei Width, GLsizei Height, GLenum InternalFormat, GLenum Attachment, GLuint Framebuffer)
	: _Texture(0), _Type(TextureType::kAttachment)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &_Texture);
	glTextureParameteri(_Texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(_Texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureStorage2D(_Texture, 1, InternalFormat, Width, Height);
	glNamedFramebufferTexture(Framebuffer, Attachment, _Texture, 0);
}

Texture::Texture(GLsizei Width, GLsizei Height, GLenum InternalFormat, GLenum Attachment, GLsizei Samples, GLboolean bFixedSampleLocations, GLuint Framebuffer)
	: _Texture(0), _Type(TextureType::kAttachment)
{
	glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &_Texture);
	glTextureStorage2DMultisample(_Texture, Samples, InternalFormat, Width, Height, bFixedSampleLocations);
	glNamedFramebufferTexture(Framebuffer, Attachment, _Texture, 0);
}

Texture::Texture(GLsizei Width, GLsizei Height)
	: _Texture(0), _Type(TextureType::kDepthMap)
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

Texture::Texture(Texture&& Other) noexcept
	:
	_Texture(Other._Texture),
	_Type(Other._Type)
{
	Other._Texture = 0;
}

Texture::~Texture()
{
	glDeleteTextures(1, &_Texture);
}

Texture& Texture::operator=(Texture&& Other) noexcept
{
	if (this != &Other)
	{
		if (_Texture)
		{
			glDeleteTextures(1, &_Texture);
		}

		_Texture       = Other._Texture;
		_Type          = Other._Type;
		Other._Texture = 0;
	}

	return *this;
}

GLubyte* Texture::Create2dTexture(const std::string& Filename, bool bSrgb, bool bFlipVertically, bool bAutoFillFilepath)
{
	std::string ImageFilepath = bAutoFillFilepath ? GetAssetFilepath(AssetType::kTexture, Filename) : Filename;

	glCreateTextures(GL_TEXTURE_2D, 1, &_Texture);

	if (std::filesystem::path(Filename).extension() == ".ddx" ||
		std::filesystem::path(Filename).extension() == ".ktx")
	{
		gli::texture2d Texture(gli::load(ImageFilepath));
		if (!Texture.empty())
		{
			gli::gl Converter(gli::gl::PROFILE_GL33);
			gli::gl::format Format = Converter.translate(Texture.format(), Texture.swizzles());

			glTextureStorage2D(_Texture, static_cast<GLint>(Texture.levels()), Format.Internal,
							   Texture.extent().x, Texture.extent().y);

			for (std::size_t Level = 0; Level < Texture.levels(); ++Level)
			{
				glTextureSubImage2D(_Texture, static_cast<GLint>(Level), 0, 0,
									Texture[Level].extent().x, Texture[Level].extent().y,
									Format.External, Format.Type, Texture[Level].data());
			}

			glTextureParameteri(_Texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(_Texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTextureParameteri(_Texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTextureParameteri(_Texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			return nullptr;
		}
	}

	ImageData Data = LoadImage(ImageFilepath, bSrgb, bFlipVertically, bAutoFillFilepath);

	glTextureStorage2D(_Texture, 1, Data.InternalFormat, Data.Width, Data.Height);
	glTextureSubImage2D(_Texture, 0, 0, 0, Data.Width, Data.Height, Data.Format, GL_UNSIGNED_BYTE, Data.Data);
	glGenerateTextureMipmap(_Texture);

	glTextureParameteri(_Texture, GL_TEXTURE_WRAP_S, Data.TexWrapS);
	glTextureParameteri(_Texture, GL_TEXTURE_WRAP_T, Data.TexWrapT);
	glTextureParameteri(_Texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(_Texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return Data.Data;
}

GLubyte* Texture::CreateCubeMap(const std::string& Filename, bool bSrgb, bool bFlipVertically)
{
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &_Texture);

	if (std::filesystem::path(Filename).extension() == ".ddx" ||
		std::filesystem::path(Filename).extension() == ".ktx")
	{
		gli::texture_cube Cube(gli::load(GetAssetFilepath(Asset::AssetType::kTexture, Filename)));
		if (!Cube.empty())
		{
			gli::gl Converter(gli::gl::PROFILE_GL33);
			gli::gl::format Format = Converter.translate(Cube.format(), Cube.swizzles());

			glTextureStorage2D(_Texture, static_cast<GLint>(Cube.levels()), Format.Internal, Cube.extent().x, Cube.extent().y);

			for (std::size_t Face = 0; Face != Cube.faces(); ++Face)
			{
				for (std::size_t Level = 0; Level != Cube.levels(); ++Level)
				{
					glTextureSubImage3D(_Texture, static_cast<GLint>(Level), 0, 0, static_cast<GLint>(Face),
										Cube[Face][Level].extent().x, Cube[Face][Level].extent().y, 1,
										Format.External, Format.Type, Cube[Face][Level].data());
				}
			}

			glTextureParameteri(_Texture, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTextureParameteri(_Texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(_Texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTextureParameteri(_Texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTextureParameteri(_Texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			return nullptr;
		}
	}

	ImageData Data;
	std::string Directory = GetAssetFilepath(AssetType::kTexture, Filename);

	std::vector<std::string> FaceNames{ "posx", "negx", "posy", "negy", "posz", "negz" };
	std::vector<std::string> Images(6);

	for (const auto& Entry : std::filesystem::directory_iterator(Directory))
	{
		if (Entry.is_regular_file())
		{
			std::string Stem = Entry.path().stem().string();
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
	}

	return Data.Data;
}

Texture::ImageData Texture::LoadImage(const std::string& Filename, bool bSrgb, bool bFlipVertically, bool bAutoFillFilepath) const
{
	GLint ImageWidth    = 0;
	GLint ImageHeight   = 0;
	GLint ImageChannels = 0;

	stbi_set_flip_vertically_on_load(bFlipVertically);
	GLubyte* ImageData = stbi_load(Filename.c_str(), &ImageWidth, &ImageHeight, &ImageChannels, 0);

	if (!ImageData)
	{
		std::println("Fatal error: Can not open image file: \"{}\": No such fire or directory.", Filename);
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
