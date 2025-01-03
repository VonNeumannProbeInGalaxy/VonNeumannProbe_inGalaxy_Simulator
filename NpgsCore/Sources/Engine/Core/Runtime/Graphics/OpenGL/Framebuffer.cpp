#include "Framebuffer.h"

#include <cstdlib>
#include <utility>
#include "Engine/Utils/Logger.h"

_NPGS_BEGIN
_RUNTIME_BEGIN
_GRAPHICS_BEGIN

FFramebuffer::FFramebuffer(EAttachmentType AttachmentType, GLsizei Width, GLsizei Height,
                           GLenum InternalFormat, GLsizei Samples, int AttachmentCount)
    :
    _Width(Width),
    _Height(Height),
    _AttachmentType(AttachmentType),
    _AttachmentCount(AttachmentCount),
    _InternalFormat(InternalFormat),
    _Samples(Samples),
    _Renderbuffer(0),
    _bEnableMsaa(false),
    _bHasColorAttachment(false),
    _bHasDepthStencilAttachment(false)
{
    InitConfig();
    CreateFramebuffers();
}

FFramebuffer::FFramebuffer(FFramebuffer&& Other) noexcept
    :
    _Textures(std::move(Other._Textures)),
    _Framebuffers(std::move(Other._Framebuffers)),
    _Width(std::exchange(Other._Width, 0)),
    _Height(std::exchange(Other._Height, 0)),
    _AttachmentType(std::exchange(Other._AttachmentType, EAttachmentType::kColor)),
    _AttachmentCount(std::exchange(Other._AttachmentCount, 0)),
    _InternalFormat(std::exchange(Other._InternalFormat, 0)),
    _Samples(std::exchange(Other._Samples, 0)),
    _Renderbuffer(std::exchange(Other._Renderbuffer, 0)),
    _bEnableMsaa(std::exchange(Other._bEnableMsaa, false)),
    _bHasColorAttachment(std::exchange(Other._bHasColorAttachment, false)),
    _bHasDepthStencilAttachment(std::exchange(Other._bHasDepthStencilAttachment, false))
{
}

FFramebuffer::~FFramebuffer()
{
    if (_Framebuffers.size() > 0 && _Framebuffers[0])
    {
        glDeleteFramebuffers(static_cast<GLsizei>(_Framebuffers.size()), _Framebuffers.data());
    }
    if (_Textures.size() > 0 && _Textures[0])
    {
        glDeleteTextures(static_cast<GLsizei>(_Textures.size()), _Textures.data());
    }
    if (_Renderbuffer)
    {
        glDeleteRenderbuffers(1, &_Renderbuffer);
    }

    _Framebuffers.clear();
    _Textures.clear();
}


FFramebuffer& FFramebuffer::operator=(FFramebuffer&& Other) noexcept
{
    if (this != &Other)
    {
        if (_Framebuffers.size() > 0 && _Framebuffers[0])
        {
            glDeleteFramebuffers(static_cast<GLsizei>(_Framebuffers.size()), _Framebuffers.data());
        }
        if (_Textures.size() > 0 && _Textures[0])
        {
            glDeleteTextures(static_cast<GLsizei>(_Textures.size()), _Textures.data());
        }
        if (_Renderbuffer)
        {
            glDeleteRenderbuffers(1, &_Renderbuffer);
        }

        _Framebuffers               = std::move(Other._Framebuffers);
        _Textures                   = std::move(Other._Textures);
        _Width                      = std::exchange(Other._Width, 0);
        _Height                     = std::exchange(Other._Height, 0);
        _AttachmentType             = std::exchange(Other._AttachmentType, EAttachmentType::kColor);
        _AttachmentCount            = std::exchange(Other._AttachmentCount, 0);
        _InternalFormat             = std::exchange(Other._InternalFormat, 0);
        _Samples                    = std::exchange(Other._Samples, 0);
        _Renderbuffer               = std::exchange(Other._Renderbuffer, 0);
        _bEnableMsaa                = std::exchange(Other._bEnableMsaa, false);
        _bHasColorAttachment        = std::exchange(Other._bHasColorAttachment, false);
        _bHasDepthStencilAttachment = std::exchange(Other._bHasDepthStencilAttachment, false);
    }

    return *this;
}

