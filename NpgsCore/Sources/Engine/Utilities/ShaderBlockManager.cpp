#include "ShaderBlockManager.h"

#include <algorithm>
#include <print>
#include <utility>

_NPGS_BEGIN
_UTIL_BEGIN

namespace
{
	std::string GetTypeName(GLenum Type);
}

void FBlockManagerBase::VerifyBlockLayout(GLuint Program, const std::string& BlockName) const
{
	const auto* Block = GetBlock(BlockName);
	if (Block == nullptr)
	{
		std::println("Block \"{}\" not found", BlockName);
		return;
	}

	std::println("{} block \"{}\" layout ({}):", GetBlockTypeName(), BlockName,
				 Block->Layout == EBlockLayout::kShared ? "shared" :
				 Block->Layout == EBlockLayout::kStd140 ? "std140" : "std430");

	auto MemberInfos = GetMemberInfos(Program, BlockName);

	// 按偏移量排序
	std::sort(MemberInfos.begin(), MemberInfos.end(), [](const auto& Lhs, const auto& Rhs) -> bool
	{
		return Lhs.Offset < Rhs.Offset;
	});

	// 打印成员信息
	for (const auto& Info : MemberInfos)
	{
		std::println("- {:<20} offset = {:<4} size = {:<4} type = {:<7} array_stride = {:<4}",
					 Info.Name, Info.Offset, Info.Size, GetTypeName(Info.Type), Info.ArrayStride);
	}

	std::println("Total size: {} bytes", Block->Size);
}

FUniformBlockManager::FBlockInfo* FUniformBlockManager::CreateSharedBlock(GLuint Program, const std::string& BlockName,
																		  GLuint BindingPoint,
																		  const std::vector<std::string>& MemberNames,
																		  EBlockLayout Layout)
{
	auto it = _SharedBlocks.find(BlockName);
	if (it != _SharedBlocks.end())
	{
		return &it->second;
	}

	return CreateBlock(BlockName, BindingPoint, MemberNames, Layout, Program);
}

void FUniformBlockManager::BindShaderToBlock(GLuint Program, const std::string& BlockName) const
{
	auto it = _SharedBlocks.find(BlockName);
	if (it != _SharedBlocks.end())
	{
		GLuint BlockIndex = GetBlockIndex(BlockName, Program);
		glUniformBlockBinding(Program, BlockIndex, it->second.BindingPoint);
	}
}

FUniformBlockManager* FUniformBlockManager::GetInstance()
{
	static FUniformBlockManager Instance;
	return &Instance;
}

FUniformBlockManager::~FUniformBlockManager()
{
	for (auto& [Name, Info] : _SharedBlocks)
	{
		glDeleteBuffers(1, &Info.Buffer);
	}
	_SharedBlocks.clear();
}

FUniformBlockManager::FBlockInfo* FUniformBlockManager::CreateBlock(const std::string& BlockName, GLuint BindingPoint,
																	const std::vector<std::string>& MemberNames,
																	EBlockLayout Layout, GLuint Program)
{
	FBlockInfo Info;
	Info.Layout = Layout;
	GLuint BlockIndex = GetBlockIndex(BlockName, Program);
	if (BlockIndex == GL_INVALID_INDEX)
	{
		return nullptr;
	}

	std::vector<GLint> Offsets = GetBlockOffsets(BlockName, MemberNames, Program);
	for (std::size_t i = 0; i != MemberNames.size(); ++i)
	{
		Info.Offsets[MemberNames[i]] = Offsets[i];
	}

	Info.Size = GetBlockSize(BlockName, BlockIndex, Program);

	glCreateBuffers(1, &Info.Buffer);
	glNamedBufferData(Info.Buffer, Info.Size, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, BindingPoint, Info.Buffer);

	std::vector<const GLchar*> NamePtrs(MemberNames.size());
	for (const auto& Names : MemberNames)
	{
		NamePtrs.emplace_back(Names.c_str());
	}

	_SharedBlocks[BlockName] = Info;
	return &_SharedBlocks[BlockName];
}

