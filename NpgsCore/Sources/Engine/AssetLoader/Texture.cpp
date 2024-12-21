#include "Texture.h"

#include <cstdlib>
#include <filesystem>
#include <print>
#include <utility>
#include <vector>

#include <gli/gli.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include "Engine/AssetLoader/GetAssetFullPath.h"

_NPGS_BEGIN
_ASSET_BEGIN

FTexture::FTexture(ETextureType CreateType, const std::string& Filename, bool bSrgb, bool bFlipVertically, bool bAutoFillFilePath)
	: _Type(CreateType)
{
	GLubyte* Data = nullptr;

	switch (_Type)
	{
	case ETextureType::k2D:
		Data = Create2dTexture(Filename, bSrgb, bFlipVertically, bAutoFillFilePath);
		break;
	case ETextureType::kCubeMap:
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

FTexture::FTexture(const FT_Face& Face)
	: _Type(ETextureType::kCharacter)
{
	GLuint MyTexture = 0;
	glCreateTextures(GL_TEXTURE_2D, 1, &MyTexture);
	glTextureStorage2D(MyTexture, 1, GL_R8, Face->glyph->bitmap.width, Face->glyph->bitmap.rows);
	glTextureSubImage2D(MyTexture, 0, 0, 0, Face->glyph->bitmap.width, Face->glyph->bitmap.rows,
						GL_RED, GL_UNSIGNED_BYTE, Face->glyph->bitmap.buffer);

	glTextureParameteri(MyTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(MyTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(MyTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(MyTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	_Textures.emplace_back(MyTexture);
}

FTexture::FTexture(GLsizei Width, GLsizei Height, GLenum InternalFormat, GLenum Attachment, GLuint Framebuffer)
	: _Type(ETextureType::kAttachment)
{
	GLuint MyTexture = 0;
	glCreateTextures(GL_TEXTURE_2D, 1, &MyTexture);
	glTextureParameteri(MyTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(MyTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureStorage2D(MyTexture, 1, InternalFormat, Width, Height);
	glNamedFramebufferTexture(Framebuffer, Attachment, MyTexture, 0);

	_Textures.emplace_back(MyTexture);
}

FTexture::FTexture(GLsizei Width, GLsizei Height, GLenum InternalFormat, GLenum Attachment,
				   GLsizei Samples, GLboolean bFixedSampleLocations, GLuint Framebuffer)
	: _Type(ETextureType::kAttachment)
{
	GLuint MyTexture = 0;
	glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &MyTexture);
	glTextureStorage2DMultisample(MyTexture, Samples, InternalFormat, Width, Height, bFixedSampleLocations);
	glNamedFramebufferTexture(Framebuffer, Attachment, MyTexture, 0);

	_Textures.emplace_back(MyTexture);
}

FTexture::FTexture(GLsizei Width, GLsizei Height)
	: _Type(ETextureType::kDepthMap)
{
	GLuint MyTexture = 0;
	glCreateTextures(GL_TEXTURE_2D, 1, &MyTexture);
	glTextureStorage2D(MyTexture, 1, GL_DEPTH_COMPONENT24, Width, Height);
	glTextureParameteri(MyTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(MyTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(MyTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTextureParameteri(MyTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glm::vec4 BorderColor(1.0f);
	glTextureParameterfv(MyTexture, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(BorderColor));

	_Textures.emplace_back(MyTexture);
}

FTexture::FTexture(FTexture&& Other) noexcept
	:
	_Textures(std::move(Other._Textures)),
	_Type(Other._Type)
{
}

FTexture::~FTexture()
{
	glDeleteTextures(static_cast<GLsizei>(_Textures.size()), _Textures.data());
}

FTexture& FTexture::operator=(FTexture&& Other) noexcept
{
	if (this != &Other)
	{
		if (!_Textures.empty())
		{
			glDeleteTextures(static_cast<GLsizei>(_Textures.size()), _Textures.data());
		}

		_Textures       = std::move(Other._Textures);
		_Type           = Other._Type;
		Other._Textures.clear();
	}

	return *this;
}

GLubyte* FTexture::Create2dTexture(const std::string& Filename, bool bSrgb, bool bFlipVertically, bool bAutoFillFilePath)
{
	std::string ImageFilePath = bAutoFillFilePath ? GetAssetFullPath(AssetType::kTexture, Filename) : Filename;

	GLenum MyTexture = 0;
	glCreateTextures(GL_TEXTURE_2D, 1, &MyTexture);

	if (std::filesystem::path(Filename).extension().string() == ".ddx" ||
		std::filesystem::path(Filename).extension().string() == ".ktx")
	{
		gli::texture2d Texture2D(gli::load(ImageFilePath));
		if (!Texture2D.empty())
		{
			gli::gl Converter(gli::gl::PROFILE_GL33);
			gli::gl::format Format = Converter.translate(Texture2D.format(), Texture2D.swizzles());

			glTextureStorage2D(MyTexture, static_cast<GLint>(Texture2D.levels()), Format.Internal,
							   Texture2D.extent().x, Texture2D.extent().y);

			for (std::size_t Level = 0; Level < Texture2D.levels(); ++Level)
			{
				glTextureSubImage2D(MyTexture, static_cast<GLint>(Level), 0, 0,
									Texture2D[Level].extent().x, Texture2D[Level].extent().y,
									Format.External, Format.Type, Texture2D[Level].data());
			}

			glTextureParameteri(MyTexture, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(MyTexture, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTextureParameteri(MyTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTextureParameteri(MyTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			_Textures.emplace_back(MyTexture);

			return nullptr;
		}
	}

	FImageData Data = LoadImage(ImageFilePath, bSrgb, bFlipVertically);

	glTextureStorage2D(MyTexture, 1, Data.InternalFormat, Data.Width, Data.Height);
	glTextureSubImage2D(MyTexture, 0, 0, 0, Data.Width, Data.Height, Data.Format, GL_UNSIGNED_BYTE, Data.Data);
	glGenerateTextureMipmap(MyTexture);

	glTextureParameteri(MyTexture, GL_TEXTURE_WRAP_S, Data.TexWrapS);
	glTextureParameteri(MyTexture, GL_TEXTURE_WRAP_T, Data.TexWrapT);
	glTextureParameteri(MyTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(MyTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	_Textures.emplace_back(MyTexture);

	return Data.Data;
}

GLubyte* FTexture::CreateCubeMap(const std::string& Filename, bool bSrgb, bool bFlipVertically)
{
	GLuint MyTexture = 0;
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &MyTexture);

	if (std::filesystem::path(Filename).extension().string() == ".ddx" ||
		std::filesystem::path(Filename).extension().string() == ".ktx")
	{
		gli::texture_cube Cube(gli::load(GetAssetFullPath(Asset::AssetType::kTexture, Filename)));
		if (!Cube.empty())
		{
			gli::gl Converter(gli::gl::PROFILE_GL33);
			gli::gl::format Format = Converter.translate(Cube.format(), Cube.swizzles());

			glTextureStorage2D(MyTexture, static_cast<GLint>(Cube.levels()), Format.Internal, Cube.extent().x, Cube.extent().y);

			for (std::size_t Face = 0; Face != Cube.faces(); ++Face)
			{
				for (std::size_t Level = 0; Level != Cube.levels(); ++Level)
				{
					glTextureSubImage3D(MyTexture, static_cast<GLint>(Level), 0, 0, static_cast<GLint>(Face),
										Cube[Face][Level].extent().x, Cube[Face][Level].extent().y, 1,
										Format.External, Format.Type, Cube[Face][Level].data());
				}
			}

			glTextureParameteri(MyTexture, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTextureParameteri(MyTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(MyTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTextureParameteri(MyTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTextureParameteri(MyTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			_Textures.emplace_back(MyTexture);

			return nullptr;
		}
	}

	FImageData Data;
	std::string Directory = GetAssetFullPath(AssetType::kTexture, Filename);

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
		Data = LoadImage(Directory + '/' + Images[i], bSrgb, bFlipVertically);

		if (i == 0)
		{
			glTextureStorage2D(MyTexture, 1, Data.InternalFormat, Data.Width, Data.Height);
		}

		glTextureSubImage3D(MyTexture, 0, 0, 0, i, Data.Width, Data.Height, 1, Data.Format, GL_UNSIGNED_BYTE, Data.Data);

		glTextureParameteri(MyTexture, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTextureParameteri(MyTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(MyTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(MyTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(MyTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	_Textures.emplace_back(MyTexture);

	return Data.Data;
}

FTexture::FImageData FTexture::LoadImage(const std::string& Filename, bool bSrgb, bool bFlipVertically) const
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
