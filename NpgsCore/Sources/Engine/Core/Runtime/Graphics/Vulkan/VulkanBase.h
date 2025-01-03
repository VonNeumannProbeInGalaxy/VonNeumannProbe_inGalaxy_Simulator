#pragma once

#include <memory>

#include "Engine/Core/Vulkan/VulkanCore.h"
#include "Engine/Core/Vulkan/VulkanWrappers.h"
#include "Engine/Core/Base/Base.h"

_NPGS_BEGIN

class FVulkanBase
{
public:
	vk::Result ExecuteGraphicsCommand(const FVulkanCommandBuffer& CommandBuffer) const;
	vk::Result SubmitCommandBufferToGraphics(const vk::SubmitInfo& SubmitInfo, const FVulkanFence& Fence = {}) const;
	vk::Result SubmitCommandBufferToGraphics(const FVulkanCommandBuffer& Buffer, const FVulkanFence& Fence = {}) const;

	vk::Result SubmitCommandBufferToGraphics(const FVulkanCommandBuffer& Buffer,
											 const FVulkanSemaphore& WaitSemaphore = {},
											 const FVulkanSemaphore& SignalSemaphore = {},
											 const FVulkanFence& Fence = {},
											 vk::PipelineStageFlags Flags = vk::PipelineStageFlagBits::eColorAttachmentOutput) const;

	vk::Result SubmitCommandBufferToPresent(const FVulkanCommandBuffer& Buffer,
											const FVulkanSemaphore& WaitSemaphore = {},
											const FVulkanSemaphore& SignalSemaphore = {},
											const FVulkanFence& Fence = {}) const;

	vk::Result SubmitCommandBufferToCompute(const vk::SubmitInfo& SubmitInfo, const FVulkanFence& Fence = {}) const;
	vk::Result SubmitCommandBufferToCompute(const FVulkanCommandBuffer& Buffer, const FVulkanFence& Fence = {}) const;

	vk::Result SubmitCommandBufferToCompute(const FVulkanCommandBuffer& Buffer,
											const FVulkanSemaphore& WaitSemaphore = {},
											const FVulkanSemaphore& SignalSemaphore = {},
											const FVulkanFence& Fence = {},
											vk::PipelineStageFlags Flags = vk::PipelineStageFlagBits::eComputeShader) const;

	vk::Result TransferImageOwnershipToPresent(const FVulkanCommandBuffer& PresentCommandBuffer,
											   const FVulkanSemaphore& WaitSemaphore,
											   const FVulkanSemaphore& SignalSemaphore,
											   const FVulkanFence& Fence = {}) const;

	const FVulkanCommandBuffer& GetTransferCommandBuffer() const;
	const FVulkanCommandBuffer& GetPresentCommandBuffer() const;
	const FVulkanCommandPool& GetGraphicsCommandPool() const;
	const FVulkanCommandPool& GetPresentCommandPool() const;
	const FVulkanCommandPool& GetComputeCommandPool() const;

	FVulkanCore* GetVulkanCore() const;

	static FVulkanBase* GetInstance();

private:
	explicit FVulkanBase();
	FVulkanBase(const FVulkanBase&&) = delete;
	FVulkanBase(FVulkanBase&&)       = delete;
	~FVulkanBase();

	FVulkanBase& operator=(const FVulkanBase&&) = delete;
	FVulkanBase& operator=(FVulkanBase&&)       = delete;

	void TransferImageOwnershipToPresentImpl(const FVulkanCommandBuffer& PresentCommandBuffer) const;

private:
	FVulkanCommandBuffer _TransferCommandBuffer;
	FVulkanCommandBuffer _PresentCommandBuffer;
	FVulkanCommandPool   _GraphicsCommandPool;
	FVulkanCommandPool   _PresentCommandPool;
	FVulkanCommandPool   _ComputeCommandPool;

	std::unique_ptr<FVulkanCore> _VulkanCore;
};

_NPGS_END

#include "VulkanBase.inl"
