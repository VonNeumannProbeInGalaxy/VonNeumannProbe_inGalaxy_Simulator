#pragma once

#include "UniformBlockManager.h"

_NPGS_BEGIN
_UTIL_BEGIN

template<typename DataType>
inline void FUniformBlockManager::UpdateEntrieBlock(const std::string& BlockName, const DataType& Data) const
{
	auto it = _SharedBlocks.find(BlockName);
	if (it == _SharedBlocks.end())
	{
		return;
	}

	const FBlockInfo& Info = it->second;
	if (sizeof(DataType) != static_cast<std::size_t>(Info.Size))
	{
		std::println("Warning: Size mismatch for block \"{}\"", BlockName);
		return;
	}

	glNamedBufferSubData(Info.Buffer, 0, Info.Size, &Data);
}

template<typename DataType>
NPGS_INLINE FUniformBlockManager::TUpdater<DataType> FUniformBlockManager::GetBlockUpdater(const std::string& BlockName,
																						   const std::string& MemberName) const
{
	auto& BlockInfo = _SharedBlocks.at(BlockName);
	return TUpdater<DataType>(BlockInfo.Buffer, BlockInfo.Offsets.at(MemberName));
}

NPGS_INLINE GLuint FUniformBlockManager::GetBlockIndex(const std::string& BlockName, GLuint Program) const
{
	GLuint BlockIndex = glGetUniformBlockIndex(Program, BlockName.c_str());
	return BlockIndex;
}

NPGS_INLINE GLint FUniformBlockManager::GetBlockSize(const std::string& BlockName, GLuint BlockIndex, GLuint Program) const
{
	GLint BlockSize = 0;
	glGetActiveUniformBlockiv(Program, BlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &BlockSize);
	return BlockSize;
}

_UTIL_END
_NPGS_END
