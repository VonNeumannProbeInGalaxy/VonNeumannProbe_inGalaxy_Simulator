#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <glad/glad.h>

#include "Engine/AssetLoader/Shader.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_UTIL_BEGIN

class UniformBlockManager
{
public:
	enum class BlockLayout
	{
		kShared,
		kStd140,
		kStd430
	};

	template <typename T>
	class Updater
	{
	public:
		Updater(GLuint Buffer, GLint Offset)
			: _Buffer(Buffer), _Offset(Offset)
		{
		}

		Updater& operator=(const T& Value)
		{
			Commit(Value);
			return *this;
		}

		void Commit(const T& Value) const
		{
			glNamedBufferSubData(_Buffer, _Offset, sizeof(T), &Value);
		}

	private:
		GLuint _Buffer;
		GLint  _Offset;
	};

	struct BlockInfo
	{
		GLuint Buffer{};
		GLuint BindingPoint{};
		GLint  Size{};
		std::unordered_map<std::string, GLint> Offsets{};
		BlockLayout Layout{ BlockLayout::kShared };
	};

public:
	BlockInfo* CreateSharedBlock(GLuint Program, const std::string& BlockName, GLuint BindingPoint,
								 const std::vector<std::string>& MemberNames, BlockLayout Layout = BlockLayout::kShared);

	void BindShaderToBlock(GLuint Program, const std::string& BlockName) const;
	void VerifyBlockLayout(GLuint Program, const std::string& BlockName) const;

	template <typename T>
	void UpdateBlockMember(const std::string& BlockName, const std::string& MemberName, const T& Value) const;

	template <typename T>
	Updater<T> GetBlockUpdater(const std::string& BlockName, const std::string& MemberName) const;

	static UniformBlockManager* GetInstance();

private:
	explicit UniformBlockManager()                  = default;
	UniformBlockManager(const UniformBlockManager&) = delete;
	UniformBlockManager(UniformBlockManager&&)      = delete;
	~UniformBlockManager();

	UniformBlockManager& operator=(const UniformBlockManager&) = delete;
	UniformBlockManager& operator=(UniformBlockManager&&)      = delete;

	BlockInfo* CreateBlock(const std::string& BlockName, GLuint BindingPoint,
						   const std::vector<std::string>& MemberNames, BlockLayout Layout, GLuint Program);

	GLuint GetBlockIndex(const std::string& BlockName, GLuint Program) const;
	GLint GetBlockSize(const std::string& BlockName, GLuint BlockIndex, GLuint Program) const;
	std::vector<GLint> GetBlockOffsets(const std::string& BlockName, const std::vector<std::string>& Names, GLuint Program) const;

private:
	std::unordered_map<std::string, BlockInfo> _SharedBlocks;
};

_UTIL_END
_NPGS_END

#include "UniformBlockManager.inl"
