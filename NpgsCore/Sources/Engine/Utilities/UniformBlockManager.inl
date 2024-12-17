#pragma once

#include "UniformBlockManager.h"

_NPGS_BEGIN
_UTIL_BEGIN

template<typename T>
inline void UniformBlockManager::UpdateEntrieBlock(const std::string& BlockName, const T& Data) const
{
	auto it = _SharedBlocks.find(BlockName);
	if (it == _SharedBlocks.end())
	{
		return;
	}

	const BlockInfo& Info = it->second;
	if (sizeof(T) != static_cast<std::size_t>(Info.Size))
	{
		std::println("Warning: Size mismatch for block \"{}\"", BlockName);
		return;
	}

	glNamedBufferSubData(Info.Buffer, 0, Info.Size, &Data);
}

template<typename T>
NPGS_INLINE UniformBlockManager::Updater<T> UniformBlockManager::GetBlockUpdater(const std::string& BlockName, const std::string& MemberName) const
{
	auto& BlockInfo = _SharedBlocks.at(BlockName);
	return Updater<T>(BlockInfo.Buffer, BlockInfo.Offsets.at(MemberName));
}

NPGS_INLINE GLuint UniformBlockManager::GetBlockIndex(const std::string& BlockName, GLuint Program) const
{
	GLuint BlockIndex = glGetUniformBlockIndex(Program, BlockName.c_str());
	return BlockIndex;
}

NPGS_INLINE GLint UniformBlockManager::GetBlockSize(const std::string& BlockName, GLuint BlockIndex, GLuint Program) const
{
	GLint BlockSize = 0;
	glGetActiveUniformBlockiv(Program, BlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &BlockSize);
	return BlockSize;
}

_UTIL_END
_NPGS_END
