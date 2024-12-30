#pragma once

#include <cstdint>
#include <limits>
#include <utility>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "Engine/Core/Base.h"
#include "Engine/Utilities/Logger.h"

_NPGS_BEGIN

template <typename VulkanType>
class TVulkanWrapperBase
{
public:
	TVulkanWrapperBase() = default;
	TVulkanWrapperBase(const vk::Device& Device)
		:
		_Device(&Device),
		_Handle(VulkanType())
	{
	}

	TVulkanWrapperBase(const TVulkanWrapperBase&) = default;
	TVulkanWrapperBase(TVulkanWrapperBase&& Other) noexcept
		:
		_Device(std::exchange(Other._Device, nullptr)),
		_Handle(std::exchange(Other._Handle, VulkanType()))
	{
	}

	virtual ~TVulkanWrapperBase()
	{
		if (_Handle)
		{
			DestroyHandle();
		}
	}

	TVulkanWrapperBase& operator=(const TVulkanWrapperBase&) = default;
	TVulkanWrapperBase& operator=(TVulkanWrapperBase&& Other) noexcept
	{
		if (this != &Other)
		{
			if (_Handle)
			{
				DestroyHandle();
			}
			_Device = std::exchange(Other._Device, nullptr);
			_Handle = std::exchange(Other._Handle, VulkanType());
		}

		return *this;
	}

	VulkanType* operator->()
	{
		return &_Handle;
	}

	const VulkanType* operator->() const
	{
		return &_Handle;
	}

	VulkanType& operator*()
	{
		return _Handle;
	}

	const VulkanType& operator*() const
	{
		return _Handle;
	}

	explicit operator bool() const
	{
		return static_cast<bool>(_Handle);
	}

protected:
	virtual void DestroyHandle()
	{
		return;
	}

	const vk::Device* _Device;
	VulkanType        _Handle;
};

template <typename VulkanType>
class TVulkanWrapper;

template <>
class TVulkanWrapper<vk::CommandBuffer> : public TVulkanWrapperBase<vk::CommandBuffer>
{
public:
	using Base = TVulkanWrapperBase<vk::CommandBuffer>;
	using Base::Base;

	vk::Result Begin(const vk::CommandBufferInheritanceInfo& InheritanceInfo, const vk::CommandBufferUsageFlags& Flags = {})
	{
		vk::CommandBufferBeginInfo CommandBufferBeginInfo(Flags, &InheritanceInfo);
		try
		{
			this->_Handle.begin(CommandBufferBeginInfo);
		}
		catch (const vk::SystemError& Error)
		{
			NpgsCoreError("Failed to begin command buffer: {}", Error.what());
			return static_cast<vk::Result>(Error.code().value());
		}

		return vk::Result::eSuccess;
	}

	vk::Result Begin(const vk::CommandBufferUsageFlags& Flags = {})
	{
		vk::CommandBufferBeginInfo CommandBufferBeginInfo(Flags);
		try
		{
			this->_Handle.begin(CommandBufferBeginInfo);
		}
		catch (const vk::SystemError& Error)
		{
			NpgsCoreError("Failed to begin command buffer: {}", Error.what());
			return static_cast<vk::Result>(Error.code().value());
		}

		return vk::Result::eSuccess;
	}

	vk::Result End()
	{
		try
		{
			this->_Handle.end();
		}
		catch (const vk::SystemError& Error)
		{
			NpgsCoreError("Failed to end command buffer: {}", Error.what());
			return static_cast<vk::Result>(Error.code().value());
		}

		return vk::Result::eSuccess;
	}

	vk::CommandBuffer& GetHandle()
	{
		return this->_Handle;
	}

private:
	void DestroyHandle() override
	{
		return;
	}
};

template <>
class TVulkanWrapper<vk::CommandPool> : public TVulkanWrapperBase<vk::CommandPool>
{
public:
	using Base = TVulkanWrapperBase<vk::CommandPool>;
	using Base::Base;

	TVulkanWrapper(const vk::Device& Device, vk::CommandPoolCreateInfo& CreateInfo)
		: Base(Device)
	{
		CreateCommandPool(CreateInfo);
	}

	TVulkanWrapper(const vk::Device& Device, std::uint32_t QueueFamilyIndex, vk::CommandPoolCreateFlags Flags)
		: Base(Device)
	{
		CreateCommandPool(QueueFamilyIndex, Flags);
	}

