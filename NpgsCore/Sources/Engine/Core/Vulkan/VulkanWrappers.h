#pragma once

#include <cstdint>
#include <vector>
#include <vulkan/vulkan.hpp>
#include "Engine/Core/Base.h"

_NPGS_BEGIN

struct FGraphicsPipelineCreateInfoPack
{
	vk::GraphicsPipelineCreateInfo                     GraphicsPipelineCreateInfo;
	vk::PipelineDepthStencilStateCreateInfo            DepthStencilStateCreateInfo;
	vk::PipelineRasterizationStateCreateInfo           RasterizationStateCreateInfo;
	vk::PipelineColorBlendStateCreateInfo              ColorBlendStateCreateInfo;
	vk::PipelineVertexInputStateCreateInfo             VertexInputStateCreateInfo;
	vk::PipelineViewportStateCreateInfo                ViewportStateCreateInfo;
	vk::PipelineMultisampleStateCreateInfo             MultisampleStateCreateInfo;
	vk::PipelineInputAssemblyStateCreateInfo           InputAssemblyStateCreateInfo;
	vk::PipelineDynamicStateCreateInfo                 DynamicStateCreateInfo;
	vk::PipelineTessellationStateCreateInfo            TessellationStateCreateInfo;

	std::vector<vk::PipelineShaderStageCreateInfo>     ShaderStages;
	std::vector<vk::VertexInputBindingDescription>     VertexInputBindings;
	std::vector<vk::VertexInputAttributeDescription>   VertexInputAttributes;
	std::vector<vk::Viewport>                          Viewports;
	std::vector<vk::Rect2D>                            Scissors;
	std::vector<vk::PipelineColorBlendAttachmentState> ColorBlendAttachmentStates;
	std::vector<vk::DynamicState>                      DynamicStates;

	std::uint32_t                                      DynamicViewportCount = 1;
	std::uint32_t                                      DynamicScissorCount = 1;

public:
	FGraphicsPipelineCreateInfoPack();
	FGraphicsPipelineCreateInfoPack(const FGraphicsPipelineCreateInfoPack&) = default;
	FGraphicsPipelineCreateInfoPack(FGraphicsPipelineCreateInfoPack&& Other) noexcept;
	~FGraphicsPipelineCreateInfoPack() = default;

	FGraphicsPipelineCreateInfoPack& operator=(const FGraphicsPipelineCreateInfoPack&) = default;
	FGraphicsPipelineCreateInfoPack& operator=(FGraphicsPipelineCreateInfoPack&& Other) noexcept;

	operator vk::GraphicsPipelineCreateInfo& ();

	void Update();

private:
	void LinkToGraphicsPipelineCreateInfo();
	void UpdateAllInfoData();
};

// Wrapper for vk::CommandBuffer
// -----------------------------
class FVulkanPipeline;
class FVulkanCommandBuffer
{
public:
	FVulkanCommandBuffer() = default;
	FVulkanCommandBuffer(const FVulkanCommandBuffer&) = default;
	FVulkanCommandBuffer(FVulkanCommandBuffer&& Other) noexcept;
	~FVulkanCommandBuffer() = default;

	FVulkanCommandBuffer& operator=(const FVulkanCommandBuffer&) = default;
	FVulkanCommandBuffer& operator=(FVulkanCommandBuffer&& Other) noexcept;

	vk::CommandBuffer* operator->();
	const vk::CommandBuffer* operator->() const;
	vk::CommandBuffer& operator*();
	const vk::CommandBuffer& operator*() const;
	explicit operator bool() const;

	vk::Result Begin(const vk::CommandBufferInheritanceInfo& InheritanceInfo, const vk::CommandBufferUsageFlags& Flags = {});
	vk::Result Begin(const vk::CommandBufferUsageFlags& Flags = {});
	vk::Result End();

private:
	vk::CommandBuffer _Handle;
};

// Wrapper for vk::CommandPool
// ---------------------------
class FVulkanCommandPool
{
public:
	FVulkanCommandPool();
	FVulkanCommandPool(const vk::Device& Device, vk::CommandPoolCreateInfo& CreateInfo);
	FVulkanCommandPool(const vk::Device& Device, std::uint32_t QueueFamilyIndex, vk::CommandPoolCreateFlags Flags = {});
	FVulkanCommandPool(const FVulkanCommandPool&) = default;
	FVulkanCommandPool(FVulkanCommandPool&& Other) noexcept;
	~FVulkanCommandPool();

	FVulkanCommandPool& operator=(const FVulkanCommandPool&) = default;
	FVulkanCommandPool& operator=(FVulkanCommandPool&& Other) noexcept;

	vk::CommandPool* operator->();
	const vk::CommandPool* operator->() const;
	vk::CommandPool& operator*();
	const vk::CommandPool& operator*() const;
	explicit operator bool() const;

