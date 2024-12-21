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

class FModel
{
public:
	FModel(const std::string& Filename, const std::string& ShaderName);
	FModel(const FModel&) = delete;
	FModel(FModel&& Other) noexcept;
	~FModel() = default;

	FModel& operator=(const FModel&) = delete;
	FModel& operator=(FModel&& Other) noexcept;

	void StaticDraw(const FShader& ModelShader) const;
	void DynamicDraw(const FShader& ModelShader) const;
	const std::vector<std::unique_ptr<FMesh>>& GetMeshes() const;

private:
	void InitModel(const std::string& Filename);
	void InitModelTexture(const FShader& ModelShader);
	void ProcessNode(const aiNode* Node, const aiScene* Scene);
	std::unique_ptr<FMesh> ProcessMesh(const aiMesh* Mesh, const aiScene* Scene);
	std::vector<FMesh::FTextureData> LoadMaterialTextures(const aiMaterial* Material, const aiTextureType& TextureType,
														  const std::string& TypeName);

private:
	std::vector<FMesh::FTextureData>    _TexturesCache;
	std::vector<std::unique_ptr<FMesh>> _Meshes;
	std::string                         _Directory;
};

_ASSET_END
_NPGS_END

#include "Model.inl"
