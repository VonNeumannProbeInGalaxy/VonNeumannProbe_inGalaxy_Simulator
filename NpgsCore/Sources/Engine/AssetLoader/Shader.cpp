#include "Shader.h"

#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <print>

#include "Engine/AssetLoader/GetAssetFilepath.h"

_NPGS_BEGIN
_ASSET_BEGIN

Shader::Shader() : _ShaderTypes{ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER }, _Program(0) {}

Shader::Shader(const std::vector<std::string>& SourceFiles, const std::string& ProgramName, const std::vector<std::string>& Macros)
	: _ShaderTypes{ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER }, _Program(0)
{
	std::string ProgramCache = GetAssetFilepath(Asset::AssetType::kBinaryShader, ProgramName + ".bin");
	if (ProgramName != "")
	{
		std::filesystem::path ProgramCachePath(ProgramCache);

		if (std::filesystem::exists(ProgramCachePath))
		{
			LoadProgramBinary(ProgramCache);
			return;
		}
	}

	std::vector<Source> ShaderSources;
	for (const auto& kSourceFile : SourceFiles)
	{
		ShaderSources.emplace_back(LoadShaderSource(GetAssetFilepath(Asset::AssetType::kShader, kSourceFile)));
	}

	if (Macros.front() != "NULL")
	{
		for (GLuint i = 0; i != SourceFiles.size(); ++i)
		{
			InsertMacros(Macros, _ShaderTypes[i], ShaderSources[i]);
		}
	}

	std::vector<GLuint> Shaders;
	for (GLuint i = 0; i != ShaderSources.size(); ++i)
	{
		Shaders.emplace_back(CompileShader(ShaderSources[i], _ShaderTypes[i]));
	}

	LinkProgram(Shaders);
	SaveProgramBinary(ProgramCache);

	for (GLuint Shader : Shaders)
	{
		glDetachShader(_Program, Shader);
		glDeleteShader(Shader);
	}
}

Shader::~Shader()
{
	glDeleteProgram(_Program);
}

std::string Shader::GetIncludeDirectory(const std::string& Filepath) const
{
	return (std::filesystem::path(Filepath).parent_path().string() + '/');
}

std::string Shader::GetIncludeFilename(const std::string& Statement) const
{
	std::string::size_type FirstQuotePosition = Statement.find_first_of("\"");
	std::string::size_type LastQuotePosition  = Statement.find_last_of("\"");

	return Statement.substr(FirstQuotePosition + 1, LastQuotePosition - FirstQuotePosition - 1);
}

Shader::Source Shader::LoadShaderSource(const std::string& Filepath)
{
	std::ifstream SourceFile(Filepath);
	std::string   SourceCode;
	std::string   Statement;

	GLboolean bHasInclude = GL_FALSE;

	if (!SourceFile.is_open())
	{
		std::println("Fatal error: Can not open shader source file: \"{}\": No such file or directory.", Filepath);
		std::system("pause");
		std::exit(EXIT_FAILURE);
	}

	GLint LineNumber = 1;
	while (std::getline(SourceFile, Statement))
	{
		if (Statement.find("#include") != std::string::npos)
		{
			bHasInclude = GL_TRUE;
			std::string IncludedFile = GetIncludeDirectory(Filepath) + GetIncludeFilename(Statement);
			if (_IncludedFiles.find(IncludedFile) == _IncludedFiles.end())
			{
				_IncludedFiles.insert(IncludedFile);
				Statement = LoadShaderSource(IncludedFile).Data;
				SourceCode += "#line 1 \"" + IncludedFile + "\"\n" + Statement + "\n#line " + std::to_string(LineNumber + 1) + " \"" + Filepath + "\"\n";
			}
			else
			{
				continue;
			}

			++LineNumber;
		}

		SourceCode += Statement + '\n';
	}

	SourceFile.close();

	return { SourceCode, Filepath, bHasInclude, GL_FALSE };
}

GLvoid Shader::InsertMacros(const std::vector<std::string>& Macros, GLenum ShaderType, Source& ShaderSource) const
{
	auto TypePrefix = [ShaderType]() -> std::string
	{
		switch (ShaderType)
		{
		case GL_VERTEX_SHADER:
			return "__VERT";
		case GL_FRAGMENT_SHADER:
			return "__FRAG";
		case GL_GEOMETRY_SHADER:
			return "__GEOM";
		default:
			assert(GL_FALSE);
			return "";
		}
	}();

	GLuint                 InsertedMacroCount = 0;
	std::string::size_type InsertedCharLength = 0;

	for (const std::string& kMacro : Macros)
	{
		if (kMacro.find(TypePrefix) != std::string::npos)
		{
			ShaderSource.bHasMacros = GL_TRUE;
			ShaderSource.Data.insert(19ULL + InsertedCharLength, "#define " + kMacro.substr(7) + '\n');
			InsertedCharLength += kMacro.size() + 2;
			++InsertedMacroCount;
		}
	}

	if (InsertedMacroCount)
	{
		ShaderSource.Data.insert(18ULL + InsertedCharLength, "\n");
		ShaderSource.Data.insert(19ULL + InsertedCharLength, "#line 1\n");
	}
}