	vk::Result AllocateBuffer(vk::CommandBufferLevel Level, vk::CommandBuffer& Buffer);
	vk::Result AllocateBuffer(vk::CommandBufferLevel Level, FVulkanCommandBuffer& Buffer);
	vk::Result AllocateBuffers(vk::CommandBufferLevel Level, std::vector<vk::CommandBuffer>& Buffers);
	vk::Result AllocateBuffers(vk::CommandBufferLevel Level, std::vector<FVulkanCommandBuffer>& Buffers);
	vk::Result FreeBuffer(vk::CommandBufferLevel Level, vk::CommandBuffer& Buffer);
	vk::Result FreeBuffer(vk::CommandBufferLevel Level, FVulkanCommandBuffer& Buffer);
	vk::Result FreeBuffers(vk::CommandBufferLevel Level, std::vector<vk::CommandBuffer>& Buffers);
	vk::Result FreeBuffers(vk::CommandBufferLevel Level, std::vector<FVulkanCommandBuffer>& Buffers);

private:
	vk::Result CreateCommandPool(vk::CommandPoolCreateInfo& CreateInfo);
	vk::Result CreateCommandPool(std::uint32_t QueueFamilyIndex, vk::CommandPoolCreateFlags Flags);

private:
	const vk::Device* _Device;
	vk::CommandPool   _Handle;
};

// Wrapper for vk::Fence
// ---------------------
class FVulkanFence
{
public:
	FVulkanFence();
	FVulkanFence(const vk::Device& Device, const vk::FenceCreateInfo& CreateInfo);
	FVulkanFence(const vk::Device& Device, const vk::FenceCreateFlags& Flags = {});
	FVulkanFence(const FVulkanFence&) = default;
	FVulkanFence(FVulkanFence&& Other) noexcept;
	~FVulkanFence();

	FVulkanFence& operator=(const FVulkanFence&) = default;
	FVulkanFence& operator=(FVulkanFence&& Other) noexcept;

	vk::Fence* operator->();
	const vk::Fence* operator->() const;
	vk::Fence& operator*();
	const vk::Fence& operator*() const;
	explicit operator bool() const;

	vk::Result Wait() const;
	vk::Result Reset() const;
	vk::Result WaitAndReset() const;
	vk::Result GetStatus() const;

private:
	vk::Result CreateFence(const vk::FenceCreateInfo& CreateInfo);
	vk::Result CreateFence(const vk::FenceCreateFlags& Flags);

private:
	const vk::Device* _Device;
	vk::Fence         _Handle;
};

// Wrapper for vk::Framebuffer
// ---------------------------
class FVulkanFramebuffer
{
public:
	FVulkanFramebuffer();
	FVulkanFramebuffer(const vk::Device& Device, const vk::FramebufferCreateInfo& CreateInfo);
	FVulkanFramebuffer(const FVulkanFramebuffer&) = default;
	FVulkanFramebuffer(FVulkanFramebuffer&& Other) noexcept;
	~FVulkanFramebuffer();

	FVulkanFramebuffer& operator=(const FVulkanFramebuffer&) = default;
	FVulkanFramebuffer& operator=(FVulkanFramebuffer&& Other) noexcept;

	vk::Framebuffer* operator->();
	const vk::Framebuffer* operator->() const;
	vk::Framebuffer& operator*();
	const vk::Framebuffer& operator*() const;
	explicit operator bool() const;

private:
	vk::Result CreateFramebuffer(const vk::FramebufferCreateInfo& CreateInfo);

private:
	const vk::Device* _Device;
	vk::Framebuffer   _Handle;
};

// Wrapper for vk::Pipeline
// ------------------------
class FVulkanPipeline
{
public:
	FVulkanPipeline();
	FVulkanPipeline(const vk::Device& Device, const vk::GraphicsPipelineCreateInfo& CreateInfo);
	FVulkanPipeline(const vk::Device& Device, const vk::ComputePipelineCreateInfo& CreateInfo);
	FVulkanPipeline(const FVulkanPipeline&) = default;
	FVulkanPipeline(FVulkanPipeline&& Other) noexcept;
	~FVulkanPipeline();

	FVulkanPipeline& operator=(const FVulkanPipeline&) = default;
	FVulkanPipeline& operator=(FVulkanPipeline&& Other) noexcept;

	vk::Pipeline* operator->();
	const vk::Pipeline* operator->() const;
	vk::Pipeline& operator*();
	const vk::Pipeline& operator*() const;
	explicit operator bool() const;

private:
	vk::Result CreateGraphicsPipeline(const vk::GraphicsPipelineCreateInfo& CreateInfo);
	vk::Result CreateComputePipeline(const vk::ComputePipelineCreateInfo& CreateInfo);

private:
	const vk::Device* _Device;
	vk::Pipeline      _Handle;
};

// Wrapper for vk::PipelineLayout
// ------------------------------
class FVulkanPipelineLayout
{
public:
	FVulkanPipelineLayout();
	FVulkanPipelineLayout(const vk::Device& Device, const vk::PipelineLayoutCreateInfo& CreateInfo);
	FVulkanPipelineLayout(const FVulkanPipelineLayout&) = default;
	FVulkanPipelineLayout(FVulkanPipelineLayout& Other) noexcept;
	~FVulkanPipelineLayout();

