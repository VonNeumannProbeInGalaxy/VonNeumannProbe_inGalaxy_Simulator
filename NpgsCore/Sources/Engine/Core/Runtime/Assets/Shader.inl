#pragma once

#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>

_NPGS_BEGIN
_ASSET_BEGIN

NPGS_INLINE GLuint FShader::GetProgram() const
{
	return _Program;
}

NPGS_INLINE GLint FShader::GetUniformLocation(const std::string& Name) const
{
	return _UniformLocationsCache.at(Name);
}

NPGS_INLINE void FShader::UseProgram() const
{
	glUseProgram(_Program);
}

NPGS_INLINE void FShader::SetUniform1i(const std::string& Name, GLboolean Value) const
{
	glUniform1i(_UniformLocationsCache.at(Name), static_cast<GLint>(Value));
}

NPGS_INLINE void FShader::SetUniform1i(const std::string& Name, GLint Value) const
{
	glUniform1i(_UniformLocationsCache.at(Name), Value);
}

NPGS_INLINE void FShader::SetUniform1f(const std::string& Name, GLfloat Value) const
{
	glUniform1f(_UniformLocationsCache.at(Name), Value);
}

NPGS_INLINE void FShader::SetUniform2f(const std::string& Name, GLfloat VectorX, GLfloat VectorY) const
{
	glUniform2f(_UniformLocationsCache.at(Name), VectorX, VectorY);
}

NPGS_INLINE void FShader::SetUniform3f(const std::string& Name, GLfloat VectorX, GLfloat VectorY, GLfloat VectorZ) const
{
	glUniform3f(_UniformLocationsCache.at(Name), VectorX, VectorY, VectorZ);
}

NPGS_INLINE void FShader::SetUniform4f(const std::string& Name, GLfloat VectorX, GLfloat VectorY, GLfloat VectorZ, GLfloat VectorW) const
{
	glUniform4f(_UniformLocationsCache.at(Name), VectorX, VectorY, VectorZ, VectorW);
}

NPGS_INLINE void FShader::SetUniform1fv(const std::string& Name, const std::vector<GLfloat>& Array) const
{
	glUniform1fv(_UniformLocationsCache.at(Name), static_cast<GLsizei>(Array.size()), Array.data());
}

NPGS_INLINE void FShader::SetUniform2fv(const std::string& Name, const glm::vec2& Vector) const
{
	glUniform2fv(_UniformLocationsCache.at(Name), 1, glm::value_ptr(Vector));
}

NPGS_INLINE void FShader::SetUniform3fv(const std::string& Name, const glm::vec3& Vector) const
{
	glUniform3fv(_UniformLocationsCache.at(Name), 1, glm::value_ptr(Vector));
}

NPGS_INLINE void FShader::SetUniform4fv(const std::string& Name, const glm::vec4& Vector) const
{
	glUniform4fv(_UniformLocationsCache.at(Name), 1, glm::value_ptr(Vector));
}

NPGS_INLINE void FShader::SetUniformMatrix2fv(const std::string& Name, const glm::mat2x2& Matrix) const
{
	glUniformMatrix2fv(_UniformLocationsCache.at(Name), 1, GL_FALSE, glm::value_ptr(Matrix));
}

NPGS_INLINE void FShader::SetUniformMatrix3fv(const std::string& Name, const glm::mat3x3& Matrix) const
{
	glUniformMatrix3fv(_UniformLocationsCache.at(Name), 1, GL_FALSE, glm::value_ptr(Matrix));
}

NPGS_INLINE void FShader::SetUniformMatrix4fv(const std::string& Name, const glm::mat4x4& Matrix) const
{
	glUniformMatrix4fv(_UniformLocationsCache.at(Name), 1, GL_FALSE, glm::value_ptr(Matrix));
}

_ASSET_END
_NPGS_END