	vk::Result AllocateBuffer(vk::CommandBufferLevel Level, vk::CommandBuffer& Buffer)
	{
		vk::CommandBufferAllocateInfo CommandBufferAllocateInfo(this->_Handle, Level, 1);
		try
		{
			Buffer = this->_Device->allocateCommandBuffers(CommandBufferAllocateInfo)[0];
		}
		catch (const vk::SystemError& Error)
		{
			NpgsCoreError("Failed to allocate command buffer: {}", Error.what());
			return static_cast<vk::Result>(Error.code().value());
		}

		NpgsCoreInfo("Command buffer allocated successfully.");
		return vk::Result::eSuccess;
	}

	vk::Result AllocateBuffer(vk::CommandBufferLevel Level, TVulkanWrapper<vk::CommandBuffer>& Buffer)
	{
		vk::CommandBufferAllocateInfo CommandBufferAllocateInfo(this->_Handle, Level, 1);
		try
		{
			Buffer.GetHandle() = this->_Device->allocateCommandBuffers(CommandBufferAllocateInfo)[0];
		}
		catch (const vk::SystemError& Error)
		{
			NpgsCoreError("Failed to allocate command buffer: {}", Error.what());
			return static_cast<vk::Result>(Error.code().value());
		}

		NpgsCoreInfo("Command buffer allocated successfully.");
		return vk::Result::eSuccess;
	}

	vk::Result AllocateBuffers(vk::CommandBufferLevel Level, std::vector<vk::CommandBuffer>& Buffers)
	{
		vk::CommandBufferAllocateInfo CommandBufferAllocateInfo(this->_Handle, Level, static_cast<std::uint32_t>(Buffers.size()));
		try
		{
			Buffers = this->_Device->allocateCommandBuffers(CommandBufferAllocateInfo);
		}
		catch (const vk::SystemError& Error)
		{
			NpgsCoreError("Failed to allocate command buffers: {}", Error.what());
			return static_cast<vk::Result>(Error.code().value());
		}

		NpgsCoreInfo("Command buffers allocated successfully.");
		return vk::Result::eSuccess;
	}

	vk::Result AllocateBuffers(vk::CommandBufferLevel Level, std::vector<TVulkanWrapper<vk::CommandBuffer>>& Buffers)
	{
		vk::CommandBufferAllocateInfo CommandBufferAllocateInfo(this->_Handle, Level, static_cast<std::uint32_t>(Buffers.size()));
		std::vector<vk::CommandBuffer> CommandBuffers;
		try
		{
			CommandBuffers = this->_Device->allocateCommandBuffers(CommandBufferAllocateInfo);
		}
		catch (const vk::SystemError& Error)
		{
			NpgsCoreError("Failed to allocate command buffers: {}", Error.what());
			return static_cast<vk::Result>(Error.code().value());
		}

		Buffers.resize(CommandBuffers.size());
		for (std::size_t i = 0; i != CommandBuffers.size(); ++i)
		{
			Buffers[i].GetHandle() = CommandBuffers[i];
		}

		NpgsCoreInfo("Command buffers allocated successfully.");
		return vk::Result::eSuccess;
	}

	vk::Result FreeBuffer(vk::CommandBufferLevel Level, vk::CommandBuffer& Buffer)
	{
		this->_Device->freeCommandBuffers(this->_Handle, Buffer);
		Buffer = vk::CommandBuffer();
		NpgsCoreInfo("Command buffer freed successfully.");
		return vk::Result::eSuccess;
	}

	vk::Result FreeBuffer(vk::CommandBufferLevel Level, TVulkanWrapper<vk::CommandBuffer>& Buffer)
	{
		return FreeBuffer(Level, Buffer.GetHandle());
	}

	vk::Result FreeBuffers(vk::CommandBufferLevel Level, std::vector<vk::CommandBuffer>& Buffers)
	{
		this->_Device->freeCommandBuffers(this->_Handle, Buffers);
		Buffers.clear();
		NpgsCoreInfo("Command buffers freed successfully.");
		return vk::Result::eSuccess;
	}

	vk::Result FreeBuffers(vk::CommandBufferLevel Level, std::vector<TVulkanWrapper<vk::CommandBuffer>>& Buffers)
	{
		std::vector<vk::CommandBuffer> CommandBuffers;
		CommandBuffers.reserve(Buffers.size());

		for (auto& Buffer : Buffers)
		{
			CommandBuffers.emplace_back(Buffer.GetHandle());
		}

		this->_Device->freeCommandBuffers(this->_Handle, CommandBuffers);
		Buffers.clear();

		NpgsCoreInfo("Command buffers freed successfully.");
		return vk::Result::eSuccess;
	}

private:
	vk::Result CreateCommandPool(vk::CommandPoolCreateInfo& CreateInfo)
	{
		try
		{
			this->_Handle = this->_Device->createCommandPool(CreateInfo);
		}
		catch (const vk::SystemError& Error)
		{
			NpgsCoreError("Failed to create command pool: {}", Error.what());
			return static_cast<vk::Result>(Error.code().value());
		}

		NpgsCoreInfo("Command pool created successfully.");
		return vk::Result::eSuccess;
	}