	FVulkanPipelineLayout& operator=(const FVulkanPipelineLayout&) = default;
	FVulkanPipelineLayout& operator=(FVulkanPipelineLayout&& Other) noexcept;

	vk::PipelineLayout* operator->();
	const vk::PipelineLayout* operator->() const;
	vk::PipelineLayout& operator*();
	const vk::PipelineLayout& operator*() const;
	explicit operator bool() const;

private:
	vk::Result CreatePipelineLayout(const vk::PipelineLayoutCreateInfo& CreateInfo);

private:
	const vk::Device*  _Device;
	vk::PipelineLayout _Handle;
};

// Wrapper for vk::RenderPass
// --------------------------
class FVulkanRenderPass
{
public:
	FVulkanRenderPass();
	FVulkanRenderPass(const vk::Device& Device, const vk::RenderPassCreateInfo& CreateInfo);
	FVulkanRenderPass(const FVulkanRenderPass&) = default;
	FVulkanRenderPass(FVulkanRenderPass&& Other) noexcept;
	~FVulkanRenderPass();

	FVulkanRenderPass& operator=(const FVulkanRenderPass&) = default;
	FVulkanRenderPass& operator=(FVulkanRenderPass&& Other) noexcept;

	vk::RenderPass* operator->();
	const vk::RenderPass* operator->() const;
	vk::RenderPass& operator*();
	const vk::RenderPass& operator*() const;
	explicit operator bool() const;

	void CommandBegin(const FVulkanCommandBuffer& CommandBuffer, const vk::RenderPassBeginInfo& BeginInfo,
					  const vk::SubpassContents& SubpassContents = vk::SubpassContents::eInline) const;

	void CommandBegin(const FVulkanCommandBuffer& CommandBuffer, const FVulkanFramebuffer& Framebuffer,
					  const vk::Rect2D& RenderArea, const std::vector<vk::ClearValue>& ClearValues,
					  const vk::SubpassContents& SubpassContents = vk::SubpassContents::eInline) const;

	void CommandNext(const FVulkanCommandBuffer& CommandBuffer,
					 const vk::SubpassContents& SubpassContents = vk::SubpassContents::eInline) const;

	void CommandEnd(const FVulkanCommandBuffer& CommandBuffer) const;

private:
	vk::Result CreateRenderPass(const vk::RenderPassCreateInfo& CreateInfo);

private:
	const vk::Device* _Device;
	vk::RenderPass    _Handle;
};

// Wrapper for vk::Semaphore
// -------------------------
class FVulkanSemaphore
{
public:
	FVulkanSemaphore();
	FVulkanSemaphore(const vk::Device& Device, const vk::SemaphoreCreateInfo& CreateInfo);
	FVulkanSemaphore(const vk::Device& Device, const vk::SemaphoreCreateFlags& Flags = {});
	FVulkanSemaphore(const FVulkanSemaphore&) = default;
	FVulkanSemaphore(FVulkanSemaphore&& Other) noexcept;
	~FVulkanSemaphore();

	FVulkanSemaphore& operator=(const FVulkanSemaphore&) = default;
	FVulkanSemaphore& operator=(FVulkanSemaphore&& Other) noexcept;

	vk::Semaphore* operator->();
	const vk::Semaphore* operator->() const;
	vk::Semaphore& operator*();
	const vk::Semaphore& operator*() const;
	explicit operator bool() const;

private:
	vk::Result CreateSemaphore(const vk::SemaphoreCreateInfo& CreateInfo);
	vk::Result CreateSemaphore(const vk::SemaphoreCreateFlags& Flags);

private:
	const vk::Device* _Device;
	vk::Semaphore     _Handle;
};

// Wrapper for vk::ShaderModule
// ----------------------------
class FVulkanShaderModule
{
public:
	FVulkanShaderModule();
	FVulkanShaderModule(const vk::Device& Device, const vk::ShaderModuleCreateInfo& CreateInfo);
	FVulkanShaderModule(const vk::Device& Device, const std::string& Filename);
	FVulkanShaderModule(const FVulkanShaderModule&) = default;
	FVulkanShaderModule(FVulkanShaderModule& Other) noexcept;
	~FVulkanShaderModule();

	FVulkanShaderModule& operator=(const FVulkanShaderModule&) = default;
	FVulkanShaderModule& operator=(FVulkanShaderModule&& Other) noexcept;

	vk::ShaderModule* operator->();
	const vk::ShaderModule* operator->() const;
	vk::ShaderModule& operator*();
	const vk::ShaderModule& operator*() const;
	explicit operator bool() const;

private:
	vk::Result CreateShaderModule(const vk::ShaderModuleCreateInfo& CreateInfo);
	vk::Result CreateShaderModule(const std::string& Filename);

private:
	const vk::Device* _Device;
	vk::ShaderModule  _Handle;
};

_NPGS_END

#include "VulkanWrappers.inl"