GLuint Shader::CompileShader(const Source& ShaderSource, GLenum ShaderType) const
{
	GLuint Shader = glCreateShader(ShaderType);
	const GLchar* SourceCodePointer = ShaderSource.Data.c_str();

	glShaderSource(Shader, 1, &SourceCodePointer, nullptr);
	glCompileShader(Shader);

	GLint CompileStatus = 0;

	glGetShaderiv(Shader, GL_COMPILE_STATUS, &CompileStatus);
	if (!CompileStatus)
	{
		GLint InfoLogLength = 0;
		glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &InfoLogLength);
		std::string InfoLog(InfoLogLength, '\0');
		glGetShaderInfoLog(Shader, InfoLogLength, nullptr, InfoLog.data());
		std::println("Shader \"{}\" compile failed:\n{}", ShaderSource.Filepath, InfoLog.data());
		if (ShaderSource.bHasInclude || ShaderSource.bHasMacros)
		{
			std::ofstream ErrorShader(ShaderSource.Filepath + ".Error.glsl");
			ErrorShader << ShaderSource.Data << std::endl;
			ErrorShader.close();
		}

		std::system("pause");
		std::exit(EXIT_FAILURE);
	}

	return Shader;
}

GLvoid Shader::LinkProgram(const std::vector<GLuint>& Shaders)
{
	_Program = glCreateProgram();

	for (GLuint Shader : Shaders)
	{
		glAttachShader(_Program, Shader);
	}

	glLinkProgram(_Program);
	CheckLinkError();
}

GLvoid Shader::SaveProgramBinary(const std::string& Filename) const
{
	std::filesystem::path Filepath(Filename);
	std::filesystem::path Directory(Filepath.parent_path());
	if (!std::filesystem::exists(Directory))
	{
		std::filesystem::create_directories(Directory);
	}

	GLint BinaryLength = 0;
	glGetProgramiv(_Program, GL_PROGRAM_BINARY_LENGTH, &BinaryLength);

	std::vector<GLubyte> Binary(BinaryLength);
	GLenum BinaryFormat = 0;
	glGetProgramBinary(_Program, BinaryLength, nullptr, &BinaryFormat, Binary.data());

	std::ofstream BinaryFile(Filename, std::ios::binary);
	if (!BinaryFile.is_open())
	{
		std::println("Error: Failed to write program binary to file \"{}\".", Filename);
		return;
	}

	BinaryFile.write(reinterpret_cast<const char*>(&BinaryFormat), sizeof(GLenum));
	BinaryFile.write(reinterpret_cast<const char*>(Binary.data()), Binary.size());
	BinaryFile.close();
}

GLvoid Shader::LoadProgramBinary(const std::string& Filename)
{
	std::ifstream BinaryFile(Filename, std::ios::binary);
	if (!BinaryFile.is_open())
	{
		std::println("Error: Failed to open binary program file \"{}\".", Filename);
		return;
	}

	GLenum BinaryFormat = 0;
	BinaryFile.read(reinterpret_cast<char*>(&BinaryFormat), sizeof(GLenum));

	std::vector<GLubyte> Binary(std::istreambuf_iterator<char>(BinaryFile), {});

	_Program = glCreateProgram();
	glProgramBinary(_Program, BinaryFormat, Binary.data(), static_cast<GLsizei>(Binary.size()));

	CheckLinkError();
}

GLvoid Shader::CheckLinkError() const
{
	GLint LinkStatus = 0;

	glGetProgramiv(_Program, GL_LINK_STATUS, &LinkStatus);
	if (!LinkStatus)
	{
		GLint InfoLogLength = 0;
		glGetProgramiv(_Program, GL_INFO_LOG_LENGTH, &InfoLogLength);
		std::string InfoLog(InfoLogLength, '\0');
		glGetProgramInfoLog(_Program, InfoLogLength, nullptr, InfoLog.data());
		std::println("Program binary load failed:\n{}", InfoLog.data());

		std::system("pause");
		std::exit(EXIT_FAILURE);
	}
}

_ASSET_END
_NPGS_END