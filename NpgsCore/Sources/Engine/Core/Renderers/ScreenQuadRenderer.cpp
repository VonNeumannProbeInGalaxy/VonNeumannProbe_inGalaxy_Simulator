#include "ScreenQuadRenderer.h"

_NPGS_BEGIN

FScreenQuadRenderer::FScreenQuadRenderer()
	: _QuadVertexArray(0)
{
	CreateQuadVertexArray();
}

FScreenQuadRenderer::~FScreenQuadRenderer()
{
	glDeleteVertexArrays(1, &_QuadVertexArray);
}

void FScreenQuadRenderer::SetTextures(const std::vector<std::pair<GLuint, GLuint>>& Targets)
{
	_Textures.clear();
	_TextureUnits.clear();
	_Locations.clear();
	for (const auto& [Texture, Location] : Targets)
	{
		_Textures.emplace_back(Texture);
		_Locations.emplace_back(Location);
	}
}

void FScreenQuadRenderer::SetTexturesFromFramebuffer(const FFramebuffer& Framebuffer, const std::vector<GLuint>& Locations)
{
	std::vector<std::pair<GLuint, GLuint>> Targets;
	for (std::size_t i = 0; i != Locations.size(); ++i)
	{
		Targets.emplace_back(Framebuffer.GetColorAttachment(i), Locations[i]);
	}

	SetTextures(Targets);
}

void FScreenQuadRenderer::Draw(const Asset::FShader& ScreenShader,
							   const std::vector<std::tuple<std::string, GLuint, std::size_t>>& Uniforms) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	ScreenShader.UseProgram();
	for (const auto& [UniformName, UniformValue, AttachmentIndex] : Uniforms)
	{
		glBindTextureUnit(UniformValue, _Textures[AttachmentIndex]);
		ScreenShader.SetUniform1i(UniformName, static_cast<GLint>(UniformValue));
	}

	DrawQuad();
}

void FScreenQuadRenderer::CreateQuadVertexArray()
{
	std::vector<GLfloat> QuadVertices
	{
		-1.0f,  1.0f,  0.0f,  1.0f,
		-1.0f, -1.0f,  0.0f,  0.0f,
		 1.0f, -1.0f,  1.0f,  0.0f,

		-1.0f,  1.0f,  0.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,  0.0f,
		 1.0f,  1.0f,  1.0f,  1.0f
	};

	GLuint QuadVertexBuffer = 0;
	glCreateBuffers(1, &QuadVertexBuffer);
	glNamedBufferData(QuadVertexBuffer, QuadVertices.size() * sizeof(GLfloat), QuadVertices.data(), GL_STATIC_DRAW);

	glCreateVertexArrays(1, &_QuadVertexArray);
	glVertexArrayVertexBuffer(_QuadVertexArray, 0, QuadVertexBuffer, 0, 4 * sizeof(GLfloat));
	glEnableVertexArrayAttrib(_QuadVertexArray, 0);
	glEnableVertexArrayAttrib(_QuadVertexArray, 1);
	glVertexArrayAttribFormat(_QuadVertexArray, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(_QuadVertexArray, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat));
	glVertexArrayAttribBinding(_QuadVertexArray, 0, 0);
	glVertexArrayAttribBinding(_QuadVertexArray, 1, 0);

	glDeleteBuffers(1, &QuadVertexBuffer);
}

void FScreenQuadRenderer::DrawQuad() const
{
	glBindVertexArray(_QuadVertexArray);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

_NPGS_END
