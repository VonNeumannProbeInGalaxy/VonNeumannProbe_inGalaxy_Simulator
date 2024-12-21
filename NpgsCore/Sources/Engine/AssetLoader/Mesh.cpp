#include "Mesh.h"

#include <cstddef>

_NPGS_BEGIN
_ASSET_BEGIN

Mesh::Mesh(const std::vector<Vertex>& Vertices, const std::vector<GLuint>& Indices, const std::vector<Texture>& Textures)
	: _Indices(Indices), _Textures(Textures), _VertexArray(0)
{
	InitStaticVertexArray(Vertices);
}

Mesh::Mesh(Mesh&& Other) noexcept
	:
	_Indices(std::move(Other._Indices)),
	_Textures(std::move(Other._Textures)),
	_VertexArray(Other._VertexArray)
{
	Other._VertexArray = 0;
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &_VertexArray);
}

Mesh& Mesh::operator=(Mesh&& Other) noexcept
{
	if (this != &Other)
	{
		glDeleteVertexArrays(1, &_VertexArray);

		_Indices           = std::move(Other._Indices);
		_Textures          = std::move(Other._Textures);
		_VertexArray       = Other._VertexArray;
		Other._VertexArray = 0;
	}

	return *this;
}

void Mesh::InitTextures(const Shader& ModelShader)
{
	GLuint TextureUnit = 0;
	for (auto& Texture : _Textures)
	{
		TextureInfo Info;
		Info.Handle          = &Texture;
		Info.Unit            = TextureUnit++;
		Info.UniformLocation =
			ModelShader.GetUniformLocation(Texture.TypeName + std::to_string(GetTextureCount(Texture.TypeName)));
		_TextureInfos.emplace_back(Info);

		Texture.Data->BindTextureUnit(Info.Unit);
	}
}

void Mesh::StaticDraw(const Shader& ModelShader) const
{
	for (const auto& Info : _TextureInfos)
	{
		glUniform1i(Info.UniformLocation, Info.Unit);
	}

	glBindVertexArray(_VertexArray);
	glDrawElements(GL_TRIANGLES, static_cast<GLuint>(_Indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::DynamicDraw(const Shader& ModelShader) const
{
	for (const auto& Info : _TextureInfos)
	{
		Info.Handle->Data->BindTextureUnit(ModelShader, Info.Handle->TypeName, Info.Unit);
		glUniform1i(Info.UniformLocation, Info.Unit);
	}

	glBindVertexArray(_VertexArray);
	glDrawElements(GL_TRIANGLES, static_cast<GLuint>(_Indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::InitStaticVertexArray(const std::vector<Vertex>& Vertices)
{
	GLuint VertexBuffer  = 0;
	GLuint ElementBuffer = 0;

	glCreateVertexArrays(1, &_VertexArray);
	glCreateBuffers(1, &VertexBuffer);
	glCreateBuffers(1, &ElementBuffer);

	glNamedBufferData(VertexBuffer,  Vertices.size() * sizeof(Vertex), Vertices.data(), GL_STATIC_DRAW);
	glNamedBufferData(ElementBuffer, _Indices.size() * sizeof(GLuint), _Indices.data(), GL_STATIC_DRAW);

	glVertexArrayVertexBuffer(_VertexArray, 0, VertexBuffer, 0, sizeof(Vertex));
	glVertexArrayElementBuffer(_VertexArray, ElementBuffer);

	for (GLuint i = 0; i <= 6; ++i)
	{
		glEnableVertexArrayAttrib(_VertexArray, i);
	}

	glVertexArrayAttribFormat(_VertexArray, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Position));
	glVertexArrayAttribFormat(_VertexArray, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Normal));
	glVertexArrayAttribFormat(_VertexArray, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, TexCoords));
	glVertexArrayAttribFormat(_VertexArray, 3, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Tangent));
	glVertexArrayAttribFormat(_VertexArray, 4, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Bitangent));
	glVertexArrayAttribFormat(_VertexArray, 5, 4, GL_INT,   GL_FALSE, offsetof(Vertex, BoneIds));
	glVertexArrayAttribFormat(_VertexArray, 6, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, Weigths));

	for (GLuint i = 0; i <= 6; ++i)
	{
		glVertexArrayAttribBinding(_VertexArray, i, 0);
	}

	glDeleteBuffers(1, &VertexBuffer);
	glDeleteBuffers(1, &ElementBuffer);
}

_ASSET_END
_NPGS_END
