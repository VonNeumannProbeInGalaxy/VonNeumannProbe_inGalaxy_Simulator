#include "Model.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <memory>
#include <print>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "Engine/AssetLoader/AssetManager.h"
#include "Engine/AssetLoader/GetAssetFullPath.h"
#include "Engine/AssetLoader/Texture.h"

_NPGS_BEGIN
_ASSET_BEGIN

FModel::FModel(const std::string& Filename, const std::string& ShaderName)
{
	InitModel(Filename);
	InitModelTexture(*FAssetManager::GetInstance()->GetAsset<FShader>(ShaderName));
}

FModel::FModel(FModel&& Other) noexcept
	:
	_TexturesCache(std::move(Other._TexturesCache)),
	_Meshes(std::move(Other._Meshes)),
	_Directory(std::move(Other._Directory))
{
}

FModel& FModel::operator=(FModel&& Other) noexcept
{
	if (this != &Other)
	{
		_TexturesCache = std::move(Other._TexturesCache);
		_Meshes        = std::move(Other._Meshes);
		_Directory     = std::move(Other._Directory);
	}

	return *this;
}

void FModel::StaticDraw(const FShader& ModelShader) const
{
	for (const auto& Mesh : _Meshes)
	{
		Mesh->StaticDraw(ModelShader);
	}
}

void FModel::DynamicDraw(const FShader& ModelShader) const
{
	for (const auto& Mesh : _Meshes)
	{
		Mesh->DynamicDraw(ModelShader);
	}
}

void FModel::InitModel(const std::string& Filename)
{
	Assimp::Importer Loader;
	std::string FilePath = GetAssetFullPath(Asset::EAssetType::kModel, Filename);
	const aiScene* Scene = Loader.ReadFile(FilePath, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
										   aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
	{
		std::println("Fatal error: Assimp: {}", Loader.GetErrorString());
		std::system("pause");
		std::exit(EXIT_FAILURE);
	}

	_Directory = std::filesystem::path(FilePath).parent_path().string();

	ProcessNode(Scene->mRootNode, Scene);
}

void FModel::InitModelTexture(const FShader& ModelShader)
{
	for (auto& Mesh : _Meshes)
	{
		Mesh->InitTextures(ModelShader);
	}
}

void FModel::ProcessNode(const aiNode* Node, const aiScene* Scene)
{
	aiMesh* Mesh = nullptr;
	for (std::uint32_t i = 0; i != Node->mNumMeshes; ++i)
	{
		Mesh = Scene->mMeshes[Node->mMeshes[i]];
		_Meshes.emplace_back(ProcessMesh(Mesh, Scene));
	}

	for (std::uint32_t i = 0; i != Node->mNumChildren; ++i)
	{
		ProcessNode(Node->mChildren[i], Scene);
	}
}

std::unique_ptr<FMesh> FModel::ProcessMesh(const aiMesh* Mesh, const aiScene* Scene)
{
	std::vector<std::uint32_t>       Indices;
	std::vector<FMesh::FTextureData> Textures;
	std::vector<FMesh::FVertex>      Vertices;
	FMesh::FVertex                   Vertex;

	for (std::uint32_t i = 0; i != Mesh->mNumVertices; ++i)
	{
		Vertex.Position = glm::vec3(Mesh->mVertices[i].x, Mesh->mVertices[i].y, Mesh->mVertices[i].z);

		if (Mesh->HasNormals())
		{
			Vertex.Normal = glm::vec3(Mesh->mNormals[i].x, Mesh->mNormals[i].y, Mesh->mNormals[i].z);
		}

		if (Mesh->HasTextureCoords(0u))
		{
			Vertex.TexCoords = glm::vec2(Mesh->mTextureCoords[0][i].x, Mesh->mTextureCoords[0][i].y);
			Vertex.Tangent   = glm::vec3(Mesh->mTangents[i].x, Mesh->mTangents[i].y, Mesh->mTangents[i].z);
			Vertex.Bitangent = glm::vec3(Mesh->mBitangents[i].x, Mesh->mBitangents[i].y, Mesh->mBitangents[i].z);
		}
		else
		{
			Vertex.TexCoords = glm::vec2(0.0f);
		}

		Vertices.emplace_back(Vertex);
	}

	aiFace Face;
	for (std::uint32_t i = 0; i != Mesh->mNumFaces; ++i)
	{
		Face = Mesh->mFaces[i];
		for (std::uint32_t j = 0; j != Face.mNumIndices; ++j)
		{
			Indices.emplace_back(Face.mIndices[j]);
		}
	}

	aiMaterial* Material = Scene->mMaterials[Mesh->mMaterialIndex];

	std::vector<FMesh::FTextureData> DiffuseMaps = LoadMaterialTextures(Material, aiTextureType_DIFFUSE, "iDiffuseTex");
	Textures.insert(Textures.end(), std::make_move_iterator(DiffuseMaps.begin()), std::make_move_iterator(DiffuseMaps.end()));
	std::vector<FMesh::FTextureData> SpecularMaps = LoadMaterialTextures(Material, aiTextureType_SPECULAR, "iSpecularTex");
	Textures.insert(Textures.end(), std::make_move_iterator(SpecularMaps.begin()), std::make_move_iterator(SpecularMaps.end()));
	std::vector<FMesh::FTextureData> NormalMaps = LoadMaterialTextures(Material, aiTextureType_NORMALS, "iNormalTex");
	Textures.insert(Textures.end(), std::make_move_iterator(NormalMaps.begin()), std::make_move_iterator(NormalMaps.end()));
	std::vector<FMesh::FTextureData> HeightMaps = LoadMaterialTextures(Material, aiTextureType_AMBIENT, "iHeightTex");
	Textures.insert(Textures.end(), std::make_move_iterator(HeightMaps.begin()), std::make_move_iterator(HeightMaps.end()));

	return std::make_unique<FMesh>(Vertices, Indices, Textures);
}

#pragma warning(push)
#pragma warning(disable : 26800) // For disable a IntelliSense bug
std::vector<FMesh::FTextureData> FModel::LoadMaterialTextures(const aiMaterial* Material, const aiTextureType& TextureType, const std::string& TypeName)
{
	std::vector<FMesh::FTextureData> Textures;
	aiString                         ImageFilename;
	FMesh::FTextureData              MaterialTexture;

	for (std::uint32_t i = 0; i != Material->GetTextureCount(TextureType); ++i)
	{
		Material->GetTexture(TextureType, i, &ImageFilename);

		bool bSkipLoading = false;
		for (const auto& Texture : _TexturesCache)
		{
			if (std::strcmp(Texture.ImageFilename.data(), ImageFilename.C_Str()) == 0)
			{
				Textures.emplace_back(Texture);
				bSkipLoading = true;
				break;
			}
		}

		if (!bSkipLoading)
		{
			std::string ImageFilePath = _Directory + '/' + ImageFilename.C_Str();
			MaterialTexture.Data =
				std::make_shared<FTexture>(FTexture::ETextureType::k2D, ImageFilePath, true, true, false);
			MaterialTexture.TypeName = TypeName;
			MaterialTexture.ImageFilename = ImageFilename.C_Str();
			_TexturesCache.emplace_back(std::move(MaterialTexture));
			Textures.emplace_back(_TexturesCache.back());
		}
	}

	return Textures;
}
#pragma warning(pop)

_ASSET_END
_NPGS_END
