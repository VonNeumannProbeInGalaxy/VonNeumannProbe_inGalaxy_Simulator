#pragma once

#pragma once

#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>

_NPGS_BEGIN
_ASSET_BEGIN

inline GLuint Shader::GetProgram() const
{
	return _Program;
}

inline void Shader::UseProgram() const
{
	glUseProgram(_Program);
}

inline void Shader::SetUniform1i(const std::string& Name, GLboolean Value) const
{
	glUniform1i(glGetUniformLocation(_Program, Name.c_str()), static_cast<GLint>(Value));
}

inline void Shader::SetUniform1i(const std::string& Name, GLint Value) const
{
	glUniform1i(glGetUniformLocation(_Program, Name.c_str()), Value);
}

inline void Shader::SetUniform1f(const std::string& Name, GLfloat Value) const
{
	glUniform1f(glGetUniformLocation(_Program, Name.c_str()), Value);
}

inline void Shader::SetUniform2f(const std::string& Name, GLfloat VectorX, GLfloat VectorY) const
{
	glUniform2f(glGetUniformLocation(_Program, Name.c_str()), VectorX, VectorY);
}

inline void Shader::SetUniform3f(const std::string& Name, GLfloat VectorX, GLfloat VectorY, GLfloat VectorZ) const
{
	glUniform3f(glGetUniformLocation(_Program, Name.c_str()), VectorX, VectorY, VectorZ);
}

inline void Shader::SetUniform4f(const std::string& Name, GLfloat VectorX, GLfloat VectorY, GLfloat VectorZ, GLfloat VectorW) const
{
	glUniform4f(glGetUniformLocation(_Program, Name.c_str()), VectorX, VectorY, VectorZ, VectorW);
}

inline void Shader::SetUniform1fv(const std::string& Name, const std::vector<GLfloat>& Array) const
{
	glUniform1fv(glGetUniformLocation(_Program, Name.c_str()), static_cast<GLsizei>(Array.size()), Array.data());
}

inline void Shader::SetUniform2fv(const std::string& Name, const glm::vec2& Vector) const
{
	glUniform2fv(glGetUniformLocation(_Program, Name.c_str()), 1, glm::value_ptr(Vector));
}

inline void Shader::SetUniform3fv(const std::string& Name, const glm::vec3& Vector) const
{
	glUniform3fv(glGetUniformLocation(_Program, Name.c_str()), 1, glm::value_ptr(Vector));
}

inline void Shader::SetUniform4fv(const std::string& Name, const glm::vec4& Vector) const
{
	glUniform4fv(glGetUniformLocation(_Program, Name.c_str()), 1, glm::value_ptr(Vector));
}

inline void Shader::SetUniformMatrix2fv(const std::string& Name, const glm::mat2x2& Matrix) const
{
	glUniformMatrix2fv(glGetUniformLocation(_Program, Name.c_str()), 1, GL_FALSE, glm::value_ptr(Matrix));
}

inline void Shader::SetUniformMatrix3fv(const std::string& Name, const glm::mat3x3& Matrix) const
{
	glUniformMatrix3fv(glGetUniformLocation(_Program, Name.c_str()), 1, GL_FALSE, glm::value_ptr(Matrix));
}

inline void Shader::SetUniformMatrix4fv(const std::string& Name, const glm::mat4x4& Matrix) const
{
	glUniformMatrix4fv(glGetUniformLocation(_Program, Name.c_str()), 1, GL_FALSE, glm::value_ptr(Matrix));
}

_ASSET_END
_NPGS_END