std::vector<GLint> FUniformBlockManager::GetBlockOffsets(const std::string& BlockName,
														 const std::vector<std::string>& Names, GLuint Program) const
{
	std::vector<GLint> Offsets(Names.size(), -1);

	GLuint BlockIndex = GetBlockIndex(BlockName, Program);
	if (BlockIndex == GL_INVALID_INDEX)
	{
		return Offsets;
	}

	std::vector<const GLchar*> NamePtrs;
	std::vector<GLuint> Indices(Names.size());

	NamePtrs.reserve(Names.size());
	for (const auto& Name : Names)
	{
		NamePtrs.emplace_back(Name.c_str());
	}

	glGetUniformIndices(Program, static_cast<GLsizei>(Names.size()), NamePtrs.data(), Indices.data());

	bool bAllValid = true;
	for (GLuint i = 0; i < Names.size(); ++i)
	{
		if (Indices[i] == GL_INVALID_INDEX)
		{
			std::println("Warning: Uniform \"{}\" not found in block \"{}\"", Names[i], BlockName);
			bAllValid = false;
		}
	}

	if (bAllValid)
	{
		glGetActiveUniformsiv(Program, static_cast<GLsizei>(Names.size()), Indices.data(), GL_UNIFORM_OFFSET, Offsets.data());
	}

	return Offsets;
}

std::vector<FUniformBlockManager::FMemberInfo>
FUniformBlockManager::GetMemberInfos(GLuint Program, const std::string& BlockName) const
{
	std::vector<FMemberInfo> MemberInfos;
	GLuint BlockIndex = GetBlockIndex(BlockName, Program);
	if (BlockIndex == GL_INVALID_INDEX)
	{
		return MemberInfos;
	}

	GLint MemberCount = 0;
	glGetActiveUniformBlockiv(Program, BlockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &MemberCount);

	std::vector<GLuint> MemberIndices(MemberCount);
	glGetActiveUniformBlockiv(Program, BlockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES,
							  reinterpret_cast<GLint*>(MemberIndices.data()));

	for (GLuint Index : MemberIndices)
	{
		FMemberInfo Info;

		GLint NameLength = 0;
		glGetActiveUniformsiv(Program, 1, &Index, GL_UNIFORM_NAME_LENGTH, &NameLength);

		std::vector<GLchar> NameBuffer(NameLength);
		glGetActiveUniformName(Program, Index, NameLength, nullptr, NameBuffer.data());
		Info.Name = std::string(NameBuffer.data());

		glGetActiveUniformsiv(Program, 1, &Index, GL_UNIFORM_TYPE, reinterpret_cast<GLint*>(&Info.Type));
		glGetActiveUniformsiv(Program, 1, &Index, GL_UNIFORM_OFFSET, &Info.Offset);
		glGetActiveUniformsiv(Program, 1, &Index, GL_UNIFORM_SIZE, &Info.Size);
		glGetActiveUniformsiv(Program, 1, &Index, GL_UNIFORM_ARRAY_STRIDE, &Info.ArrayStride);

		MemberInfos.emplace_back(Info);
	}

	return MemberInfos;
}

FStorageBlockManager::FBlockInfo* FStorageBlockManager::CreateSharedBlock(GLuint Program, const std::string& BlockName,
																		  GLuint BindingPoint,
																		  const std::vector<std::string>& MemberNames,
																		  EBlockLayout Layout)
{
	auto it = _StorageBlocks.find(BlockName);
	if (it != _StorageBlocks.end())
	{
		return &it->second;
	}

	return CreateBlock(BlockName, BindingPoint, MemberNames, Layout, Program);
}

void FStorageBlockManager::BindShaderToBlock(GLuint Program, const std::string& BlockName) const
{
	auto it = _StorageBlocks.find(BlockName);
	if (it != _StorageBlocks.end())
	{
		GLuint BlockIndex = GetBlockIndex(BlockName, Program);
		glShaderStorageBlockBinding(Program, BlockIndex, it->second.BindingPoint);
	}
}

FStorageBlockManager* FStorageBlockManager::GetInstance()
{
	static FStorageBlockManager Instance;
	return &Instance;
}

FStorageBlockManager::~FStorageBlockManager()
{
	for (auto& [Name, Info] : _StorageBlocks)
	{
		glDeleteBuffers(1, &Info.Buffer);
	}
	_StorageBlocks.clear();
}

