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
	Model(const std::string& Filename, const std::string& ShaderName);
	Model(const Model&) = delete;
	Model(Model&& Other) noexcept;
	~Model() = default;

	Model& operator=(const Model&) = delete;
	Model& operator=(Model&& Other) noexcept;

	void StaticDraw(const Shader& ModelShader) const;
	void DynamicDraw(const Shader& ModelShader) const;
	const std::vector<std::unique_ptr<Mesh>>& GetMeshes() const;

private:
	void InitModel(const std::string& Filename);
	void InitModelTexture(const Shader& ModelShader);
	void ProcessNode(const aiNode* Node, const aiScene* Scene);
	std::unique_ptr<Mesh> ProcessMesh(const aiMesh* Mesh, const aiScene* Scene);
	std::vector<Mesh::Texture> LoadMaterialTextures(const aiMaterial* Material, const aiTextureType& TextureType, const std::string& TypeName);

private:
	std::vector<Mesh::Texture>         _TexturesCache;
	std::vector<std::unique_ptr<Mesh>> _Meshes;
	std::string                        _Directory;
};

_ASSET_END
_NPGS_END

#include "Model.inl"
