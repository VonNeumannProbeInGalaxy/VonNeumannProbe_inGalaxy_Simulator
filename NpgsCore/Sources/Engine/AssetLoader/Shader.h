#pragma once

#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Engine/Core/Base.h"

_NPGS_BEGIN
_ASSET_BEGIN

class Shader
{
public:
	enum class UniformBlockLayout
	{
		kShared,
		kStd140,
		kStd430
	};

	template <typename T>
	class UniformBlockUpdater
	{
	public:
		UniformBlockUpdater(GLuint Buffer, GLint Offset)
			: _Buffer(Buffer), _Offset(Offset)
		{
		}

		UniformBlockUpdater& operator=(const T& Value)
		{
			Commit(Value);
			return *this;
		}

		void Commit(const T& Value) const
		{
			glNamedBufferSubData(_Buffer, _Offset, sizeof(T), &Value);
		}

	private:
		GLuint _Buffer;
		GLint  _Offset;
	};

private:
	struct Source
	{
		std::string Data;
		std::string Filepath;
		bool        bHasInclude{ false };
		bool        bHasMacros{ false };
	};

	struct UniformBlockInfo
	{
		GLuint Buffer{};
		GLuint Index{};
		GLint  Size{};
		std::unordered_map<std::string, GLint> Offsets{};
		UniformBlockLayout Layout{ UniformBlockLayout::kShared };
	};

public:
	Shader();
	Shader(const std::vector<std::string>& SourceFiles, const std::string& ProgramName = "", const std::vector<std::string>& Macros = { "NULL" });
	Shader(const Shader&) = delete;
	Shader(Shader&& Other) noexcept;
	~Shader();

	Shader& operator=(const Shader&) = delete;
	Shader& operator=(Shader&& Other) noexcept;

	void CreateUniformBlock(const std::string& BlockName, GLuint BindingPoint, const std::vector<std::string>& MemberNames,
							UniformBlockLayout Layout = UniformBlockLayout::kShared);

	template <typename T>
	void UpdateUniformBlockMember(const std::string& BlockName, const std::string& MemberName, const T& Value) const;

	template <typename T>
	UniformBlockUpdater<T> GetUniformBlockUpdater(const std::string& BlockName, const std::string& MemberName) const;

	GLuint GetUniformBuffer(const std::string& BlockName) const;
	bool HasUniformBlock(const std::string& BlockName) const;

	void UseProgram() const;
	void SetUniform1i(const std::string& Name, GLboolean Value) const;
	void SetUniform1i(const std::string& Name, GLint     Value) const;
	void SetUniform1f(const std::string& Name, GLfloat   Value) const;
	void SetUniform2f(const std::string& Name, GLfloat VectorX, GLfloat VectorY) const;
	void SetUniform3f(const std::string& Name, GLfloat VectorX, GLfloat VectorY, GLfloat VectorZ) const;
	void SetUniform4f(const std::string& Name, GLfloat VectorX, GLfloat VectorY, GLfloat VectorZ, GLfloat VectorW) const;
	void SetUniform1fv(const std::string& Name, const std::vector<GLfloat>& Array) const;
	void SetUniform2fv(const std::string& Name, const glm::vec2& Vector) const;
	void SetUniform3fv(const std::string& Name, const glm::vec3& Vector) const;
	void SetUniform4fv(const std::string& Name, const glm::vec4& Vector) const;
	void SetUniformMatrix2fv(const std::string& Name, const glm::mat2x2& Matrix) const;
	void SetUniformMatrix3fv(const std::string& Name, const glm::mat3x3& Matrix) const;
	void SetUniformMatrix4fv(const std::string& Name, const glm::mat4x4& Matrix) const;

	GLuint GetProgram() const;
	GLint  GetUniformLocation(const std::string& Name) const;

private:
	void InitShader(const std::vector<std::string>& SourceFiles, const std::string& ProgramName, const std::vector<std::string>& Macros);
	std::string GetIncludeDirectory(const std::string& Filepath) const;
	std::string GetIncludeFilename(const std::string& Statement) const;
	Source LoadShaderSource(const std::string& Filepath);
	void InsertMacros(const std::vector<std::string>& Macros, GLenum ShaderType, Source& ShaderSource) const;
	GLuint CompileShader(const Source& ShaderSource, GLenum ShaderType) const;
	void LinkProgram(const std::vector<GLuint>& Shaders);
	void SaveProgramBinary(const std::string& Filename) const;
	void LoadProgramBinary(const std::string& Filename);
	void CheckLinkError() const;
	GLuint GetUniformBlockIndex(const std::string& BlockName) const;
	GLint GetUniformBlockSize(const std::string& BlockName, GLuint BlockIndex) const;
	std::vector<GLint> GetUniformBlockOffsets(const std::string& BlockName, const std::vector<std::string>& Names) const;

private:
	std::unordered_map<std::string, UniformBlockInfo> _UniformBlocks;
	std::vector<GLenum>                               _ShaderTypes;
	std::set<std::string>                             _IncludedFiles;
	GLuint                                            _Program;
};

class UniformBlockManager
{
public:
	UniformBlockManager(Shader* Shader, const std::string& BlockName, GLuint BindingPoint,
						const std::vector<std::string>& MemberNames,
						Shader::UniformBlockLayout Layout = Shader::UniformBlockLayout::kShared);

	UniformBlockManager(Shader* Shader, const std::string& BlockName);

	template<typename T>
	Shader::UniformBlockUpdater<T> Get(const std::string& MemberName);

private:
	Shader*     _Shader;
	std::string _BlockName;
};


_ASSET_END
_NPGS_END

#include "Shader.inl"
