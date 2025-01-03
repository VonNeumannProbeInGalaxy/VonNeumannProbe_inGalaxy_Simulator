#include "Texture.h"

#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <utility>
#include <vector>

#include <gli/gli.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Engine/Core/Runtime/Assets/GetAssetFullPath.h"
#include "Engine/Utils/Logger.h"

_NPGS_BEGIN
_RUNTIME_BEGIN
_ASSET_BEGIN

FTexture::FTexture(ETextureType CreateType, const std::string& Filename, bool bSrgb, bool bFlipVertically, bool bAutoFillFilePath)
    : _Texture(0), _Type(CreateType)
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
    : _Texture(0), _Type(ETextureType::kCharacter)
{
    glCreateTextures(GL_TEXTURE_2D, 1, &_Texture);
    glTextureStorage2D(_Texture, 1, GL_R8, Face->glyph->bitmap.width, Face->glyph->bitmap.rows);
    glTextureSubImage2D(_Texture, 0, 0, 0, Face->glyph->bitmap.width, Face->glyph->bitmap.rows,
                        GL_RED, GL_UNSIGNED_BYTE, Face->glyph->bitmap.buffer);

    glTextureParameteri(_Texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(_Texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(_Texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(_Texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

FTexture::FTexture(GLsizei Width, GLsizei Height)
    : _Texture(0), _Type(ETextureType::kDepthMap)
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

FTexture::FTexture(FTexture&& Other) noexcept
    :
    _Texture(std::exchange(Other._Texture, 0)),
    _Type(Other._Type)
{
}

FTexture::~FTexture()
{
    glDeleteTextures(1, &_Texture);
}

FTexture& FTexture::operator=(FTexture&& Other) noexcept
{
    if (this != &Other)
    {
        if (_Texture)
        {
            glDeleteTextures(1, &_Texture);
        }

        _Texture = std::exchange(Other._Texture, 0);
        _Type    = Other._Type;
    }

    return *this;
}

GLubyte* FTexture::Create2dTexture(const std::string& Filename, bool bSrgb, bool bFlipVertically, bool bAutoFillFilePath)
{
    std::string ImageFilePath = bAutoFillFilePath ? GetAssetFullPath(EAssetType::kTexture, Filename) : Filename;

    glCreateTextures(GL_TEXTURE_2D, 1, &_Texture);

    if (std::filesystem::path(Filename).extension().string() == ".ddx" ||
        std::filesystem::path(Filename).extension().string() == ".ktx")
    {
        gli::texture2d Texture2D(gli::load(ImageFilePath));
        if (!Texture2D.empty())
        {
            gli::gl Converter(gli::gl::PROFILE_GL33);
            gli::gl::format Format = Converter.translate(Texture2D.format(), Texture2D.swizzles());

            glTextureStorage2D(_Texture, static_cast<GLint>(Texture2D.levels()), Format.Internal,
                               Texture2D.extent().x, Texture2D.extent().y);

            for (std::size_t Level = 0; Level < Texture2D.levels(); ++Level)
            {
                glTextureSubImage2D(_Texture, static_cast<GLint>(Level), 0, 0,
                                    Texture2D[Level].extent().x, Texture2D[Level].extent().y,
                                    Format.External, Format.Type, Texture2D[Level].data());
            }

            glTextureParameteri(_Texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(_Texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTextureParameteri(_Texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(_Texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            return nullptr;
        }
    }

    FImageData Data = LoadImage(ImageFilePath, bSrgb, bFlipVertically);

    glTextureStorage2D(_Texture, 1, Data.InternalFormat, Data.Width, Data.Height);
    glTextureSubImage2D(_Texture, 0, 0, 0, Data.Width, Data.Height, Data.Format, GL_UNSIGNED_BYTE, Data.Data);
    glGenerateTextureMipmap(_Texture);

    glTextureParameteri(_Texture, GL_TEXTURE_WRAP_S, Data.TexWrapS);
    glTextureParameteri(_Texture, GL_TEXTURE_WRAP_T, Data.TexWrapT);
    glTextureParameteri(_Texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(_Texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return Data.Data;
}

GLubyte* FTexture::CreateCubeMap(const std::string& Filename, bool bSrgb, bool bFlipVertically)
{
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &_Texture);

    if (std::filesystem::path(Filename).extension().string() == ".ddx" ||
        std::filesystem::path(Filename).extension().string() == ".ktx")
    {
        gli::texture_cube Cube(gli::load(GetAssetFullPath(Asset::EAssetType::kTexture, Filename)));
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

    FImageData Data;
    std::string Directory = GetAssetFullPath(EAssetType::kTexture, Filename);

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

FTexture::FImageData FTexture::LoadImage(const std::string& Filename, bool bSrgb, bool bFlipVertically) const
{
    GLint ImageWidth    = 0;
    GLint ImageHeight   = 0;
    GLint ImageChannels = 0;

    stbi_set_flip_vertically_on_load(bFlipVertically);
    GLubyte* ImageData = stbi_load(Filename.c_str(), &ImageWidth, &ImageHeight, &ImageChannels, 0);

    if (!ImageData)
    {
        NpgsCoreError("Can not open image file: \"{}\": No such fire or directory.", Filename);
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
_RUNTIME_END
_NPGS_END