	vk::Result CreateCommandPool(std::uint32_t QueueFamilyIndex, vk::CommandPoolCreateFlags Flags)
	{
		vk::CommandPoolCreateInfo CommandPoolCreateInfo(Flags, QueueFamilyIndex);
		return CreateCommandPool(CommandPoolCreateInfo);
	}

	void DestroyHandle() override
	{
		this->_Device->destroyCommandPool(this->_Handle);
	}
};

template <>
class TVulkanWrapper<vk::Fence> : public TVulkanWrapperBase<vk::Fence>
{
public:
	using Base = TVulkanWrapperBase<vk::Fence>;
	using Base::Base;

	TVulkanWrapper(const vk::Device& Device, const vk::FenceCreateInfo& CreateInfo)
		: Base(Device)
	{
		CreateFence(CreateInfo);
	}

	TVulkanWrapper(const vk::Device& Device, const vk::FenceCreateFlags& Flags = {})
		: Base(Device)
	{
		CreateFence(Flags);
	}

	vk::Result Wait() const
	{
		vk::Result Result;
		try
		{
			Result = this->_Device->waitForFences(this->_Handle, vk::True, std::numeric_limits<std::uint64_t>::max());
		}
		catch (const vk::SystemError& Error)
		{
			NpgsCoreError("Failed to wait for fence: {}", Error.what());
			return static_cast<vk::Result>(Error.code().value());
		}

		return Result;
	}

	vk::Result Reset() const
	{
		try
		{
			this->_Device->resetFences(this->_Handle);
		}
		catch (const vk::SystemError& Error)
		{
			NpgsCoreError("Failed to reset fence: {}", Error.what());
			return static_cast<vk::Result>(Error.code().value());
		}

		return vk::Result::eSuccess;
	}

	vk::Result WaitAndReset() const
	{
		vk::Result WaitResult = Wait();
		if (WaitResult != vk::Result::eSuccess)
		{
			return WaitResult;
		}

		vk::Result ResetResult = Reset();
		return ResetResult;
	}

	vk::Result GetStatus() const
	{
		vk::Result Result = this->_Device->getFenceStatus(this->_Handle);
		if (Result != vk::Result::eSuccess)
		{
			NpgsCoreError("Failed to get fence status: {}.", vk::to_string(Result));
			return Result;
		}

		return vk::Result::eSuccess;
	}

private:
	vk::Result CreateFence(const vk::FenceCreateInfo& CreateInfo)
	{
		try
		{
			this->_Handle = this->_Device->createFence(CreateInfo);
		}
		catch (const vk::SystemError& Error)
		{
			NpgsCoreError("Failed to create fence: {}", Error.what());
			return static_cast<vk::Result>(Error.code().value());
		}

		NpgsCoreInfo("Fence created successfully.");
		return vk::Result::eSuccess;
	}

	vk::Result CreateFence(const vk::FenceCreateFlags& Flags)
	{
		vk::FenceCreateInfo FenceCreateInfo(Flags);
		return CreateFence(FenceCreateInfo);
	}

	void DestroyHandle() override
	{
		this->_Device->destroyFence(this->_Handle);
	}
};

template <>
class TVulkanWrapper <vk::Framebuffer> : public TVulkanWrapperBase<vk::Framebuffer>
{
public:
	using Base = TVulkanWrapperBase<vk::Framebuffer>;
	using Base::Base;

	TVulkanWrapper(const vk::Device& Device, const vk::FramebufferCreateInfo& CreateInfo)
		: Base(Device)
	{
		CreateFramebuffer(CreateInfo);
	}

private:
	vk::Result CreateFramebuffer(const vk::FramebufferCreateInfo& CreateInfo)
	{
		try
		{
			this->_Handle = this->_Device->createFramebuffer(CreateInfo);
		}
		catch (const vk::SystemError& Error)
		{
			NpgsCoreError("Failed to create framebuffer: {}.", Error.what());
			return static_cast<vk::Result>(Error.code().value());
		}

		NpgsCoreInfo("Framebuffer created successfully.");
		return vk::Result::eSuccess;
	}

