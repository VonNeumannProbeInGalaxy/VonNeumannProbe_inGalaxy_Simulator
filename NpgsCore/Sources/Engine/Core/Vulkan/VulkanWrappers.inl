#pragma once

#include "VulkanWrappers.h"

_NPGS_BEGIN

NPGS_INLINE FGraphicsPipelineCreateInfoPack::operator vk::GraphicsPipelineCreateInfo& ()
{
	return GraphicsPipelineCreateInfo;
}

// Wrapper for vk::CommandBuffer
// -----------------------------
NPGS_INLINE FVulkanCommandBuffer::operator bool() const
{
	return static_cast<bool>(_CommandBuffer);
}

NPGS_INLINE void FVulkanCommandBuffer::BindPipeline(vk::PipelineBindPoint PipelineBindPoint, const FVulkanPipeline& Pipeline)
{
	_CommandBuffer.bindPipeline(PipelineBindPoint, Pipeline.GetPipeline());
}

NPGS_INLINE void FVulkanCommandBuffer::Draw(std::uint32_t VertexCount, std::uint32_t InstanceCount,
											std::uint32_t FirstVertex, std::uint32_t FirstInstance)
{
	_CommandBuffer.draw(VertexCount, InstanceCount, FirstVertex, FirstInstance);
}

NPGS_INLINE vk::CommandBuffer& FVulkanCommandBuffer::GetCommandBufferMutable()
{
	return _CommandBuffer;
}

NPGS_INLINE const vk::CommandBuffer& FVulkanCommandBuffer::GetCommandBuffer() const
{
	return _CommandBuffer;
}

// Wrapper for vk::CommandPool
// ---------------------------
NPGS_INLINE FVulkanCommandPool::operator bool() const
{
	return static_cast<bool>(_CommandPool);
}

NPGS_INLINE vk::CommandPool& FVulkanCommandPool::GetCommandPoolMutable()
{
	return _CommandPool;
}

NPGS_INLINE const vk::CommandPool& FVulkanCommandPool::GetCommandPool() const
{
	return _CommandPool;
}

// Wrapper for vk::Fence
// ---------------------
NPGS_INLINE FVulkanFence::operator bool() const
{
	return static_cast<bool>(_Fence);
}

NPGS_INLINE vk::Fence& FVulkanFence::GetFenceMutable()
{
	return _Fence;
}

NPGS_INLINE const vk::Fence& FVulkanFence::GetFence() const
{
	return _Fence;
}

// Wrapper for vk::Framebuffer
// ---------------------------
NPGS_INLINE FVulkanFramebuffer::operator bool() const
{
	return static_cast<bool>(_Framebuffer);
}

NPGS_INLINE vk::Framebuffer& FVulkanFramebuffer::GetFramebufferMutable()
{
	return _Framebuffer;
}

NPGS_INLINE const vk::Framebuffer& FVulkanFramebuffer::GetFramebuffer() const
{
	return _Framebuffer;
}

// Wrapper for vk::Pipeline
// ------------------------
NPGS_INLINE FVulkanPipeline::operator bool() const
{
	return static_cast<bool>(_Pipeline);
}

NPGS_INLINE vk::Pipeline& FVulkanPipeline::GetPipelineMutable()
{
	return _Pipeline;
}

NPGS_INLINE const vk::Pipeline& FVulkanPipeline::GetPipeline() const
{
	return _Pipeline;
}

// Wrapper for vk::PipelineLayout
// ------------------------------
NPGS_INLINE FVulkanPipelineLayout::operator bool() const
{
	return static_cast<bool>(_PipelineLayout);
}

NPGS_INLINE vk::PipelineLayout& FVulkanPipelineLayout::GetPipelineLayoutMutable()
{
	return _PipelineLayout;
}

NPGS_INLINE const vk::PipelineLayout& FVulkanPipelineLayout::GetPipelineLayout() const
{
	return _PipelineLayout;
}

// Wrapper for vk::RenderPass
// --------------------------
NPGS_INLINE void FVulkanRenderPass::CommandBegin(const FVulkanCommandBuffer& CommandBuffer,
												 const vk::RenderPassBeginInfo& BeginInfo,
												 const vk::SubpassContents& SubpassContents) const
{
	CommandBuffer.GetCommandBuffer().beginRenderPass(BeginInfo, SubpassContents);
}

NPGS_INLINE void FVulkanRenderPass::CommandNext(const FVulkanCommandBuffer& CommandBuffer,
												const vk::SubpassContents& SubpassContents) const
{
	CommandBuffer.GetCommandBuffer().nextSubpass(SubpassContents);
}

NPGS_INLINE void FVulkanRenderPass::CommandEnd(const FVulkanCommandBuffer& CommandBuffer) const
{
	CommandBuffer.GetCommandBuffer().endRenderPass();
}

NPGS_INLINE FVulkanRenderPass::operator bool() const
{
	return static_cast<bool>(_RenderPass);
}

NPGS_INLINE vk::RenderPass& FVulkanRenderPass::GetRenderPassMutable()
{
	return _RenderPass;
}

NPGS_INLINE const vk::RenderPass& FVulkanRenderPass::GetRenderPass() const
{
	return _RenderPass;
}

// Wrapper for vk::Semaphore
// -------------------------
NPGS_INLINE FVulkanSemaphore::operator bool() const
{
	return static_cast<bool>(_Semaphore);
}

NPGS_INLINE vk::Semaphore& FVulkanSemaphore::GetSemaphoreMutable()
{
	return _Semaphore;
}

NPGS_INLINE const vk::Semaphore& FVulkanSemaphore::GetSemaphore() const
{
	return _Semaphore;
}

// Wrapper for vk::ShaderModule
// ----------------------------
NPGS_INLINE FVulkanShaderModule::operator bool() const
{
	return static_cast<bool>(_ShaderModule);
}

NPGS_INLINE vk::ShaderModule& FVulkanShaderModule::GetShaderModuleMutable()
{
	return _ShaderModule;
}

NPGS_INLINE const vk::ShaderModule& FVulkanShaderModule::GetShaderModule() const
{
	return _ShaderModule;
}

_NPGS_END
