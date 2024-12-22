#pragma once

#include <tuple>
#include <utility>
#include <vector>

#include <glad/glad.h>

#include "Engine/AssetLoader/Shader.h"
#include "Engine/Core/Renderers/Buffers/Framebuffer.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN

class FScreenQuadRenderer
{
public:
	FScreenQuadRenderer();
	~FScreenQuadRenderer();

	void SetTextures(const std::vector<std::pair<GLuint, GLuint>>& Targets);
	void SetTexturesFromFramebuffer(const FFramebuffer& Framebuffer, const std::vector<GLuint>& Locations);
	void Draw(const Asset::FShader& ScreenShader, const std::vector<std::tuple<std::string, GLuint, std::size_t>>& Uniforms) const;

private:
	void CreateQuadVertexArray();
	void DrawQuad() const;

private:
	std::vector<GLuint> _Textures;
	std::vector<GLuint> _TextureUnits;
	std::vector<GLuint> _Locations;
	GLuint _QuadVertexArray;
};

_NPGS_END