	void DestroyHandle() override
	{
		this->_Device->destroyFramebuffer(this->_Handle);
	}
};

template <>
class TVulkanWrapper<vk::Pipeline> : public TVulkanWrapperBase<vk::Pipeline>
{
public:
	using Base = TVulkanWrapperBase<vk::Pipeline>;
	using Base::Base;

	TVulkanWrapper(const vk::Device& Device, const vk::GraphicsPipelineCreateInfo& CreateInfo)
		: Base(Device)
	{
		CreateGraphicsPipeline(CreateInfo);
	}

	TVulkanWrapper(const vk::Device& Device, const vk::ComputePipelineCreateInfo& CreateInfo)
		: Base(Device)
	{
		CreateComputePipeline(CreateInfo);
	}

private:
	vk::Result CreateGraphicsPipeline(const vk::GraphicsPipelineCreateInfo& CreateInfo)
	{
		try
		{
			this->_Handle = this->_Device->createGraphicsPipeline(vk::PipelineCache(), CreateInfo).value;
		}
		catch (const vk::SystemError& Error)
		{
			NpgsCoreError("Failed to create graphics pipeline: {}", Error.what());
			return static_cast<vk::Result>(Error.code().value());
		}

		NpgsCoreInfo("Graphics pipeline created successfully.");
		return vk::Result::eSuccess;
	}

	vk::Result CreateComputePipeline(const vk::ComputePipelineCreateInfo& CreateInfo)
	{
		try
		{
			this->_Handle = this->_Device->createComputePipeline(vk::PipelineCache(), CreateInfo).value;
		}
		catch (const vk::SystemError& Error)
		{
			NpgsCoreError("Failed to create compute pipeline: {}", Error.what());
			return static_cast<vk::Result>(Error.code().value());
		}

		NpgsCoreInfo("Compute pipeline created successfully.");
		return vk::Result::eSuccess;
	}

	void DestroyHandle() override
	{
		this->_Device->destroyPipeline(this->_Handle);
	}
};

template <>
class TVulkanWrapper<vk::PipelineLayout> : public TVulkanWrapperBase<vk::PipelineLayout>
{
public:
	using Base = TVulkanWrapperBase<vk::PipelineLayout>;
	using Base::Base;

	TVulkanWrapper(const vk::Device& Device, const vk::PipelineLayoutCreateInfo& CreateInfo)
		: Base(Device)
	{
		CreatePipelineLayout(CreateInfo);
	}

private:
	vk::Result CreatePipelineLayout(const vk::PipelineLayoutCreateInfo& CreateInfo)
	{
		try
		{
			this->_Handle = this->_Device->createPipelineLayout(CreateInfo);
			return vk::Result::eSuccess;
		}
		catch (const vk::SystemError& Error)
		{
			NpgsCoreError("Failed to create pipeline layout: {}", Error.what());
			return static_cast<vk::Result>(Error.code().value());
		}

		NpgsCoreInfo("Pipeline layout created successfully");
		return vk::Result::eSuccess;
	}

	void DestroyHandle() override
	{
		this->_Device->destroyPipelineLayout(this->_Handle);
	}
};

template <>
class TVulkanWrapper<vk::RenderPass> : public TVulkanWrapperBase<vk::RenderPass>
{
public:
	using Base = TVulkanWrapperBase<vk::RenderPass>;
	using Base::Base;

	TVulkanWrapper(const vk::Device& Device, const vk::RenderPassCreateInfo& CreateInfo)
		: Base(Device)
	{
		CreateRenderPass(CreateInfo);
	}

	void CommandBegin(const TVulkanWrapper<vk::CommandBuffer>& CommandBuffer, const vk::RenderPassBeginInfo& BeginInfo,
					  const vk::SubpassContents& SubpassContents) const
	{
		CommandBuffer->beginRenderPass(BeginInfo, SubpassContents);
	}

	void CommandBegin(const TVulkanWrapper<vk::CommandBuffer>& CommandBuffer, const FVulkanFramebuffer& Framebuffer,
					  const vk::Rect2D& RenderArea, const std::vector<vk::ClearValue>& ClearValues,
					  const vk::SubpassContents& SubpassContents) const
	{
		vk::RenderPassBeginInfo RenderPassBeginInfo(this->_Handle, Framebuffer.GetFramebuffer(), RenderArea, ClearValues);
		CommandBegin(CommandBuffer, RenderPassBeginInfo, SubpassContents);
	}

	void CommandNext(const TVulkanWrapper<vk::CommandBuffer>& CommandBuffer,
					 const vk::SubpassContents& SubpassContents) const
	{
		CommandBuffer->nextSubpass(SubpassContents);
	}

