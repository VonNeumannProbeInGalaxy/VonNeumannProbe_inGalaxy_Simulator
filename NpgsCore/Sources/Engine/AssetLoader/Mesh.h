#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Engine/AssetLoader/Shader.h"
#include "Engine/AssetLoader/Texture.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_ASSET_BEGIN

const int kMaxBoneInfluence = 4;

class Mesh
{
public:
	struct Texture
	{
		std::shared_ptr<const Asset::Texture> Data;
		std::string TypeName;
		std::string ImageFilename;
	};

	struct TextureInfo
	{
		Texture* Texture{};
		GLuint   Unit;
		GLint    UniformLocation;
	};

	struct Vertex
	{
		glm::vec3 Position{};
		glm::vec3 Normal{};
		glm::vec2 TexCoords{};
		glm::vec3 Tangent{};
		glm::vec3 Bitangent{};

		std::array<GLint,   kMaxBoneInfluence> BoneIds{};
		std::array<GLfloat, kMaxBoneInfluence> Weigths{};
	};

public:
	Mesh(const std::vector<Vertex>& Vertices, const std::vector<GLuint>& Indices, const std::vector<Texture>& Textures);
	Mesh(const Mesh&) = delete;
	Mesh(Mesh&& Other) noexcept;
	~Mesh();

	Mesh& operator=(const Mesh&) = delete;
	Mesh& operator=(Mesh&& Other) noexcept;

	void InitTextures(const Shader& ModelShader);
	void StaticDraw(const Shader& ModelShader) const;
	void DynamicDraw(const Shader& ModelShader) const;
	GLuint GetVertexArray() const;
	const std::vector<GLuint>& GetIndices() const;

private:
	void InitStaticVertexArray(const std::vector<Vertex>& Vertices, const std::vector<GLuint>& Indices);
	std::ptrdiff_t GetTextureCount(const std::string& typeName) const;

private:
	std::vector<GLuint>      _Indices;
	std::vector<Texture>     _Textures;
	std::vector<TextureInfo> _TextureInfos;
	GLuint                   _VertexArray;
};

_ASSET_END
_NPGS_END

#include "Mesh.inl"
