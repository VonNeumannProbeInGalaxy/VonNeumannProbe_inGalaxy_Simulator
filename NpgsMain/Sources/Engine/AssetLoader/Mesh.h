#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture.h"

const GLint kMaxBoneInfluence = 4;

// @brief Mesh 网格类，用于加载和绘制网格模型。
//        该类不会手动调用，故省略注释。
class Mesh
{
public:
	struct Texture
	{
		std::shared_ptr<const ::Texture> Data;
		std::string TypeName;
		std::string ImageFilename;
	};

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;

		std::array<GLint,   kMaxBoneInfluence> BoneIds;
		std::array<GLfloat, kMaxBoneInfluence> Weigths;
	};

public:
	Mesh() = default;
	Mesh(const std::vector<Vertex>& Vertices, const std::vector<GLuint>& Indices, const std::vector<Texture>& Textures);
	~Mesh();

	GLvoid Draw(const Shader& ModelShader) const;

public:
	GLuint GetVertexArray() const;
	const std::vector<GLuint>& GetIndices() const;

private:
	std::vector<GLuint>  _Indices;
	std::vector<Texture> _Textures;
	GLuint               _VertexArray;
};

#include "Mesh.inl"
