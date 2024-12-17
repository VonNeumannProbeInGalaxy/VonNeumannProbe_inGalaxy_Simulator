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

namespace
{

std::string GetIncludeDirectory(const std::string& Filepath);
std::string GetIncludeFilename(const std::string& Statement);
std::string GetTypeName(GLenum Type);
GLint GetActualSize(GLenum Type);
GLint GetStd140Alignment(GLenum Type);
GLint GetStd430Alignment(GLenum Type);
GLint GetStd140Stride(GLenum Type, GLint ArraySize);
GLint GetStd430Stride(GLenum Type, GLint ArraySize);

}

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

	// 根据布局类型选择不同的偏移量计算方式
	if (Layout == UniformBlockLayout::kShared)
	{
		std::vector<GLint> Offsets = GetUniformBlockOffsets(BlockName, MemberNames);
		for (std::size_t i = 0; i != MemberNames.size(); ++i)
		{
			BlockInfo.Offsets[MemberNames[i]] = Offsets[i];
		}
	}
	else
	{
		auto TypeInfos = GetUniformTypeInfos(BlockName, MemberNames);
		std::vector<GLint> Offsets = ComputeStandardLayoutOffsets(TypeInfos, Layout);
		for (std::size_t i = 0; i != MemberNames.size(); ++i)
		{
			BlockInfo.Offsets[MemberNames[i]] = Offsets[i];
		}
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

void Shader::VerifyUniformBlockLayout(const std::string& BlockName) const
{
	auto it = _UniformBlocks.find(BlockName);
	if (it == _UniformBlocks.end())
	{
		std::println("Block \"{}\" not found", BlockName);
		return;
	}

	const auto& Block = it->second;
	std::println("Uniform block \"{}\" layout ({}):", BlockName,
				 Block.Layout == UniformBlockLayout::kShared ? "shared" :
				 Block.Layout == UniformBlockLayout::kStd140 ? "std140" : "std430");

	// 获取所有成员的类型信息
	GLint ActiveUniforms = 0;
	glGetActiveUniformBlockiv(_Program, Block.Index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &ActiveUniforms);

	for (const auto& [Name, Offset] : Block.Offsets) {
		GLint Size = 0, ArrayStride = 0, Type = 0;
		GLuint Index = 0;
		const GLchar* NameStr = Name.c_str();
		glGetUniformIndices(_Program, 1, &NameStr, &Index);
		if (Index != GL_INVALID_INDEX) {
			glGetActiveUniformsiv(_Program, 1, &Index, GL_UNIFORM_SIZE, &Size);
			glGetActiveUniformsiv(_Program, 1, &Index, GL_UNIFORM_ARRAY_STRIDE, &ArrayStride);
			glGetActiveUniformsiv(_Program, 1, &Index, GL_UNIFORM_TYPE, &Type);

			std::println("- {:<16} offset = {:<4} size = {:<4} type = {:<7} array_stride = {:<4}",
						 Name, Offset, Size, GetTypeName(Type), ArrayStride);
		}
	}

	std::println("Total size: {} bytes", Block.Size);
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

std::vector<GLint> Shader::ComputeStandardLayoutOffsets(const std::vector<UniformTypeInfo>& TypeInfos, UniformBlockLayout Layout) const
{
	std::vector<GLint> Offsets;
	GLint CurrentOffset = 0;

	for (const auto& TypeInfo : TypeInfos)
	{
		GLint Alignment = (Layout == UniformBlockLayout::kStd140)
						? GetStd140Alignment(TypeInfo.Type)
						: GetStd430Alignment(TypeInfo.Type);

		// 按布局规则对齐
		CurrentOffset = (CurrentOffset + Alignment - 1) & ~(Alignment - 1);
		Offsets.emplace_back(CurrentOffset);

		if (TypeInfo.Size > 1)
		{
			// 数组类型
			GLint Stride = (Layout == UniformBlockLayout::kStd140)
						 ? GetStd140Stride(TypeInfo.Type, TypeInfo.Size)
						 : GetStd430Stride(TypeInfo.Type, TypeInfo.Size);

			if (TypeInfo.MatrixCount > 1)
			{
				Stride *= TypeInfo.MatrixCount;
			}

			CurrentOffset += Stride;
		}
		else
		{
			// 非数组类型 - 使用实际大小
			CurrentOffset += GetActualSize(TypeInfo.Type);
		}
	}

	return Offsets;
}

std::vector<Shader::UniformTypeInfo> Shader::GetUniformTypeInfos(const std::string& BlockName,
																 const std::vector<std::string>& MemberNames) const
{
	std::vector<UniformTypeInfo> TypeInfos;
	TypeInfos.reserve(MemberNames.size());

	GLuint BlockIndex = GetUniformBlockIndex(BlockName);
	if (BlockIndex == GL_INVALID_INDEX)
	{
		return TypeInfos;
	}

	std::vector<GLuint> Indices(MemberNames.size());
	std::vector<const GLchar*> NamePtrs(MemberNames.size());

	for (std::size_t i = 0; i != MemberNames.size(); ++i)
	{
		NamePtrs[i] = MemberNames[i].c_str();
	}

	glGetUniformIndices(_Program, static_cast<GLsizei>(MemberNames.size()), NamePtrs.data(), Indices.data());

	// 获取每个成员的类型信息
	std::vector<GLint> Types(MemberNames.size());
	std::vector<GLint> ArrayStrides(MemberNames.size());
	std::vector<GLint> Sizes(MemberNames.size());

	glGetActiveUniformsiv(_Program, static_cast<GLsizei>(MemberNames.size()), Indices.data(), GL_UNIFORM_TYPE, Types.data());
	glGetActiveUniformsiv(_Program, static_cast<GLsizei>(MemberNames.size()), Indices.data(), GL_UNIFORM_ARRAY_STRIDE, ArrayStrides.data());
	glGetActiveUniformsiv(_Program, static_cast<GLsizei>(MemberNames.size()), Indices.data(), GL_UNIFORM_SIZE, Sizes.data());

	for (std::size_t i = 0; i != MemberNames.size(); ++i)
	{
		if (Indices[i] != GL_INVALID_INDEX)
		{
			UniformTypeInfo TypeInfo;
			TypeInfo.Type = static_cast<GLenum>(Types[i]);
			TypeInfo.MatrixCount = 1;
			// 特殊处理矩阵类型
			switch (Types[i])
			{
			case GL_FLOAT_MAT2:
				TypeInfo.ArrayStride = 16; // 每列 vec2，16 字节对齐
				TypeInfo.Size = 2;         // 2 列
				break;
			case GL_FLOAT_MAT2x3:
				TypeInfo.ArrayStride = 16; // 每列 vec3，16 字节对齐
				TypeInfo.Size = 2;         // 2 列
				break;
			case GL_FLOAT_MAT2x4:
				TypeInfo.ArrayStride = 16; // 每列 vec4，16 字节对齐
				TypeInfo.Size = 2;         // 2 列
				break;
			case GL_FLOAT_MAT3x2:
				TypeInfo.ArrayStride = 16; // 每列 vec2，16 字节对齐
				TypeInfo.Size = 3;         // 3 列
				break;
			case GL_FLOAT_MAT3:
				TypeInfo.ArrayStride = 16; // 每列 vec3，16 字节对齐
				TypeInfo.Size = 3;         // 3 列
				break;
			case GL_FLOAT_MAT3x4:
				TypeInfo.ArrayStride = 16; // 每列 vec4，16 字节对齐
				TypeInfo.Size = 3;         // 3 列
				break;
			case GL_FLOAT_MAT4x2:
				TypeInfo.ArrayStride = 16; // 每列 vec2，16 字节对齐
				TypeInfo.Size = 4;         // 4 列
				break;
			case GL_FLOAT_MAT4x3:
				TypeInfo.ArrayStride = 16; // 每列 vec3，16 字节对齐
				TypeInfo.Size = 4;         // 4 列
				break;
			case GL_FLOAT_MAT4:
				TypeInfo.ArrayStride = 16; // 每列 vec4，16 字节对齐
				TypeInfo.Size = 4;         // 4 列
				break;
			default:
				// 非矩阵类型使用原有逻辑
				TypeInfo.ArrayStride = ArrayStrides[i];
				TypeInfo.MatrixCount = 0;
				TypeInfo.Size = Sizes[i];
				break;
			}

			if (TypeInfo.MatrixCount != 0 && Sizes[i] > 1)
			{
				TypeInfo.MatrixCount = Sizes[i];
			}

			TypeInfo.BaseAlign = (TypeInfo.Size > 1)
							   ? 16                                 // 数组总是 16 字节对齐
							   : GetStd140Alignment(TypeInfo.Type); // 非数组使用基本对齐

			TypeInfos.emplace_back(TypeInfo);
		}
		else
		{
			// 如果找不到成员，添加一个默认的类型信息
			UniformTypeInfo TypeInfo{};
			TypeInfo.Type        = GL_FLOAT; // 默认类型
			TypeInfo.ArrayStride = 0;
			TypeInfo.MatrixCount = 0;
			TypeInfo.Size        = 1;
			TypeInfo.BaseAlign   = 4;
			TypeInfos.emplace_back(TypeInfo);

			std::println("Warning: Uniform \"{}\" not found in block \"{}\"", MemberNames[i], BlockName);
		}
	}

	return TypeInfos;
}

namespace
{

std::string GetIncludeDirectory(const std::string& Filepath)
{
	return (std::filesystem::path(Filepath).parent_path().string() + '/');
}

std::string GetIncludeFilename(const std::string& Statement)
{
	std::string::size_type FirstQuotePosition = Statement.find_first_of("\"");
	std::string::size_type LastQuotePosition  = Statement.find_last_of("\"");

	return Statement.substr(FirstQuotePosition + 1, LastQuotePosition - FirstQuotePosition - 1);
}

std::string GetTypeName(GLenum Type)
{
	switch (Type)
	{
	case GL_BOOL:		             return "bool";
	case GL_BOOL_VEC2:	             return "bvec2";
	case GL_BOOL_VEC3:	             return "bvec3";
	case GL_BOOL_VEC4:	             return "bvec4";
	case GL_INT:		             return "int";
	case GL_INT_VEC2:	             return "ivec2";
	case GL_INT_VEC3:	             return "ivec3";
	case GL_INT_VEC4:	             return "ivec4";
	case GL_UNSIGNED_INT:            return "uint";
	case GL_UNSIGNED_INT_VEC2:       return "uvec2";
	case GL_UNSIGNED_INT_VEC3:       return "uvec3";
	case GL_UNSIGNED_INT_VEC4:       return "uvec4";
	case GL_INT64_ARB:               return "int64_t";
	case GL_INT64_VEC2_ARB:          return "i64vec2";
	case GL_INT64_VEC3_ARB:          return "i64vec3";
	case GL_INT64_VEC4_ARB:          return "i64vec4";
	case GL_UNSIGNED_INT64_ARB:      return "uint64_t";
	case GL_UNSIGNED_INT64_VEC2_ARB: return "u64vec2";
	case GL_UNSIGNED_INT64_VEC3_ARB: return "u64vec3";
	case GL_UNSIGNED_INT64_VEC4_ARB: return "u64vec4";
	case GL_FLOAT:                   return "float";
	case GL_FLOAT_VEC2:              return "vec2";
	case GL_FLOAT_VEC3:              return "vec3";
	case GL_FLOAT_VEC4:              return "vec4";
	case GL_DOUBLE:		             return "double";
	case GL_DOUBLE_VEC2:             return "dvec2";
	case GL_DOUBLE_VEC3:             return "dvec3";
	case GL_DOUBLE_VEC4:             return "dvec4";
	case GL_FLOAT_MAT2:	             return "mat2x2";
	case GL_FLOAT_MAT2x3:            return "mat2x3";
	case GL_FLOAT_MAT2x4:            return "mat2x4";
	case GL_FLOAT_MAT3x2:            return "mat3x2";
	case GL_FLOAT_MAT3:	             return "mat3x3";
	case GL_FLOAT_MAT3x4:            return "mat3x4";
	case GL_FLOAT_MAT4x2:            return "mat4x2";
	case GL_FLOAT_MAT4x3:            return "mat4x3";
	case GL_FLOAT_MAT4:	             return "mat4x4";
	case GL_DOUBLE_MAT2:             return "dmat2x2";
	case GL_DOUBLE_MAT2x3:           return "dmat2x3";
	case GL_DOUBLE_MAT2x4:           return "dmat2x4";
	case GL_DOUBLE_MAT3x2:           return "dmat3x2";
	case GL_DOUBLE_MAT3:             return "dmat3x3";
	case GL_DOUBLE_MAT3x4:           return "dmat3x4";
	case GL_DOUBLE_MAT4x2:           return "dmat4x2";
	case GL_DOUBLE_MAT4x3:           return "dmat4x3";
	case GL_DOUBLE_MAT4:             return "dmat4x4";
	case GL_SAMPLER_1D:              return "sampler1D";
	case GL_SAMPLER_2D:              return "sampler2D";
	case GL_SAMPLER_3D:              return "sampler3D";
	case GL_SAMPLER_CUBE:            return "samplerCube";
	case GL_SAMPLER_2D_SHADOW:       return "sampler2DShadow";
	case GL_IMAGE_1D:                return "image1D";
	case GL_IMAGE_2D:                return "image2D";
	case GL_IMAGE_3D:                return "image3D";
	case GL_IMAGE_CUBE:			     return "imageCube";
	default:                         return "unknown";
	}
}

GLint GetActualSize(GLenum Type)
{
	switch (Type)
	{
	// 标量类型
	case GL_FLOAT:
	case GL_INT:
	case GL_UNSIGNED_INT:
	case GL_BOOL:
		return 4;  // 4 字节

	// 向量类型
	case GL_FLOAT_VEC2:
	case GL_INT_VEC2:
	case GL_UNSIGNED_INT_VEC2:
	case GL_BOOL_VEC2:
		return 8;  // 8 字节
	case GL_FLOAT_VEC3:
	case GL_INT_VEC3:
	case GL_UNSIGNED_INT_VEC3:
	case GL_BOOL_VEC3:
		return 12; // 12 字节（实际大小）
	case GL_FLOAT_VEC4:
	case GL_INT_VEC4:
	case GL_UNSIGNED_INT_VEC4:
	case GL_BOOL_VEC4:
		return 16; // 16 字节

	// 矩阵类型 - 按列存储
	case GL_FLOAT_MAT2:
		return 16; // 2x2: 2 列，每列 vec2 (8 字节)
	case GL_FLOAT_MAT2x3:
		return 24; // 2x3: 2 列，每列 vec3 (12 字节)
	case GL_FLOAT_MAT2x4:
		return 32; // 2x4: 2 列，每列 vec4 (16 字节)
	case GL_FLOAT_MAT3x2:
		return 24; // 3x2: 3 列，每列 vec2 (8 字节)
	case GL_FLOAT_MAT3:
		return 36; // 3x3: 3 列，每列 vec3 (12 字节)
	case GL_FLOAT_MAT3x4:
		return 48; // 3x4: 3 列，每列 vec4 (16 字节)
	case GL_FLOAT_MAT4x2:
		return 32; // 4x2: 4 列，每列 vec2 (8 字节)
	case GL_FLOAT_MAT4x3:
		return 48; // 4x3: 4 列，每列 vec3 (12 字节)
	case GL_FLOAT_MAT4:
		return 64; // 4x4: 4 列，每列 vec4 (16 字节)
	default:
		return 0;
	}
}

GLint GetStd140Alignment(GLenum Type)
{
	switch (Type)
	{
	case GL_FLOAT:
	case GL_INT:
	case GL_BOOL:
		return 4;  // 标量类型 4 字节对齐
	case GL_FLOAT_VEC2:
	case GL_INT_VEC2:
	case GL_BOOL_VEC2:
		return 8;  // 2 分量向量 8 字节对齐
	case GL_FLOAT_VEC3:
	case GL_FLOAT_VEC4:
	case GL_INT_VEC3:
	case GL_INT_VEC4:
	case GL_BOOL_VEC3:
	case GL_BOOL_VEC4:
	case GL_FLOAT_MAT2:
	case GL_FLOAT_MAT2x3:
	case GL_FLOAT_MAT2x4:
	case GL_FLOAT_MAT3x2:
	case GL_FLOAT_MAT3:
	case GL_FLOAT_MAT3x4:
	case GL_FLOAT_MAT4x2:
	case GL_FLOAT_MAT4x3:
	case GL_FLOAT_MAT4:
		return 16;  // 3/4 分量向量和矩阵 16 字节对齐
	default:
		return 4;   // 默认 4 字节对齐
	}
}

GLint GetStd430Alignment(GLenum Type)
{
	switch (Type)
	{
	case GL_FLOAT:
	case GL_INT:
	case GL_BOOL:
		return 4;  // 标量类型 4 字节对齐
	case GL_FLOAT_VEC2:
	case GL_INT_VEC2:
		return 8;  // 2 分量向量 8 字节对齐
	case GL_FLOAT_VEC3:
	case GL_INT_VEC3:
		return 16; // 3 分量向量 16 字节对齐
	case GL_FLOAT_VEC4:
	case GL_INT_VEC4:
		return 16; // 4 分量向量 16 字节对齐
	case GL_FLOAT_MAT2:
		return 8;  // 2x2 矩阵 8 字节对齐
	case GL_FLOAT_MAT3:
	case GL_FLOAT_MAT4:
		return 16; // 3x3 和 4x4 矩阵 16 字节对齐
	default:
		return 4;  // 默认 4 字节对齐
	}
}

GLint GetStd140Stride(GLenum Type, GLint ArraySize)
{
	GLint BaseSize = 0;

	switch (Type)
	{
	case GL_FLOAT:
	case GL_INT:
	case GL_BOOL:
		BaseSize = 4;  // 标量类型在 std140 中占用 4 字节
		if (ArraySize > 1)
		{
			return BaseSize * ArraySize; // 基本类型数组不需要 16 字节对齐
		}
		break;
	case GL_FLOAT_VEC2:
	case GL_INT_VEC2:
		BaseSize = 8;  // vec2 在 std140 中占用 8 字节
		break;
	case GL_FLOAT_VEC3:
	case GL_INT_VEC3:
		BaseSize = 16; // vec3 在 std140 中占用 16 字节
		break;
	case GL_FLOAT_VEC4:
	case GL_INT_VEC4:
		BaseSize = 16; // vec4 在 std140 中占用 16 字节
		break;

	// 矩阵类型特殊处理
	case GL_FLOAT_MAT2:
		BaseSize = ArraySize > 1 ? 32 : 16; // 单个 mat2 占用 16 字节，数组时 32 字节对齐
		return BaseSize;
	case GL_FLOAT_MAT2x3:
		BaseSize = ArraySize > 1 ? 32 : 24;
		return BaseSize;
	case GL_FLOAT_MAT2x4:
		BaseSize = 32;
		return BaseSize;
	case GL_FLOAT_MAT3x2:
		BaseSize = ArraySize > 1 ? 48 : 24;
		return BaseSize;
	case GL_FLOAT_MAT3:
		BaseSize = ArraySize > 1 ? 48 : 48; // 3 列，每列 16 字节
		return BaseSize;
	case GL_FLOAT_MAT3x4:
		BaseSize = 48;
		return BaseSize;
	case GL_FLOAT_MAT4x2:
		BaseSize = ArraySize > 1 ? 64 : 32;
		return BaseSize;
	case GL_FLOAT_MAT4x3:
		BaseSize = ArraySize > 1 ? 64 : 48;
		return BaseSize;
	case GL_FLOAT_MAT4:
		BaseSize = 64;
		return BaseSize;
	}

	if (ArraySize > 1)
	{
		// 只对非基本类型的数组执行 16 字节对齐
		if (Type != GL_FLOAT && Type != GL_INT && Type != GL_BOOL)
		{
			BaseSize = (BaseSize + 15) & ~15;
		}
		return BaseSize * ArraySize;
	}

	return BaseSize;
}

GLint GetStd430Stride(GLenum Type, GLint ArraySize)
{
	GLint BaseSize = 0;
	GLint Alignment = GetStd430Alignment(Type);

	switch (Type)
	{
	case GL_FLOAT:
	case GL_INT:
	case GL_BOOL:
		BaseSize = 4;
		break;
	case GL_FLOAT_VEC2:
	case GL_INT_VEC2:
		BaseSize = 8;
		break;
	case GL_FLOAT_VEC3:
	case GL_INT_VEC3:
		BaseSize = 12; // vec3 在 std430 中紧密排列
		break;
	case GL_FLOAT_VEC4:
	case GL_INT_VEC4:
		BaseSize = 16;
		break;
	case GL_FLOAT_MAT2:
		BaseSize = 16; // 2x2 矩阵，每列 vec2，共 2 列
		break;
	case GL_FLOAT_MAT2x3:
		BaseSize = 24; // 2x3 矩阵，每列 vec3，共 2 列
		break;
	case GL_FLOAT_MAT2x4:
		BaseSize = 32; // 2x4 矩阵，每列 vec4，共 2 列
		break;
	case GL_FLOAT_MAT3x2:
		BaseSize = 24; // 3x2 矩阵，每列 vec2，共 3 列
		break;
	case GL_FLOAT_MAT3:
		BaseSize = 36; // 3x3 矩阵，每列 vec3，共 3 列
		break;
	case GL_FLOAT_MAT3x4:
		BaseSize = 48; // 3x4 矩阵，每列 vec4，共 3 列
		break;
	case GL_FLOAT_MAT4x2:
		BaseSize = 32; // 4x2 矩阵，每列 vec2，共 4 列
		break;
	case GL_FLOAT_MAT4x3:
		BaseSize = 48; // 4x3 矩阵，每列 vec3，共 4 列
		break;
	case GL_FLOAT_MAT4:
		BaseSize = 64; // 4x4 矩阵，每列 vec4，共 4 列
		break;
	default:
		BaseSize = 0; // 未知类型，大小为 0
		break;
	}

	if (ArraySize > 1)
	{
		// 数组元素按基本类型对齐
		GLint Alignment = GetStd430Alignment(Type);
		BaseSize = (BaseSize + Alignment - 1) & ~(Alignment - 1);
		return BaseSize * ArraySize;
	}

	return BaseSize;
}

}

_ASSET_END
_NPGS_END
