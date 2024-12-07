#pragma once

#include <memory>
#include <vector>
#include <string>

#include <assimp/scene.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Engine/AssetLoader/Mesh.h"
#include "Engine/AssetLoader/Shader.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_ASSET_BEGIN

class Model
{
public:
	Model() = default;
	Model(const std::string& Filename);
	Model(const Model&) = delete;
	Model(Model&& Other) noexcept;
	~Model() = default;

	Model& operator=(const Model&) = delete;
	Model& operator=(Model&& Other) noexcept;

	void Draw(const Shader& ModelShader) const;

private:
	void ProcessNode(const aiNode* Node, const aiScene* Scene);
	std::unique_ptr<Mesh> ProcessMesh(const aiMesh* Mesh, const aiScene* Scene);
	std::vector<Mesh::Texture> LoadMaterialTextures(const aiMaterial* Material, const aiTextureType& TextureType, const std::string& TypeName);
	const std::vector<std::unique_ptr<Mesh>>& GetMeshes() const;

private:
	std::vector<Mesh::Texture>         _Textures;
	std::vector<std::unique_ptr<Mesh>> _Meshes;
	std::string                        _Directory;
};

_ASSET_END
_NPGS_END

#include "Model.inl"
