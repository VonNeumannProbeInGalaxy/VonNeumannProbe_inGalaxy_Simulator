#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <glad/glad.h>

#include "Engine/AssetLoader/Shader.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_UTIL_BEGIN

class FUniformBlockManager
{
public:
	enum class EBlockLayout
	{
		kShared,
		kStd140,
		kStd430
	};

	template <typename DataType>
	class TUpdater
	{
	public:
		TUpdater(GLuint Buffer, GLint Offset)
			: _Buffer(Buffer), _Offset(Offset)
		{
		}

		TUpdater& operator<<(const DataType& Data)
		{
			Commit(Data);
			return *this;
		}

		void Commit(const DataType& Data) const
		{
			glNamedBufferSubData(_Buffer, _Offset, sizeof(DataType), &Data);
		}

	private:
		GLuint _Buffer;
		GLint  _Offset;
	};

	struct FBlockInfo
	{
		GLuint Buffer{};
		GLuint BindingPoint{};
		GLint  Size{};
		std::unordered_map<std::string, GLint> Offsets{};
		EBlockLayout Layout{ EBlockLayout::kShared };
	};

public:
	FBlockInfo* CreateSharedBlock(GLuint Program, const std::string& BlockName, GLuint BindingPoint,
								  const std::vector<std::string>& MemberNames, EBlockLayout Layout = EBlockLayout::kShared);

	void BindShaderToBlock(GLuint Program, const std::string& BlockName) const;
	void VerifyBlockLayout(GLuint Program, const std::string& BlockName) const;

	template <typename DataType>
	void UpdateEntrieBlock(const std::string& BlockName, const DataType& Data) const;

	template <typename DataType>
	TUpdater<DataType> GetBlockUpdater(const std::string& BlockName, const std::string& MemberName) const;

	static FUniformBlockManager* GetInstance();

private:
	explicit FUniformBlockManager()                   = default;
	FUniformBlockManager(const FUniformBlockManager&) = delete;
	FUniformBlockManager(FUniformBlockManager&&)      = delete;
	~FUniformBlockManager();

	FUniformBlockManager& operator=(const FUniformBlockManager&) = delete;
	FUniformBlockManager& operator=(FUniformBlockManager&&)      = delete;

	FBlockInfo* CreateBlock(const std::string& BlockName, GLuint BindingPoint,
							const std::vector<std::string>& MemberNames, EBlockLayout Layout, GLuint Program);

	GLuint GetBlockIndex(const std::string& BlockName, GLuint Program) const;
	GLint GetBlockSize(const std::string& BlockName, GLuint BlockIndex, GLuint Program) const;
	std::vector<GLint> GetBlockOffsets(const std::string& BlockName, const std::vector<std::string>& Names, GLuint Program) const;

private:
	std::unordered_map<std::string, FBlockInfo> _SharedBlocks;
};

_UTIL_END
_NPGS_END

#include "UniformBlockManager.inl"
