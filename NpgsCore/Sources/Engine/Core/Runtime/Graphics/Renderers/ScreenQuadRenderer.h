#pragma once

#include <cstddef>
#include <tuple>
#include <utility>
#include <vector>

#include <glad/glad.h>

#include "Engine/Core/Base/Base.h"
#include "Engine/Core/Runtime/Assets/Shader.h"
#include "Engine/Core/Runtime/Graphics/OpenGL/Framebuffer.h"

_NPGS_BEGIN
_RUNTIME_BEGIN
_GRAPHICS_BEGIN

class FScreenQuadRenderer
{
public:
    FScreenQuadRenderer();
    ~FScreenQuadRenderer();

    void SetTextures(const std::vector<std::pair<GLuint, GLuint>>& Targets);
    void SetTexturesFromFramebuffer(const FFramebuffer& Framebuffer, const std::vector<GLuint>& Locations);

    void Draw(const Runtime::Asset::FShader& ScreenShader,
              const std::vector<std::tuple<std::string, GLuint, std::size_t>>& Uniforms) const;

private:
    void CreateQuadVertexArray();
    void DrawQuad() const;

private:
    std::vector<GLuint> _Textures;
    std::vector<GLuint> _TextureUnits;
    std::vector<GLuint> _Locations;
    GLuint              _QuadVertexArray;
};

_GRAPHICS_END
_RUNTIME_END
_NPGS_END
