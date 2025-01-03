#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <glad/glad.h>
#include "Engine/Core/Base/Base.h"

_NPGS_BEGIN

class FFramebuffer
{
public:
	enum class EAttachmentType : std::uint32_t
	{
		kColor        = Bit(1),
		kDepthStencil = Bit(2)
	};

	enum class EFramebufferType
	{
		kDefault,
		kMsaa
	};

public:
	FFramebuffer(EAttachmentType AttachmentType, GLsizei Width, GLsizei Height,
				 GLenum InternalFormat, GLsizei Samples, int AttachmentCount);

	FFramebuffer(const FFramebuffer&) = delete;
	FFramebuffer(FFramebuffer&& Other) noexcept;
	~FFramebuffer();

	FFramebuffer& operator=(const FFramebuffer&) = delete;
	FFramebuffer& operator=(FFramebuffer&& Other) noexcept;

	void Bind(EFramebufferType Type = EFramebufferType::kMsaa) const;
	void Blit() const;
	void Resize(GLsizei Width, GLsizei Height);
	GLuint GetColorAttachment(std::size_t Index) const;
	GLuint GetDepthStencilAttachment() const;

private:
	void InitConfig();
	void CreateFramebuffers();
	GLuint BlitRenderbufferToTexture(GLsizei Width, GLsizei Height) const;

private:
	std::vector<GLuint> _Framebuffers;
	std::vector<GLuint> _Textures;
	GLsizei             _Width;
	GLsizei             _Height;
	EAttachmentType     _AttachmentType;
	int                 _AttachmentCount;
	GLenum              _InternalFormat;
	GLsizei             _Samples;
	GLuint              _Renderbuffer;
	bool				_bEnableMsaa;
	bool                _bHasColorAttachment;
	bool                _bHasDepthStencilAttachment;
};

_NPGS_END

#include "Framebuffer.inl"
