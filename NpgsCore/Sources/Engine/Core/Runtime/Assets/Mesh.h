#pragma once

#include <cstddef>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Engine/Core/Base/Base.h"
#include "Engine/Core/Runtime/Assets/Shader.h"
#include "Engine/Core/Runtime/Assets/Texture.h"

_NPGS_BEGIN
_RUNTIME_BEGIN
_ASSET_BEGIN

constexpr int kMaxBoneInfluence = 4;

class FMesh
{
public:
    struct FTextureData
    {
        std::shared_ptr<const FTexture> Data;
        std::string TypeName;
        std::string ImageFilename;
    };

    struct FTextureInfo
    {
        FTextureData* Handle{};
        GLuint Unit{};
        GLint  UniformLocation{};
    };

    struct FVertex
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
    FMesh(const std::vector<FVertex>& Vertices, const std::vector<GLuint>& Indices, const std::vector<FTextureData>& Textures);
    FMesh(const FMesh&) = delete;
    FMesh(FMesh&& Other) noexcept;
    ~FMesh();

    FMesh& operator=(const FMesh&) = delete;
    FMesh& operator=(FMesh&& Other) noexcept;

    void InitTextures(const FShader& ModelShader);
    void StaticDraw(const FShader& ModelShader) const;
    void DynamicDraw(const FShader& ModelShader) const;
    GLuint GetVertexArray() const;
    const std::vector<GLuint>& GetIndices() const;

private:
    void InitStaticVertexArray(const std::vector<FVertex>& Vertices);
    std::ptrdiff_t GetTextureCount(const std::string& typeName) const;

private:
    std::vector<GLuint>       _Indices;
    std::vector<FTextureData> _Textures;
    std::vector<FTextureInfo> _TextureInfos;
    GLuint                    _VertexArray;
};

_ASSET_END
_RUNTIME_END
_NPGS_END

#include "Mesh.inl"
