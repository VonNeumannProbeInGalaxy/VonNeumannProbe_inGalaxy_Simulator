#include "Model.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <memory>
#include <print>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "Engine/AssetLoader/GetAssetFilepath.h"
#include "Engine/AssetLoader/Texture.h"

_NPGS_BEGIN
_ASSET_BEGIN

Model::Model(const std::string& Filename)
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

Model::Model(Model&& Other) noexcept
	:
	_Textures(std::move(Other._Textures)),
	_Meshes(std::move(Other._Meshes)),
	_Directory(std::move(Other._Directory))
{
}

Model& Model::operator=(Model&& Other) noexcept
{
	if (this != &Other)
	{
		_Textures  = std::move(Other._Textures);
		_Meshes    = std::move(Other._Meshes);
		_Directory = std::move(Other._Directory);
	}

	return *this;
}

void Model::Draw(const Shader& ModelShader) const
{
	for (const auto& Mesh : _Meshes)
	{
		Mesh->Draw(ModelShader);
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
		glm::vec3 Vector(Mesh->mVertices[i].x, Mesh->mVertices[i].y, Mesh->mVertices[i].z);
		MeshVertex.Position = Vector;

		if (Mesh->HasNormals())
		{
			Vector.x = Mesh->mNormals[i].x;
			Vector.y = Mesh->mNormals[i].y;
			Vector.z = Mesh->mNormals[i].z;
			MeshVertex.Normal = Vector;
		}

		if (Mesh->HasTextureCoords(0))
		{
			Vector.x = Mesh->mTextureCoords[0][i].x;
			Vector.y = Mesh->mTextureCoords[0][i].y;
			MeshVertex.TexCoords.x = Vector.x;
			MeshVertex.TexCoords.y = Vector.y;

			Vector.x = Mesh->mTangents[i].x;
			Vector.y = Mesh->mTangents[i].y;
			Vector.z = Mesh->mTangents[i].z;
			MeshVertex.Tangent = Vector;

			Vector.x = Mesh->mBitangents[i].x;
			Vector.y = Mesh->mBitangents[i].y;
			Vector.z = Mesh->mBitangents[i].z;
			MeshVertex.Bitangent = Vector;
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
	Textures.insert(Textures.end(), DiffuseMaps.begin(), DiffuseMaps.end());
	std::vector<Mesh::Texture> SpecularMaps = LoadMaterialTextures(Material, aiTextureType_SPECULAR, "iSpecularTex");
	Textures.insert(Textures.end(), SpecularMaps.begin(), SpecularMaps.end());
	std::vector<Mesh::Texture> NormalMaps = LoadMaterialTextures(Material, aiTextureType_NORMALS, "iNormalTex");
	Textures.insert(Textures.end(), NormalMaps.begin(), NormalMaps.end());
	std::vector<Mesh::Texture> HeightMaps = LoadMaterialTextures(Material, aiTextureType_AMBIENT, "iHeightTex");
	Textures.insert(Textures.end(), HeightMaps.begin(), HeightMaps.end());

	return std::make_unique<Asset::Mesh>(Vertices, Indices, Textures);
}

std::vector<Mesh::Texture> Model::LoadMaterialTextures(const aiMaterial* Material, const aiTextureType& TextureType, const std::string& TypeName)
{
	std::vector<Mesh::Texture> Textures;
	aiString                   ImageFilename;
	Mesh::Texture              MaterialTexture;

	for (std::uint32_t i = 0; i != Material->GetTextureCount(TextureType); ++i)
	{
		Material->GetTexture(TextureType, i, &ImageFilename);

		bool bSkipLoading = false;
		for (const auto& kTexture : _Textures)
		{
			if (std::strcmp(kTexture.ImageFilename.data(), ImageFilename.C_Str()) == 0)
			{
				Textures.emplace_back(kTexture);
				bSkipLoading = true;
				break;
			}
		}

		if (!bSkipLoading)
		{
			std::string ImageFilepath = _Directory + '/' + ImageFilename.C_Str();
			MaterialTexture.Data = std::make_shared<Asset::Texture>(Asset::Texture::TextureType::k2D, ImageFilepath, true, true, false);
			MaterialTexture.TypeName = TypeName;
			MaterialTexture.ImageFilename = ImageFilename.C_Str();
			Textures.emplace_back(MaterialTexture);
		   _Textures.emplace_back(MaterialTexture);
		}
	}

	return Textures;
}

_ASSET_END
_NPGS_END
