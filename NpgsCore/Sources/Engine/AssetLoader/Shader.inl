#pragma once

#pragma once

#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>

_NPGS_BEGIN
_ASSET_BEGIN

template<typename T>
NPGS_INLINE Shader::UniformBlockUpdater<T> Shader::GetUniformBlockUpdater(const std::string& BlockName, const std::string& MemberName) const
{
	auto& BlockInfo = _UniformBlocks.at(BlockName);
	return UniformBlockUpdater<T>(BlockInfo.Buffer, BlockInfo.Offsets.at(MemberName));
}

NPGS_INLINE GLuint Shader::GetUniformBuffer(const std::string& BlockName) const
{
	auto it = _UniformBlocks.find(BlockName);
	return it != _UniformBlocks.end() ? it->second.Buffer : 0;
}

NPGS_INLINE bool Shader::HasUniformBlock(const std::string& BlockName) const
{
	return _UniformBlocks.find(BlockName) != _UniformBlocks.end();
}

NPGS_INLINE void Shader::UseProgram() const
{
	glUseProgram(_Program);
}

NPGS_INLINE void Shader::SetUniform1i(const std::string& Name, GLboolean Value) const
{
	glUniform1i(glGetUniformLocation(_Program, Name.c_str()), static_cast<GLint>(Value));
}

NPGS_INLINE void Shader::SetUniform1i(const std::string& Name, GLint Value) const
{
	glUniform1i(glGetUniformLocation(_Program, Name.c_str()), Value);
}

NPGS_INLINE void Shader::SetUniform1f(const std::string& Name, GLfloat Value) const
{
	glUniform1f(glGetUniformLocation(_Program, Name.c_str()), Value);
}

NPGS_INLINE void Shader::SetUniform2f(const std::string& Name, GLfloat VectorX, GLfloat VectorY) const
{
	glUniform2f(glGetUniformLocation(_Program, Name.c_str()), VectorX, VectorY);
}

NPGS_INLINE void Shader::SetUniform3f(const std::string& Name, GLfloat VectorX, GLfloat VectorY, GLfloat VectorZ) const
{
	glUniform3f(glGetUniformLocation(_Program, Name.c_str()), VectorX, VectorY, VectorZ);
}

NPGS_INLINE void Shader::SetUniform4f(const std::string& Name, GLfloat VectorX, GLfloat VectorY, GLfloat VectorZ, GLfloat VectorW) const
{
	glUniform4f(glGetUniformLocation(_Program, Name.c_str()), VectorX, VectorY, VectorZ, VectorW);
}

NPGS_INLINE void Shader::SetUniform1fv(const std::string& Name, const std::vector<GLfloat>& Array) const
{
	glUniform1fv(glGetUniformLocation(_Program, Name.c_str()), static_cast<GLsizei>(Array.size()), Array.data());
}

NPGS_INLINE void Shader::SetUniform2fv(const std::string& Name, const glm::vec2& Vector) const
{
	glUniform2fv(glGetUniformLocation(_Program, Name.c_str()), 1, glm::value_ptr(Vector));
}

NPGS_INLINE void Shader::SetUniform3fv(const std::string& Name, const glm::vec3& Vector) const
{
	glUniform3fv(glGetUniformLocation(_Program, Name.c_str()), 1, glm::value_ptr(Vector));
}

NPGS_INLINE void Shader::SetUniform4fv(const std::string& Name, const glm::vec4& Vector) const
{
	glUniform4fv(glGetUniformLocation(_Program, Name.c_str()), 1, glm::value_ptr(Vector));
}

NPGS_INLINE void Shader::SetUniformMatrix2fv(const std::string& Name, const glm::mat2x2& Matrix) const
{
	glUniformMatrix2fv(glGetUniformLocation(_Program, Name.c_str()), 1, GL_FALSE, glm::value_ptr(Matrix));
}

NPGS_INLINE void Shader::SetUniformMatrix3fv(const std::string& Name, const glm::mat3x3& Matrix) const
{
	glUniformMatrix3fv(glGetUniformLocation(_Program, Name.c_str()), 1, GL_FALSE, glm::value_ptr(Matrix));
}

NPGS_INLINE void Shader::SetUniformMatrix4fv(const std::string& Name, const glm::mat4x4& Matrix) const
{
	glUniformMatrix4fv(glGetUniformLocation(_Program, Name.c_str()), 1, GL_FALSE, glm::value_ptr(Matrix));
}

NPGS_INLINE GLuint Shader::GetProgram() const
{
	return _Program;
}

NPGS_INLINE GLint Shader::GetUniformLocation(const std::string& Name) const
{
	return glGetUniformLocation(_Program, Name.c_str());
}

template<typename T>
NPGS_INLINE Shader::UniformBlockUpdater<T> UniformBlockManager::Get(const std::string& MemberName)
{
	return _Shader->GetUniformBlockUpdater<T>(_BlockName, MemberName);
}

_ASSET_END
_NPGS_END