void FFramebuffer::Bind(EFramebufferType Type) const
{
    if (Type == EFramebufferType::kDefault)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _Framebuffers[0]);
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _Framebuffers[1]);
    }
}

void FFramebuffer::Blit() const
{
    if (!_bEnableMsaa)
    {
        return;
    }

    for (int i = 0; i < _AttachmentCount; ++i)
    {
        glNamedFramebufferReadBuffer(_Framebuffers[1], GL_COLOR_ATTACHMENT0 + i);
        glNamedFramebufferDrawBuffer(_Framebuffers[0], GL_COLOR_ATTACHMENT0 + i);
        glBlitNamedFramebuffer(_Framebuffers[1], _Framebuffers[0], 0, 0, _Width, _Height,
                               0, 0, _Width, _Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    if (_bHasDepthStencilAttachment)
    {
        glBlitNamedFramebuffer(_Framebuffers[1], _Framebuffers[0], 0, 0, _Width, _Height,
                               0, 0, _Width, _Height, GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
    }

    std::vector<GLenum> DrawBuffers;
    for (int i = 0; i < _AttachmentCount; ++i)
    {
        DrawBuffers.emplace_back(GL_COLOR_ATTACHMENT0 + i);
    }

    glNamedFramebufferDrawBuffers(_Framebuffers[0], static_cast<GLsizei>(DrawBuffers.size()), DrawBuffers.data());
}

void FFramebuffer::Resize(GLsizei Width, GLsizei Height)
{
    _Width  = Width;
    _Height = Height;

    if (_Textures.size() > 0 && _Textures[0])
    {
        glDeleteTextures(static_cast<GLsizei>(_Textures.size()), _Textures.data());
        _Textures.clear();
    }
    if (_Renderbuffer)
    {
        glDeleteRenderbuffers(1, &_Renderbuffer);
        _Renderbuffer = 0;
    }

    InitConfig();
    CreateFramebuffers();
}

void FFramebuffer::InitConfig()
{
    _bEnableMsaa = _Samples > 1;
    _bHasColorAttachment =
        static_cast<std::uint32_t>(_AttachmentType) & static_cast<std::uint32_t>(EAttachmentType::kColor);
    _bHasDepthStencilAttachment =
        static_cast<std::uint32_t>(_AttachmentType) & static_cast<std::uint32_t>(EAttachmentType::kDepthStencil);

    if (_bEnableMsaa)
    {
        _Framebuffers.resize(2);
    }
    else
    {
        _Framebuffers.resize(1);
    }

    std::size_t TextureCount = 0;
    if (_bHasColorAttachment)
    {
        TextureCount += _bEnableMsaa ? (_AttachmentCount * 2) : _AttachmentCount;
    }

    _Textures.resize(TextureCount);
}

void FFramebuffer::CreateFramebuffers()
{
    std::size_t TextureIndex = 0;
    std::vector<GLenum> DrawBuffers;

    glCreateFramebuffers(static_cast<GLsizei>(_Framebuffers.size()), _Framebuffers.data());

    if (_bEnableMsaa)
    {
        if (_bHasColorAttachment)
        {
            for (int i = 0; i != _AttachmentCount; ++i)
            {
                DrawBuffers.emplace_back(GL_COLOR_ATTACHMENT0 + i);

                glCreateTextures(GL_TEXTURE_2D, 1, &_Textures[TextureIndex]);
                glTextureStorage2D(_Textures[TextureIndex], 1, _InternalFormat, _Width, _Height);
                glTextureParameteri(_Textures[TextureIndex], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTextureParameteri(_Textures[TextureIndex], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glNamedFramebufferTexture(_Framebuffers[0], GL_COLOR_ATTACHMENT0 + i, _Textures[TextureIndex], 0);
                ++TextureIndex;

                glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &_Textures[TextureIndex]);
                glTextureStorage2DMultisample(_Textures[TextureIndex], _Samples, _InternalFormat, _Width, _Height, GL_TRUE);
                glNamedFramebufferTexture(_Framebuffers[1], GL_COLOR_ATTACHMENT0 + i, _Textures[TextureIndex], 0);
                ++TextureIndex;
            }

            glNamedFramebufferDrawBuffers(_Framebuffers[0], static_cast<GLsizei>(DrawBuffers.size()), DrawBuffers.data());
            glNamedFramebufferDrawBuffers(_Framebuffers[1], static_cast<GLsizei>(DrawBuffers.size()), DrawBuffers.data());
        }

        if (_bHasDepthStencilAttachment)
        {
            glCreateRenderbuffers(1, &_Renderbuffer);
            glNamedRenderbufferStorageMultisample(_Renderbuffer, _Samples, GL_DEPTH24_STENCIL8, _Width, _Height);
            glNamedFramebufferRenderbuffer(_Framebuffers[1], GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _Renderbuffer);
        }
    }
    else
    {
        if (_bHasColorAttachment)
        {
            for (int i = 0; i != _AttachmentCount; ++i)
            {
                glCreateTextures(GL_TEXTURE_2D, 1, &_Textures[TextureIndex]);
                glTextureStorage2D(_Textures[TextureIndex], 1, _InternalFormat, _Width, _Height);
                glTextureParameteri(_Textures[TextureIndex], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTextureParameteri(_Textures[TextureIndex], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glNamedFramebufferTexture(_Framebuffers[0], GL_COLOR_ATTACHMENT0 + i, _Textures[TextureIndex], 0);
                DrawBuffers.emplace_back(GL_COLOR_ATTACHMENT0 + i);
                ++TextureIndex;
            }

            glNamedFramebufferDrawBuffers(_Framebuffers[0], static_cast<GLsizei>(DrawBuffers.size()), DrawBuffers.data());
        }

        if (_bHasDepthStencilAttachment)
        {
            glCreateRenderbuffers(1, &_Renderbuffer);
            glNamedRenderbufferStorage(_Renderbuffer, GL_DEPTH24_STENCIL8, _Width, _Height);
            glNamedFramebufferRenderbuffer(_Framebuffers[0], GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _Renderbuffer);
        }
    }

    for (const auto& Framebuffer : _Framebuffers)
    {
        if (glCheckNamedFramebufferStatus(Framebuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            NpgsCoreError("Framebuffer {} is not complete.", Framebuffer);
            std::exit(EXIT_FAILURE);
        }
    }
}

GLuint FFramebuffer::BlitRenderbufferToTexture(GLsizei Width, GLsizei Height) const
{
    if (!_bHasDepthStencilAttachment)
    {
        return 0;
    }

    GLuint Texture = 0;
    glCreateTextures(GL_TEXTURE_2D, 1, &Texture);
    glTextureStorage2D(Texture, 1, GL_DEPTH24_STENCIL8, Width, Height);

    GLuint TemporaryFramebuffer = 0;
    glCreateFramebuffers(1, &TemporaryFramebuffer);
    glNamedFramebufferTexture(TemporaryFramebuffer, GL_DEPTH_STENCIL_ATTACHMENT, Texture, 0);

    glBlitNamedFramebuffer(_Framebuffers[_bEnableMsaa ? 1 : 0], TemporaryFramebuffer, 0, 0, _Width, _Height,
                           0, 0, Width, Height, GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
    glDeleteFramebuffers(1, &TemporaryFramebuffer);

    return Texture;
}

_GRAPHICS_END
_RUNTIME_END
_NPGS_END
