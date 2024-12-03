#include "Model.h"

#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <memory>
#include <print>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "GetAssetFilepath.h"
#include "Texture.h"

Model::Model(const std::string& Filename)
{
	Assimp::Importer Loader;
	std::string Filepath = GetAssetFilepath(AssetType::kModel, Filename);
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

GLvoid Model::Draw(const Shader& ModelShader) const
{
	for (const auto& Mesh : _Meshes)
	{
		Mesh->Draw(ModelShader);
	}
}

GLvoid Model::ProcessNode(const aiNode* Node, const aiScene* Scene)
{
	aiMesh* Mesh = nullptr;
	for (GLint i = 0; i != Node->mNumMeshes; ++i)
	{
		Mesh = Scene->mMeshes[Node->mMeshes[i]];
		_Meshes.emplace_back(ProcessMesh(Mesh, Scene));
	}

	for (GLint i = 0; i != Node->mNumChildren; ++i)
	{
		ProcessNode(Node->mChildren[i], Scene);
	}
}

std::unique_ptr<Mesh> Model::ProcessMesh(const aiMesh* Mesh, const aiScene* Scene)
{
	std::vector<GLuint>        Indices;
	std::vector<Mesh::Texture> Textures;
	std::vector<Mesh::Vertex>  Vertices;
	Mesh::Vertex               MeshVertex{};

	for (GLint i = 0; i != Mesh->mNumVertices; ++i)
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

		if (Mesh->mTextureCoords[0])
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
	for (GLint i = 0; i != Mesh->mNumFaces; ++i)
	{
		Face = Mesh->mFaces[i];
		for (GLint j = 0; j != Face.mNumIndices; ++j)
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

	return std::make_unique<::Mesh>(Vertices, Indices, Textures);
}

std::vector<Mesh::Texture> Model::LoadMaterialTextures(const aiMaterial* Material, const aiTextureType& TextureType, const std::string& TypeName)
{
	std::vector<Mesh::Texture> Textures;
	aiString                   ImageFilename;
	Mesh::Texture              MaterialTexture;

	for (GLint i = 0; i != Material->GetTextureCount(TextureType); ++i)
	{
		Material->GetTexture(TextureType, i, &ImageFilename);

		GLboolean bSkipLoading = GL_FALSE;
		for (const auto& kTexture : _Textures)
		{
			if (std::strcmp(kTexture.ImageFilename.data(), ImageFilename.C_Str()) == 0)
			{
				Textures.emplace_back(kTexture);
				bSkipLoading = GL_TRUE;
				break;
			}
		}

		if (!bSkipLoading)
		{
			std::string ImageFilepath = _Directory + '/' + ImageFilename.C_Str();
			MaterialTexture.Data = std::make_shared<::Texture>(::Texture::Type::k2D, ImageFilepath, GL_TRUE, GL_TRUE, GL_FALSE);
			MaterialTexture.TypeName = TypeName;
			MaterialTexture.ImageFilename = ImageFilename.C_Str();
			Textures.emplace_back(MaterialTexture);
		   _Textures.emplace_back(MaterialTexture);
		}
	}

	return Textures;
}
