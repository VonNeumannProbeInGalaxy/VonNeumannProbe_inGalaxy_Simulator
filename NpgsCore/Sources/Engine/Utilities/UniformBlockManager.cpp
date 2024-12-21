#include "UniformBlockManager.h"

#include <algorithm>
#include <print>
#include <utility>

_NPGS_BEGIN
_UTIL_BEGIN

namespace
{
	std::string GetTypeName(GLenum Type);
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

void FUniformBlockManager::VerifyBlockLayout(GLuint Program, const std::string& BlockName) const
{
	auto it = _SharedBlocks.find(BlockName);
	if (it == _SharedBlocks.end())
	{
		std::println("Block \"{}\" not found", BlockName);
		return;
	}

	const auto& Block = it->second;
	std::println("Uniform block \"{}\" layout ({}):", BlockName,
				 Block.Layout == EBlockLayout::kShared ? "shared" :
				 Block.Layout == EBlockLayout::kStd140 ? "std140" : "std430");

	std::vector<std::pair<std::string, GLint>> SortedMembers;
	SortedMembers.reserve(Block.Offsets.size());

	for (const auto& [Name, Offset] : Block.Offsets)
	{
		SortedMembers.emplace_back(Name, Offset);
	}

	std::sort(SortedMembers.begin(), SortedMembers.end(), [](const auto& Lhs, const auto& Rhs) -> bool
	{
		return Lhs.second < Rhs.second;
	});

	// 获取所有成员的类型信息

	for (const auto& [Name, Offset] : SortedMembers)
	{
		GLint  Size        = 0;
		GLint  ArrayStride = 0;
		GLint  Type        = 0;
		GLuint Index       = 0;
		const GLchar* NameStr = Name.c_str();
		glGetUniformIndices(Program, 1, &NameStr, &Index);
		if (Index != GL_INVALID_INDEX)
		{
			glGetActiveUniformsiv(Program, 1, &Index, GL_UNIFORM_SIZE, &Size);
			glGetActiveUniformsiv(Program, 1, &Index, GL_UNIFORM_ARRAY_STRIDE, &ArrayStride);
			glGetActiveUniformsiv(Program, 1, &Index, GL_UNIFORM_TYPE, &Type);

			std::println("- {:<20} offset = {:<4} size = {:<4} type = {:<7} array_stride = {:<4}",
						 Name, Offset, Size, GetTypeName(Type), ArrayStride);
		}
	}

	std::println("Total size: {} bytes", Block.Size);
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

std::vector<GLint> FUniformBlockManager::GetBlockOffsets(const std::string& BlockName, const std::vector<std::string>& Names, GLuint Program) const
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