	void CommandEnd(const TVulkanWrapper<vk::CommandBuffer>& CommandBuffer) const
	{
		CommandBuffer->endRenderPass();
	}

private:
	vk::Result CreateRenderPass(const vk::RenderPassCreateInfo& CreateInfo)
	{
		try
		{
			this->_Handle = this->_Device->createRenderPass(CreateInfo);
		}
		catch (const vk::SystemError& Error)
		{
			NpgsCoreError("Failed to create render pass: {}.", Error.what());
			return static_cast<vk::Result>(Error.code().value());
		}
		NpgsCoreInfo("Render pass created successfully.");
		return vk::Result::eSuccess;
	}

	void DestroyHandle() override
	{
		this->_Device->destroyRenderPass(this->_Handle);
	}
};

template <>
class TVulkanWrapper<vk::Semaphore> : public TVulkanWrapperBase<vk::Semaphore>
{
public:
	using Base = TVulkanWrapperBase<vk::Semaphore>;
	using Base::Base;

	TVulkanWrapper(const vk::Device& Device, const vk::SemaphoreCreateInfo& CreateInfo)
		: Base(Device)
	{
		CreateSemaphore(CreateInfo);
	}

	TVulkanWrapper(const vk::Device& Device, const vk::SemaphoreCreateFlags& Flags = {})
		: Base(Device)
	{
		CreateSemaphore(Flags);
	}

private:
	vk::Result CreateSemaphore(const vk::SemaphoreCreateInfo& CreateInfo)
	{
		try
		{
			this->_Handle = this->_Device->createSemaphore(CreateInfo);
		}
		catch (const vk::SystemError& Error)
		{
			NpgsCoreError("Failed to create semaphore: {}", Error.what());
			return static_cast<vk::Result>(Error.code().value());
		}

		NpgsCoreInfo("Semaphore created successfully.");
		return vk::Result::eSuccess;
	}

	vk::Result CreateSemaphore(const vk::SemaphoreCreateFlags& Flags)
	{
		vk::SemaphoreCreateInfo SemaphoreCreateInfo(Flags);
		return CreateSemaphore(SemaphoreCreateInfo);
	}

	void DestroyHandle() override
	{
		this->_Device->destroySemaphore(this->_Handle);
	}
};

template <>
class TVulkanWrapper<vk::ShaderModule> : public TVulkanWrapperBase<vk::ShaderModule>
{
public:
	using Base = TVulkanWrapperBase<vk::ShaderModule>;
	using Base::Base;

	TVulkanWrapper(const vk::Device& Device, const vk::ShaderModuleCreateInfo& CreateInfo)
		: Base(Device)
	{
		CreateShaderModule(CreateInfo);
	}

	// Temp
	TVulkanWrapper(const vk::Device& Device, const std::string& Filename)
		: Base(Device)
	{
		CreateShaderModule(Filename);
	}

private:
	vk::Result CreateShaderModule(const vk::ShaderModuleCreateInfo& CreateInfo)
	{
		try
		{
			this->_Handle = this->_Device->createShaderModule(CreateInfo);
		}
		catch (const vk::SystemError& Error)
		{
			NpgsCoreError("Failed to create shader module: {}", Error.what());
			return static_cast<vk::Result>(Error.code().value());
		}

		NpgsCoreInfo("Shader module created successfully.");
		return vk::Result::eSuccess;
	}

	vk::Result CreateShaderModule(const std::string& Filename)
	{
		std::ifstream ShaderFile(Filename, std::ios::ate | std::ios::binary);
		if (!ShaderFile.is_open())
		{
			NpgsCoreError("Failed to open shader file: {}", Filename);
			return vk::Result::eErrorInitializationFailed;
		}

		std::size_t FileSize = static_cast<std::size_t>(ShaderFile.tellg());
		std::vector<std::uint32_t> ShaderCode(FileSize / sizeof(std::uint32_t));
		ShaderFile.seekg(0);
		ShaderFile.read(reinterpret_cast<char*>(ShaderCode.data()), FileSize);
		ShaderFile.close();

		vk::ShaderModuleCreateInfo ShaderModuleCreateInfo = vk::ShaderModuleCreateInfo()
			.setCodeSize(FileSize)
			.setPCode(ShaderCode.data());
		return CreateShaderModule(ShaderModuleCreateInfo);
	}

	void DestroyHandle() override
	{
		this->_Device->destroyShaderModule(this->_Handle);
	}
};

_NPGS_END
