#pragma once

#include <string>

#include <freetype/freetype.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Shader.h"

// @brief 纹理类，用于创建各种不同的纹理。
class Texture
{
public:
	enum class Type
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
	// @brief 纹理类用于创建 2D 纹理和立方体纹理的构造函数。
	// @param CreateType 纹理类型。如果是立方体纹理类型，则立方体纹理的 6 个面必须命名为 Right、Left、Top、Bottom、Front、Back。
	// @param Filepath 纹理图片文件路径。函数内部已经调用过 GetAssetFilepath，故只需要传递 Textures/ 路径之后的文件名。
	//        例：AwesomeFace.png。
	// @param bSrgb 是否使用 sRGB 纹理。
	// @param bFlipVertically 是否垂直翻转图片。
	// @param bAutoFillFilepath 是否自动填充文件路径。在加载模型的时候自动传递参数，不需要手动调整。
	Texture(Type CreateType, const std::string& Filepath, GLboolean bSrgb = GL_FALSE, GLboolean bFlipVertically = GL_TRUE, GLboolean bAutoFillFilepath = GL_TRUE);

	// @brief 纹理类用于创建字符纹理的构造函数。
	// @param CreateType 纹理类型。
	// @param Face 字体库中的字体。
	Texture(Type CreateType, const FT_Face& Face);

	// @brief 纹理类用于创建帧缓冲纹理附件的构造函数。
	// @param CreateType 纹理类型。
	// @param Width 纹理宽度。
	// @param Height 纹理高度。
	// @param InternalFormat 纹理内部格式。
	// @param Attachment 纹理附件。
	// @param Framebuffer 帧缓冲对象。
	Texture(Type CreateType, GLsizei Width, GLsizei Height, GLenum InternalFormat, GLenum Attachment, GLuint Framebuffer);

	// @brief 纹理类用于创建多重采样纹理的构造函数。
	// @param CreateType 纹理类型。
	// @param Width 纹理宽度。
	// @param Height 纹理高度。
	// @param InternalFormat 纹理内部格式。
	// @param Samples 多重采样数。
	// @param Attachment 纹理附件。
	// @param bFixedSampleLocations 是否固定采样位置。
	// @param Framebuffer 帧缓冲对象。
	Texture(Type CreateType, GLsizei Width, GLsizei Height, GLenum InternalFormat, GLsizei Samples,
			GLenum Attachment, GLboolean bFixedSampleLocations, GLuint Framebuffer);

	// @brief 纹理类用于创建深度纹理的构造函数。
	// @param CreateType 纹理类型。
	// @param Width 纹理宽度。
	// @param Height 纹理高度。
	Texture(Type CreateType, GLsizei Width, GLsizei Height);

	~Texture();

	// @brief 绑定纹理单元。
	// @param ActivatedShader 激活的着色器。
	// @param UniformName 纹理采样器 uniform 名称。
	// @param Unit 纹理单元。
	GLvoid BindTextureUnit(const Shader& ActivatedShader, const std::string& UniformName, GLuint Unit) const;

	// @brief 获取纹理 ID。
	// @return 纹理 ID。
	GLuint GetTexture() const;

	// @brief 获取纹理类型。
	// @return 纹理类型。
	Type GetTextureType() const;

private:
	ImageData LoadImage(const std::string& ImageFilename, GLboolean bSrgb, GLboolean bFlipVertically, GLboolean bAutoFillFilepath) const;

private:
	GLuint _Texture;
	Type   _TextureType;
};

#include "Texture.inl"
