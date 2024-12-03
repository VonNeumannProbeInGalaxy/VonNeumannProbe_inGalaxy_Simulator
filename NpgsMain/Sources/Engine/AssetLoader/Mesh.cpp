#include "Mesh.h"

#include <cstddef>

Mesh::Mesh(const std::vector<Vertex>& Vertices, const std::vector<GLuint>& Indices, const std::vector<Texture>& Textures)
	: _Indices(Indices), _Textures(Textures), _VertexArray(0)
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

	for (GLint i = 0; i <= 6; ++i)
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

	for (GLint i = 0; i <= 6; ++i)
	{
		glVertexArrayAttribBinding(_VertexArray, i, 0);
	}

	glDeleteBuffers(1, &VertexBuffer);
	glDeleteBuffers(1, &ElementBuffer);
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &_VertexArray);
}

GLvoid Mesh::Draw(const Shader& ModelShader) const
{
	GLuint DiffuseIndex  = 0;
	GLuint SpecularIndex = 0;
	GLuint NormalIndex   = 0;
	GLuint HeightIndex   = 0;

	for (GLuint i = 0; i != _Textures.size(); ++i)
	{
		std::string Index;
		std::string UniformName = _Textures[i].TypeName;

		if (UniformName == "iDiffuseTex")
		{
			Index = std::to_string(DiffuseIndex++);
		}
		else if (UniformName == "iSpecularTex")
		{
			Index = std::to_string(SpecularIndex++);
		}
		else if (UniformName == "iNormalTex")
		{
			Index = std::to_string(NormalIndex++);
		}
		else if (UniformName == "iHeightTex")
		{
			Index = std::to_string(HeightIndex++);
		}

		_Textures[i].Data->BindTextureUnit(ModelShader, UniformName + Index, i);
	}

	glBindVertexArray(_VertexArray);
	glDrawElements(GL_TRIANGLES, static_cast<GLuint>(_Indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
