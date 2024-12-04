#pragma once

#include <set>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Engine/Core/Base.h"

_NPGS_BEGIN
_ASSET_BEGIN

// @brief 着色器类，用于从 GLSL 代码文件或二进制着色器程序文件加载并创建着色器程序。
//        支持 #include 指令，支持在加载的同时插入宏。
class Shader
{
private:
	// @brief Source 结构体
	struct Source
	{
		std::string Data;        // 着色器字符串数据
		std::string Filepath;    // 着色器源代码文件路径
		bool        bHasInclude; // 是否有 #include 指令
		bool        bHasMacros;  // 是否包含插入宏
	};

public:
	Shader();

	// @brief 着色器类主构造函数，用于通过给定的参数加载着色器程序。
	// @param SourceFiles 着色器源代码文件路径列表。使用 {} 初始化列表传递 2-3 个参数，几何着色器是可选的。
	//        内部已经调用了 GetAssetFilepath 函数，直接从 Shaders/ 之后传递文件名即可。
	//        例：{ "VertShaders/Shader.glsl", "FragShaders/Shader.glsl" }
	//        或  { "VertShaders/Shader.glsl", "FragShaders/Shader.glsl", "GeomShaders/Shader.glsl" }。
	// @param ProgramName 着色器二进制程序文件名。留空则不保存二进制程序，也不会从二进制程序加载。
	// @param Macros 插入宏列表。可以使用 {} 初始化列表传递多个。通过不同前缀识别不同着色器的宏，
	//        __VERT 表示顶点着色器，__FRAG 表示片元着色器，__GEOM 表示几何着色器（注意是两个下划线）。目前支持这三种着色器。
	//        例：{ __FRAG_MY_MACRO 1 } 将会插入 #define MY_MACRO 1 语句到片元着色器的源代码中。
	Shader(const std::vector<std::string>& SourceFiles, const std::string& ProgramName = "", const std::vector<std::string>& Macros = { "NULL" });
	~Shader();

	GLuint GetProgram() const;
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
	std::string GetIncludeDirectory(const std::string& Filepath) const;
	std::string GetIncludeFilename(const std::string& Statement) const;
	Source LoadShaderSource(const std::string& Filepath);
	void InsertMacros(const std::vector<std::string>& Macros, GLenum ShaderType, Source& ShaderSource) const;
	GLuint CompileShader(const Source& ShaderSource, GLenum ShaderType) const;
	void LinkProgram(const std::vector<GLuint>& Shaders);
	void SaveProgramBinary(const std::string& Filename) const;
	void LoadProgramBinary(const std::string& Filename);
	void CheckLinkError() const;

private:
	std::set<std::string> _IncludedFiles;
	std::vector<GLenum>   _ShaderTypes;
	GLuint                _Program;
};

_ASSET_END
_NPGS_END

#include "Shader.inl"
