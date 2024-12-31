#pragma once

#include "VulkanWrappers.h"

_NPGS_BEGIN

NPGS_INLINE FGraphicsPipelineCreateInfoPack::operator vk::GraphicsPipelineCreateInfo& ()
{
	return GraphicsPipelineCreateInfo;
}

// Wrapper for vk::CommandBuffer
// -----------------------------
NPGS_INLINE vk::CommandBuffer* FVulkanCommandBuffer::operator->()
{
	return &_Handle;
}

NPGS_INLINE const vk::CommandBuffer* FVulkanCommandBuffer::operator->() const
{
	return &_Handle;
}

NPGS_INLINE vk::CommandBuffer& FVulkanCommandBuffer::operator*()
{
	return _Handle;
}

NPGS_INLINE const vk::CommandBuffer& FVulkanCommandBuffer::operator*() const
{
	return _Handle;
}

NPGS_INLINE FVulkanCommandBuffer::operator bool() const
{
	return static_cast<bool>(_Handle);
}

// Wrapper for vk::CommandPool
// ---------------------------
NPGS_INLINE vk::CommandPool* FVulkanCommandPool::operator->()
{
	return &_Handle;
}

NPGS_INLINE const vk::CommandPool* FVulkanCommandPool::operator->() const
{
	return &_Handle;
}

NPGS_INLINE vk::CommandPool& FVulkanCommandPool::operator*()
{
	return _Handle;
}

NPGS_INLINE const vk::CommandPool& FVulkanCommandPool::operator*() const
{
	return _Handle;
}

NPGS_INLINE FVulkanCommandPool::operator bool() const
{
	return static_cast<bool>(_Handle);
}

// Wrapper for vk::Fence
// ---------------------
NPGS_INLINE vk::Fence* FVulkanFence::operator->()
{
	return &_Handle;
}

NPGS_INLINE const vk::Fence* FVulkanFence::operator->() const
{
	return &_Handle;
}

NPGS_INLINE vk::Fence& FVulkanFence::operator*()
{
	return _Handle;
}

NPGS_INLINE const vk::Fence& FVulkanFence::operator*() const
{
	return _Handle;
}

NPGS_INLINE FVulkanFence::operator bool() const
{
	return static_cast<bool>(_Handle);
}

// Wrapper for vk::Framebuffer
// ---------------------------
NPGS_INLINE vk::Framebuffer* FVulkanFramebuffer::operator->()
{
	return &_Handle;
}

NPGS_INLINE const vk::Framebuffer* FVulkanFramebuffer::operator->() const
{
	return &_Handle;
}

NPGS_INLINE vk::Framebuffer& FVulkanFramebuffer::operator*()
{
	return _Handle;
}

NPGS_INLINE const vk::Framebuffer& FVulkanFramebuffer::operator*() const
{
	return _Handle;
}

NPGS_INLINE FVulkanFramebuffer::operator bool() const
{
	return static_cast<bool>(_Handle);
}

// Wrapper for vk::Pipeline
// ------------------------
NPGS_INLINE vk::Pipeline* FVulkanPipeline::operator->()
{
	return &_Handle;
}

NPGS_INLINE const vk::Pipeline* FVulkanPipeline::operator->() const
{
	return &_Handle;
}

NPGS_INLINE vk::Pipeline& FVulkanPipeline::operator*()
{
	return _Handle;
}

NPGS_INLINE const vk::Pipeline& FVulkanPipeline::operator*() const
{
	return _Handle;
}

NPGS_INLINE FVulkanPipeline::operator bool() const
{
	return static_cast<bool>(_Handle);
}

// Wrapper for vk::PipelineLayout
// ------------------------------
NPGS_INLINE vk::PipelineLayout* FVulkanPipelineLayout::operator->()
{
	return &_Handle;
}

NPGS_INLINE const vk::PipelineLayout* FVulkanPipelineLayout::operator->() const
{
	return &_Handle;
}

NPGS_INLINE vk::PipelineLayout& FVulkanPipelineLayout::operator*()
{
	return _Handle;
}

NPGS_INLINE const vk::PipelineLayout& FVulkanPipelineLayout::operator*() const
{
	return _Handle;
}

NPGS_INLINE FVulkanPipelineLayout::operator bool() const
{
	return static_cast<bool>(_Handle);
}

// Wrapper for vk::RenderPass
// --------------------------
NPGS_INLINE void FVulkanRenderPass::CommandBegin(const FVulkanCommandBuffer& CommandBuffer,
												 const vk::RenderPassBeginInfo& BeginInfo,
												 const vk::SubpassContents& SubpassContents) const
{
	CommandBuffer->beginRenderPass(BeginInfo, SubpassContents);
}

NPGS_INLINE void FVulkanRenderPass::CommandNext(const FVulkanCommandBuffer& CommandBuffer,
												const vk::SubpassContents& SubpassContents) const
{
	CommandBuffer->nextSubpass(SubpassContents);
}

NPGS_INLINE void FVulkanRenderPass::CommandEnd(const FVulkanCommandBuffer& CommandBuffer) const
{
	CommandBuffer->endRenderPass();
}

NPGS_INLINE vk::RenderPass* FVulkanRenderPass::operator->()
{
	return &_Handle;
}

NPGS_INLINE const vk::RenderPass* FVulkanRenderPass::operator->() const
{
	return &_Handle;
}

NPGS_INLINE vk::RenderPass& FVulkanRenderPass::operator*()
{
	return _Handle;
}

NPGS_INLINE const vk::RenderPass& FVulkanRenderPass::operator*() const
{
	return _Handle;
}

NPGS_INLINE FVulkanRenderPass::operator bool() const
{
	return static_cast<bool>(_Handle);
}

// Wrapper for vk::Semaphore
// -------------------------
NPGS_INLINE vk::Semaphore* FVulkanSemaphore::operator->()
{
	return &_Handle;
}

NPGS_INLINE const vk::Semaphore* FVulkanSemaphore::operator->() const
{
	return &_Handle;
}

NPGS_INLINE vk::Semaphore& FVulkanSemaphore::operator*()
{
	return _Handle;
}

NPGS_INLINE const vk::Semaphore& FVulkanSemaphore::operator*() const
{
	return _Handle;
}

NPGS_INLINE FVulkanSemaphore::operator bool() const
{
	return static_cast<bool>(_Handle);
}

// Wrapper for vk::ShaderModule
// ----------------------------
NPGS_INLINE vk::ShaderModule* FVulkanShaderModule::operator->()
{
	return &_Handle;
}

NPGS_INLINE const vk::ShaderModule* FVulkanShaderModule::operator->() const
{
	return &_Handle;
}

NPGS_INLINE vk::ShaderModule& FVulkanShaderModule::operator*()
{
	return _Handle;
}

NPGS_INLINE const vk::ShaderModule& FVulkanShaderModule::operator*() const
{
	return _Handle;
}

NPGS_INLINE FVulkanShaderModule::operator bool() const
{
	return static_cast<bool>(_Handle);
}

_NPGS_END
