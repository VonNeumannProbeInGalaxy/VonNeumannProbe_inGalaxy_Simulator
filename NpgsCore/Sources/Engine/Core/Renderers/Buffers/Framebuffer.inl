#pragma once

#include "Framebuffer.h"

_NPGS_BEGIN

NPGS_INLINE GLuint FFramebuffer::GetColorAttachment(std::size_t Index) const
{
	if (!_bHasColorAttachment || Index > _AttachmentCount)
	{
		return 0;
	}

	return _bEnableMsaa ? _Textures[Index * 2] : _Textures[Index];
}

NPGS_INLINE GLuint FFramebuffer::GetDepthStencilAttachment() const
{
	if (!_bHasDepthStencilAttachment)
	{
		return 0;
	}

	return BlitRenderbufferToTexture(_Width, _Height);
}

_NPGS_END
