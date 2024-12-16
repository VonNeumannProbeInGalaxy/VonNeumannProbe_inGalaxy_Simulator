#include "Shader.h"

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <print>

#include "Engine/AssetLoader/GetAssetFilepath.h"
#include "Engine/Core/Assert.h"

_NPGS_BEGIN
_ASSET_BEGIN

Shader::Shader() : _ShaderTypes{ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER }, _Program(0) {}

Shader::Shader(const std::vector<std::string>& SourceFiles, const std::string& ProgramName, const std::vector<std::string>& Macros)
	: _ShaderTypes{ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER }, _Program(0)
{
	InitShader(SourceFiles, ProgramName, Macros);
}

Shader::Shader(Shader&& Other) noexcept
	:
	_IncludedFiles(std::move(Other._IncludedFiles)),
	_ShaderTypes(std::move(Other._ShaderTypes)),
	_Program(Other._Program)
{
	Other._Program = 0;
}

Shader::~Shader()
{
	glDeleteProgram(_Program);

	for (auto& [Name, Block] : _UniformBlocks)
	{
		glDeleteBuffers(1, &Block.Buffer);
	}
	_UniformBlocks.clear();
}

Shader& Shader::operator=(Shader&& Other) noexcept
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

void Shader::CreateUniformBlock(const std::string& BlockName, GLuint BindingPoint,
								const std::vector<std::string>& MemberNames, UniformBlockLayout Layout)
{
	UniformBlockInfo BlockInfo;
	BlockInfo.Layout = Layout;
	BlockInfo.Index = GetUniformBlockIndex(BlockName);
	if (BlockInfo.Index == GL_INVALID_INDEX)
	{
		return;
	}

	BlockInfo.Size = GetUniformBlockSize(BlockName, BlockInfo.Index);

	glCreateBuffers(1, &BlockInfo.Buffer);
	glNamedBufferData(BlockInfo.Buffer, BlockInfo.Size, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, BindingPoint, BlockInfo.Buffer);
	glUniformBlockBinding(_Program, BlockInfo.Index, BindingPoint);

	std::vector<const GLchar*> NamePtrs(MemberNames.size());
	for (const auto& Names : MemberNames)
	{
		NamePtrs.emplace_back(Names.c_str());
	}

	std::vector<GLint> Offsets = GetUniformBlockOffsets(BlockName, MemberNames);

	for (std::size_t i = 0; i != MemberNames.size(); ++i)
	{
		BlockInfo.Offsets[MemberNames[i]] = Offsets[i];
	}

	_UniformBlocks[BlockName] = BlockInfo;
}

template<typename T>
void Shader::UpdateUniformBlockMember(const std::string& BlockName, const std::string& MemberName, const T& Value) const
{
	auto BlockIt = _UniformBlocks.find(BlockName);
	if (BlockIt == _UniformBlocks.end())
	{
		return;
	}

	auto& BlockInfo = BlockIt->second;
	auto OffsetIt = BlockInfo.Offsets.find(MemberName);
	if (OffsetIt == BlockInfo.Offsets.end())
	{
		return;
	}

	glNamedBufferSubData(BlockInfo.Buffer, OffsetIt->second, sizeof(T), &Value);
}

void Shader::InitShader(const std::vector<std::string>& SourceFiles, const std::string& ProgramName, const std::vector<std::string>& Macros)
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

	if (ProgramName != "")
	{
		SaveProgramBinary(ProgramCache);
	}

	for (GLuint Shader : Shaders)
	{
		glDetachShader(_Program, Shader);
		glDeleteShader(Shader);
	}
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

	bool bHasInclude = false;

	if (!SourceFile.is_open())
	{
		std::println("Fatal error: Can not open shader source file: \"{}\": No such file or directory.", Filepath);
		std::system("pause");
		std::exit(EXIT_FAILURE);
	}

	int LineNumber = 1;
	while (std::getline(SourceFile, Statement))
	{
		if (Statement.find("#include") != std::string::npos)
		{
			bHasInclude = true;
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

	return { SourceCode, Filepath, bHasInclude, false };
}

void Shader::InsertMacros(const std::vector<std::string>& Macros, GLenum ShaderType, Source& ShaderSource) const
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
			NpgsAssert(false, "Invalid shader type");
			return "";
		}
	}();

	std::uint32_t          InsertedMacroCount = 0;
	std::string::size_type InsertedCharLength = 0;

	for (const std::string& kMacro : Macros)
	{
		if (kMacro.find(TypePrefix) != std::string::npos)
		{
			ShaderSource.bHasMacros = true;
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

void Shader::LinkProgram(const std::vector<GLuint>& Shaders)
{
	_Program = glCreateProgram();

	for (GLuint Shader : Shaders)
	{
		glAttachShader(_Program, Shader);
	}

	glLinkProgram(_Program);
	CheckLinkError();
}

void Shader::SaveProgramBinary(const std::string& Filename) const
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

void Shader::LoadProgramBinary(const std::string& Filename)
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

void Shader::CheckLinkError() const
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

std::vector<GLint> Shader::GetUniformBlockOffsets(const std::string& BlockName, const std::vector<std::string>& Names) const
{
	GLint MaxUniformIndices = 0;
	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &MaxUniformIndices);

	if (static_cast<GLint>(Names.size()) > MaxUniformIndices)
	{
		std::println("Warning: Number of uniforms ({}) exceeds maximum supported ({})",
					 Names.size(), MaxUniformIndices);
		return {};
	}

	std::vector<GLint> Offsets(Names.size());
	std::vector<const GLchar*> NamePtrs(Names.size());

	for (std::size_t i = 0; i != Names.size(); ++i)
	{
		NamePtrs[i] = Names[i].c_str();
	}

	std::vector<GLuint> Indices(MaxUniformIndices);
	GLuint BlockIndex = GetUniformBlockIndex(BlockName);
	if (BlockIndex != GL_INVALID_INDEX)
	{
		glGetUniformIndices(_Program, static_cast<GLsizei>(Names.size()), NamePtrs.data(), Indices.data());
		glGetActiveUniformsiv(_Program, static_cast<GLsizei>(Names.size()), Indices.data(), GL_UNIFORM_OFFSET, Offsets.data());
	}

	return Offsets;
}

_ASSET_END
_NPGS_END