FStorageBlockManager::FBlockInfo* FStorageBlockManager::CreateBlock(const std::string& BlockName, GLuint BindingPoint,
																	const std::vector<std::string>& MemberNames,
																	EBlockLayout Layout, GLuint Program)
{
	FBlockInfo Info;
	Info.Layout = Layout;
	GLuint BlockIndex = GetBlockIndex(BlockName, Program);
	if (BlockIndex == GL_INVALID_INDEX)
	{
		return nullptr;
	}

	std::vector<GLint> Offsets = GetBlockOffsets(BlockName, MemberNames, Program);
	for (std::size_t i = 0; i != MemberNames.size(); ++i)
	{
		Info.Offsets[MemberNames[i]] = Offsets[i];

		static const GLenum Properties[] = { GL_TOP_LEVEL_ARRAY_SIZE, GL_ARRAY_STRIDE };
		GLint Results[2];
		glGetProgramResourceiv(Program, GL_BUFFER_VARIABLE, BlockIndex, 2, Properties, 2, nullptr, Results);
		Info.bIsDynamicArray = (Results[0] == 0);
		Info.ArrayStride     = Results[1];
	}

	Info.Size = GetBlockSize(BlockName, BlockIndex, Program);

	glCreateBuffers(1, &Info.Buffer);
	glNamedBufferStorage(Info.Buffer, Info.Size, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BindingPoint, Info.Buffer);

	_StorageBlocks[BlockName] = Info;
	return &_StorageBlocks[BlockName];
}

std::vector<GLint> FStorageBlockManager::GetBlockOffsets(const std::string& BlockName,
														 const std::vector<std::string>& Names, GLuint Program) const
{
	std::vector<GLint> Offsets;

	for (std::size_t i = 0; i != Names.size(); ++i)
	{
		GLuint Index = glGetProgramResourceIndex(Program, GL_BUFFER_VARIABLE, Names[i].c_str());
		if (Index != GL_INVALID_INDEX)
		{
			const static GLenum Properties[]{ GL_OFFSET };
			GLint Offset = 0;
			glGetProgramResourceiv(Program, GL_BUFFER_VARIABLE, Index, 1, Properties, 1, nullptr, &Offset);
			Offsets.emplace_back(Offset);
		}
		else
		{
			Offsets.emplace_back(-1);
		}
	}

	return Offsets;
}

std::vector<FBlockManagerBase::FMemberInfo>
FStorageBlockManager::GetMemberInfos(GLuint Program, const std::string& BlockName) const
{
	std::vector<FMemberInfo> MemberInfos;
	GLuint BlockIndex = GetBlockIndex(BlockName, Program);
	if (BlockIndex == GL_INVALID_INDEX)
	{
		return MemberInfos;
	}

	static const GLenum MemberGetProperties[]{ GL_NUM_ACTIVE_VARIABLES, GL_ACTIVE_VARIABLES };
	GLint MemberCount = 0;
	glGetProgramResourceiv(Program, GL_SHADER_STORAGE_BLOCK, BlockIndex,
						   1, MemberGetProperties, 1, nullptr, &MemberCount);

	std::vector<GLint> MemberIndices(MemberCount);
	glGetProgramResourceiv(Program, GL_SHADER_STORAGE_BLOCK, BlockIndex,
						   1, &MemberGetProperties[1], MemberCount, nullptr, MemberIndices.data());

	for (GLint Index : MemberIndices)
	{
		FMemberInfo Info;

		GLint NameLength = 0;
		static const GLenum Properties[]
		{
			GL_NAME_LENGTH, GL_TYPE, GL_OFFSET, GL_ARRAY_SIZE, GL_ARRAY_STRIDE
		};
		GLint Results[5];
		glGetProgramResourceiv(Program, GL_BUFFER_VARIABLE, Index, 5, Properties, 5, nullptr, Results);

		std::vector<GLchar> NameBuffer(Results[0]);
		glGetProgramResourceName(Program, GL_BUFFER_VARIABLE, Index, Results[0], nullptr, NameBuffer.data());
		Info.Name        = std::string(NameBuffer.data());
		Info.Type        = static_cast<GLenum>(Results[1]);
		Info.Offset      = Results[2];
		Info.Size        = Results[3];
		Info.ArrayStride = Results[4];

		MemberInfos.emplace_back(Info);
	}

	return MemberInfos;
}

namespace
{
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
}

_UTIL_END
_NPGS_END
