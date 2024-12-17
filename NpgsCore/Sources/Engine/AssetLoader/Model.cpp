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
#include "Engine/AssetLoader/GetAssetFilepath.h"
#include "Engine/AssetLoader/Texture.h"

_NPGS_BEGIN
_ASSET_BEGIN

Model::Model(const std::string& Filename, const std::string& ShaderName)
{
	InitModel(Filename);
	InitModelTexture(*AssetManager::GetInstance()->GetAsset<Shader>(ShaderName));
}

Model::Model(Model&& Other) noexcept
	:
	_TexturesCache(std::move(Other._TexturesCache)),
	_Meshes(std::move(Other._Meshes)),
	_Directory(std::move(Other._Directory))
{
}

Model& Model::operator=(Model&& Other) noexcept
{
	if (this != &Other)
	{
		_TexturesCache = std::move(Other._TexturesCache);
		_Meshes        = std::move(Other._Meshes);
		_Directory     = std::move(Other._Directory);
	}

	return *this;
}

void Model::StaticDraw(const Shader& ModelShader) const
{
	for (const auto& Mesh : _Meshes)
	{
		Mesh->StaticDraw(ModelShader);
	}
}

void Model::DynamicDraw(const Shader& ModelShader) const
{
	for (const auto& Mesh : _Meshes)
	{
		Mesh->DynamicDraw(ModelShader);
	}
}

void Model::InitModel(const std::string& Filename)
{
	Assimp::Importer Loader;
	std::string Filepath = GetAssetFilepath(Asset::AssetType::kModel, Filename);
	const aiScene* Scene = Loader.ReadFile(Filepath, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
										   aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
	{
		std::println("Fatal error: Assimp: {}", Loader.GetErrorString());
		std::system("pause");
		std::exit(EXIT_FAILURE);
	}

	_Directory = std::filesystem::path(Filepath).parent_path().string();

	ProcessNode(Scene->mRootNode, Scene);
}

void Model::InitModelTexture(const Shader& ModelShader)
{
	for (auto& Mesh : _Meshes)
	{
		Mesh->InitTextures(ModelShader);
	}
}

void Model::ProcessNode(const aiNode* Node, const aiScene* Scene)
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

std::unique_ptr<Mesh> Model::ProcessMesh(const aiMesh* Mesh, const aiScene* Scene)
{
	std::vector<std::uint32_t> Indices;
	std::vector<Mesh::Texture> Textures;
	std::vector<Mesh::Vertex>  Vertices;
	Mesh::Vertex               MeshVertex{};

	for (std::uint32_t i = 0; i != Mesh->mNumVertices; ++i)
	{
		MeshVertex.Position = glm::vec3(Mesh->mVertices[i].x, Mesh->mVertices[i].y, Mesh->mVertices[i].z);

		if (Mesh->HasNormals())
		{
			MeshVertex.Normal = glm::vec3(Mesh->mNormals[i].x, Mesh->mNormals[i].y, Mesh->mNormals[i].z);
		}

		if (Mesh->HasTextureCoords(0u))
		{
			MeshVertex.TexCoords = glm::vec2(Mesh->mTextureCoords[0][i].x, Mesh->mTextureCoords[0][i].y);
			MeshVertex.Tangent   = glm::vec3(Mesh->mTangents[i].x, Mesh->mTangents[i].y, Mesh->mTangents[i].z);
			MeshVertex.Bitangent = glm::vec3(Mesh->mBitangents[i].x, Mesh->mBitangents[i].y, Mesh->mBitangents[i].z);
		}
		else
		{
			MeshVertex.TexCoords = glm::vec2(0.0f);
		}

		Vertices.emplace_back(MeshVertex);
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

	std::vector<Mesh::Texture> DiffuseMaps = LoadMaterialTextures(Material, aiTextureType_DIFFUSE, "iDiffuseTex");
	Textures.insert(Textures.end(), std::make_move_iterator(DiffuseMaps.begin()), std::make_move_iterator(DiffuseMaps.end()));
	std::vector<Mesh::Texture> SpecularMaps = LoadMaterialTextures(Material, aiTextureType_SPECULAR, "iSpecularTex");
	Textures.insert(Textures.end(), std::make_move_iterator(SpecularMaps.begin()), std::make_move_iterator(SpecularMaps.end()));
	std::vector<Mesh::Texture> NormalMaps = LoadMaterialTextures(Material, aiTextureType_NORMALS, "iNormalTex");
	Textures.insert(Textures.end(), std::make_move_iterator(NormalMaps.begin()), std::make_move_iterator(NormalMaps.end()));
	std::vector<Mesh::Texture> HeightMaps = LoadMaterialTextures(Material, aiTextureType_AMBIENT, "iHeightTex");
	Textures.insert(Textures.end(), std::make_move_iterator(HeightMaps.begin()), std::make_move_iterator(HeightMaps.end()));

	return std::make_unique<Asset::Mesh>(Vertices, Indices, Textures);
}

#pragma warning(push)
#pragma warning(disable : 26800) // For disable a IntelliSense bug
std::vector<Mesh::Texture> Model::LoadMaterialTextures(const aiMaterial* Material, const aiTextureType& TextureType, const std::string& TypeName)
{
	std::vector<Mesh::Texture> Textures;
	aiString                   ImageFilename;
	Mesh::Texture              MaterialTexture;

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
			std::string ImageFilepath = _Directory + '/' + ImageFilename.C_Str();
			MaterialTexture.Data =
				std::make_shared<Asset::Texture>(Asset::Texture::TextureType::k2D, ImageFilepath, true, true, false);
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
