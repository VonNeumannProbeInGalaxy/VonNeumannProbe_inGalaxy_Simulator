#pragma once

#include <set>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Engine/Core/Base.h"

_NPGS_BEGIN
_ASSET_BEGIN

class Shader
{
private:
	struct Source
	{
		std::string Data;
		std::string Filename;
		bool bHasIncludes{ false };
		bool bHasMacros{ false };
	};

public:
	Shader(const std::vector<std::string>& SourceFiles, const std::string& ProgramName = "", const std::vector<std::string>& Macros = { "NULL" });
	Shader(const Shader&) = delete;
	Shader(Shader&& Other) noexcept;
	~Shader();

	Shader& operator=(const Shader&) = delete;
	Shader& operator=(Shader&& Other) noexcept;

	GLuint GetProgram() const;
	GLint GetUniformLocation(const std::string& Name) const;

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

private:
	void InitShader(const std::vector<std::string>& SourceFiles, const std::string& ProgramName, const std::vector<std::string>& Macros);
	Source LoadShaderSource(const std::string& Filepath);
	void PushShaderType(const std::string& Extension);
	void InsertMacros(const std::vector<std::string>& Macros, GLenum ShaderType, Source& ShaderSource) const;
	GLuint CompileShader(const Source& ShaderSource, GLenum ShaderType) const;
	void LinkProgram(const std::vector<GLuint>& Shaders);
	void SaveProgramBinary(const std::string& Filename) const;
	void LoadProgramBinary(const std::string& Filename);
	void CheckLinkError() const;

private:
	std::vector<GLenum>   _ShaderTypes;
	std::set<std::string> _IncludedFiles;
	GLuint                _Program;
};

_ASSET_END
_NPGS_END

#include "Shader.inl"
