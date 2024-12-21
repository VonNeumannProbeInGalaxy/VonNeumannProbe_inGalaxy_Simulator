#include "Shader.h"

#include <cstdint>
#include <cstdlib>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <print>
#include <utility>

#include "Engine/AssetLoader/GetAssetFullPath.h"
#include "Engine/Core/Assert.h"

_NPGS_BEGIN
_ASSET_BEGIN

namespace
{
	std::string GetIncludeDirectory(const std::string& FilePath);
	std::string GetIncludeFilename(const std::string& Statement);
}

FShader::FShader(const std::vector<std::string>& SourceFiles, const std::string& ProgramBinaryName,
				 const std::vector<std::string>& Macros)
	:
	_Program(0)
{
	InitShader(SourceFiles, ProgramBinaryName, Macros);
}

FShader::FShader(FShader&& Other) noexcept
	:
	_IncludedFiles(std::move(Other._IncludedFiles)),
	_ShaderTypes(std::move(Other._ShaderTypes)),
	_Program(Other._Program)
{
	Other._Program = 0;
}

FShader::~FShader()
{
	glDeleteProgram(_Program);
}

FShader& FShader::operator=(FShader&& Other) noexcept
{
	if (this != &Other)
	{
		if (_Program)
		{
			glDeleteProgram(_Program);
		}

		_IncludedFiles = std::move(Other._IncludedFiles);
		_ShaderTypes   = std::move(Other._ShaderTypes);
		_Program       = Other._Program;
		Other._Program = 0;
	}

	return *this;
}

void FShader::InitShader(const std::vector<std::string>& SourceFiles, const std::string& ProgramBinaryName,
						 const std::vector<std::string>& Macros)
{
	std::string ProgramCache = GetAssetFullPath(Asset::EAssetType::kBinaryShader, ProgramBinaryName + ".bin");
	if (ProgramBinaryName != "")
	{
		std::filesystem::path ProgramCachePath(ProgramCache);

		if (std::filesystem::exists(ProgramCachePath))
		{
			LoadProgramBinary(ProgramCache);
			return;
		}
	}

	std::vector<FSource> ShaderSources;
	for (const auto& SourceFile : SourceFiles)
	{
		ShaderSources.emplace_back(LoadShaderSource(GetAssetFullPath(Asset::EAssetType::kShader, SourceFile)));
	}

	for (const auto& SourceFile : SourceFiles)
	{
		PushShaderType(std::filesystem::path(SourceFile).extension().string());
	}

	if (Macros.front() != "NULL")
	{
		for (std::size_t i = 0; i != SourceFiles.size(); ++i)
		{
			InsertMacros(Macros, _ShaderTypes[i], ShaderSources[i]);
		}
	}

	std::vector<GLuint> Shaders;
	for (std::size_t i = 0; i != ShaderSources.size(); ++i)
	{
		Shaders.emplace_back(CompileShader(ShaderSources[i], _ShaderTypes[i]));
	}

	LinkProgram(Shaders);

	if (ProgramBinaryName != "")
	{
		SaveProgramBinary(ProgramCache);
	}

	for (GLuint Shader : Shaders)
	{
		glDetachShader(_Program, Shader);
		glDeleteShader(Shader);
	}
}

FShader::FSource FShader::LoadShaderSource(const std::string& Filename)
{
	std::ifstream SourceFile(Filename);
	std::string   SourceCode;
	std::string   Statement;

	bool bHasInclude = false;

	if (!SourceFile.is_open())
	{
		std::println("Fatal error: Can not open shader source file: \"{}\": No such file or directory.", Filename);
		std::system("pause");
		std::exit(EXIT_FAILURE);
	}

	int LineNumber = 1;
	while (std::getline(SourceFile, Statement))
	{
		if (Statement.find("#include") != std::string::npos)
		{
			bHasInclude = true;
			std::string IncludedFile = GetIncludeDirectory(Filename) + GetIncludeFilename(Statement);
			if (_IncludedFiles.find(IncludedFile) == _IncludedFiles.end())
			{
				_IncludedFiles.insert(IncludedFile);
				Statement = LoadShaderSource(IncludedFile).Data;
				SourceCode += "#line 1 \"" + IncludedFile + "\"\n" + Statement + "\n#line " +
					std::to_string(LineNumber + 1) + " \"" + Filename + "\"\n";
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

	return { SourceCode, Filename, bHasInclude, false };
}

void FShader::PushShaderType(const std::string& Extension)
{
	if (Extension == ".comp")
	{
		_ShaderTypes.emplace_back(GL_COMPUTE_SHADER);
	}
	else if (Extension == ".frag")
	{
		_ShaderTypes.emplace_back(GL_FRAGMENT_SHADER);
	}
	else if (Extension == ".geom")
	{
		_ShaderTypes.emplace_back(GL_GEOMETRY_SHADER);
	}
	else if (Extension == ".tesc")
	{
		_ShaderTypes.emplace_back(GL_TESS_CONTROL_SHADER);
	}
	else if (Extension == ".tese")
	{
		_ShaderTypes.emplace_back(GL_TESS_EVALUATION_SHADER);
	}
	else if (Extension == ".vert")
	{
		_ShaderTypes.emplace_back(GL_VERTEX_SHADER);
	}
	else
	{
		NpgsAssert(false, "Invalid shader file extension");
	}
}

void FShader::InsertMacros(const std::vector<std::string>& Macros, GLenum ShaderType, FSource& ShaderSource) const
{
	auto TypePrefix = [ShaderType]() -> std::string
	{
		switch (ShaderType)
		{
		case GL_COMPUTE_SHADER:
			return "__COMP";
		case GL_FRAGMENT_SHADER:
			return "__FRAG";
		case GL_GEOMETRY_SHADER:
			return "__GEOM";
		case GL_TESS_CONTROL_SHADER:
			return "__TESC";
		case GL_TESS_EVALUATION_SHADER:
			return "__TESE";
		case GL_VERTEX_SHADER:
			return "__VERT";
		default:
			NpgsAssert(false, "Invalid shader type");
			return "";
		}
	}();

	std::uint32_t          InsertedMacroCount = 0;
	std::string::size_type InsertedCharLength = 0;

	for (const std::string& Macro : Macros)
	{
		if (Macro.find(TypePrefix) != std::string::npos)
		{
			ShaderSource.bHasMacros = true;
			ShaderSource.Data.insert(19ULL + InsertedCharLength, "#define " + Macro.substr(TypePrefix.size() + 1) + '\n');
			InsertedCharLength += Macro.size() + 2;
			++InsertedMacroCount;
		}
	}

	if (InsertedMacroCount)
	{
		ShaderSource.Data.insert(18ULL + InsertedCharLength, "\n");
		ShaderSource.Data.insert(19ULL + InsertedCharLength, "#line 1\n");
	}
}

GLuint FShader::CompileShader(const FSource& ShaderSource, GLenum ShaderType) const
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
		std::println("Shader \"{}\" compile failed:\n{}", ShaderSource.Filename, InfoLog.data());
		if (ShaderSource.bHasIncludes || ShaderSource.bHasMacros)
		{
			std::ofstream ErrorShader(ShaderSource.Filename + ".Error.glsl");
			ErrorShader << ShaderSource.Data << std::endl;
			ErrorShader.close();
		}

		std::system("pause");
		std::exit(EXIT_FAILURE);
	}

	return Shader;
}

void FShader::LinkProgram(const std::vector<GLuint>& Shaders)
{
	_Program = glCreateProgram();

	for (GLuint Shader : Shaders)
	{
		glAttachShader(_Program, Shader);
	}

	glLinkProgram(_Program);
	CheckLinkError();
}

void FShader::SaveProgramBinary(const std::string& Filename) const
{
	std::filesystem::path FilePath(Filename);
	std::filesystem::path Directory(FilePath.parent_path());
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

void FShader::LoadProgramBinary(const std::string& Filename)
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

void FShader::CheckLinkError() const
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

namespace
{
	std::string GetIncludeDirectory(const std::string& FilePath)
	{
		return (std::filesystem::path(FilePath).parent_path().string() + '/');
	}

	std::string GetIncludeFilename(const std::string& Statement)
	{
		std::size_t FirstQuotePosition = Statement.find_first_of("\"");
		std::size_t LastQuotePosition  = Statement.find_last_of("\"");

		return Statement.substr(FirstQuotePosition + 1, LastQuotePosition - FirstQuotePosition - 1);
	}
}

_ASSET_END
_NPGS_END
