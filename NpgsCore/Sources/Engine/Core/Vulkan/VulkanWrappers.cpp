#include "VulkanWrappers.h"

#include <limits>
#include <utility>
#include "Engine/Utilities/Logger.h"

_NPGS_BEGIN

FGraphicsPipelineCreateInfoPack::FGraphicsPipelineCreateInfoPack()
{
	LinkToGraphicsPipelineCreateInfo();
	GraphicsPipelineCreateInfo.setBasePipelineIndex(-1);
}

FGraphicsPipelineCreateInfoPack::FGraphicsPipelineCreateInfoPack(FGraphicsPipelineCreateInfoPack&& Other) noexcept
	:
	GraphicsPipelineCreateInfo(std::move(Other.GraphicsPipelineCreateInfo)),
	DepthStencilStateCreateInfo(std::move(Other.DepthStencilStateCreateInfo)),
	RasterizationStateCreateInfo(std::move(Other.RasterizationStateCreateInfo)),
	ColorBlendStateCreateInfo(std::move(Other.ColorBlendStateCreateInfo)),
	VertexInputStateCreateInfo(std::move(Other.VertexInputStateCreateInfo)),
	ViewportStateCreateInfo(std::move(Other.ViewportStateCreateInfo)),
	MultisampleStateCreateInfo(std::move(Other.MultisampleStateCreateInfo)),
	InputAssemblyStateCreateInfo(std::move(Other.InputAssemblyStateCreateInfo)),
	DynamicStateCreateInfo(std::move(Other.DynamicStateCreateInfo)),
	TessellationStateCreateInfo(std::move(Other.TessellationStateCreateInfo)),

	ShaderStages(std::move(Other.ShaderStages)),
	VertexInputBindings(std::move(Other.VertexInputBindings)),
	VertexInputAttributes(std::move(Other.VertexInputAttributes)),
	Viewports(std::move(Other.Viewports)),
	Scissors(std::move(Other.Scissors)),
	ColorBlendAttachmentStates(std::move(Other.ColorBlendAttachmentStates)),
	DynamicStates(std::move(Other.DynamicStates)),

	DynamicViewportCount(Other.DynamicViewportCount),
	DynamicScissorCount(Other.DynamicScissorCount)
{
	Other.GraphicsPipelineCreateInfo   = vk::GraphicsPipelineCreateInfo();
	Other.DepthStencilStateCreateInfo  = vk::PipelineDepthStencilStateCreateInfo();
	Other.RasterizationStateCreateInfo = vk::PipelineRasterizationStateCreateInfo();
	Other.ColorBlendStateCreateInfo    = vk::PipelineColorBlendStateCreateInfo();
	Other.VertexInputStateCreateInfo   = vk::PipelineVertexInputStateCreateInfo();
	Other.ViewportStateCreateInfo      = vk::PipelineViewportStateCreateInfo();
	Other.MultisampleStateCreateInfo   = vk::PipelineMultisampleStateCreateInfo();
	Other.InputAssemblyStateCreateInfo = vk::PipelineInputAssemblyStateCreateInfo();
	Other.DynamicStateCreateInfo       = vk::PipelineDynamicStateCreateInfo();
	Other.TessellationStateCreateInfo  = vk::PipelineTessellationStateCreateInfo();

	Other.DynamicViewportCount         = 1;
	Other.DynamicScissorCount          = 1;

	LinkToGraphicsPipelineCreateInfo();
	UpdateAllInfoData();
}

FGraphicsPipelineCreateInfoPack& FGraphicsPipelineCreateInfoPack::operator=(FGraphicsPipelineCreateInfoPack&& Other) noexcept
{
	if (this != &Other)
	{
		GraphicsPipelineCreateInfo         = std::move(Other.GraphicsPipelineCreateInfo);
		DepthStencilStateCreateInfo        = std::move(Other.DepthStencilStateCreateInfo);
		RasterizationStateCreateInfo       = std::move(Other.RasterizationStateCreateInfo);
		ColorBlendStateCreateInfo          = std::move(Other.ColorBlendStateCreateInfo);
		VertexInputStateCreateInfo         = std::move(Other.VertexInputStateCreateInfo);
		ViewportStateCreateInfo            = std::move(Other.ViewportStateCreateInfo);
		MultisampleStateCreateInfo         = std::move(Other.MultisampleStateCreateInfo);
		InputAssemblyStateCreateInfo       = std::move(Other.InputAssemblyStateCreateInfo);
		DynamicStateCreateInfo             = std::move(Other.DynamicStateCreateInfo);
		TessellationStateCreateInfo        = std::move(Other.TessellationStateCreateInfo);

		ShaderStages                       = std::move(Other.ShaderStages);
		VertexInputBindings                = std::move(Other.VertexInputBindings);
		VertexInputAttributes              = std::move(Other.VertexInputAttributes);
		Viewports                          = std::move(Other.Viewports);
		Scissors                           = std::move(Other.Scissors);
		ColorBlendAttachmentStates         = std::move(Other.ColorBlendAttachmentStates);
		DynamicStates                      = std::move(Other.DynamicStates);

		DynamicViewportCount               = Other.DynamicViewportCount;
		DynamicScissorCount                = Other.DynamicScissorCount;

		Other.GraphicsPipelineCreateInfo   = vk::GraphicsPipelineCreateInfo();
		Other.DepthStencilStateCreateInfo  = vk::PipelineDepthStencilStateCreateInfo();
		Other.RasterizationStateCreateInfo = vk::PipelineRasterizationStateCreateInfo();
		Other.ColorBlendStateCreateInfo    = vk::PipelineColorBlendStateCreateInfo();
		Other.VertexInputStateCreateInfo   = vk::PipelineVertexInputStateCreateInfo();
		Other.ViewportStateCreateInfo      = vk::PipelineViewportStateCreateInfo();
		Other.MultisampleStateCreateInfo   = vk::PipelineMultisampleStateCreateInfo();
		Other.InputAssemblyStateCreateInfo = vk::PipelineInputAssemblyStateCreateInfo();
		Other.DynamicStateCreateInfo       = vk::PipelineDynamicStateCreateInfo();
		Other.TessellationStateCreateInfo  = vk::PipelineTessellationStateCreateInfo();
		Other.DynamicViewportCount         = 1;
		Other.DynamicScissorCount          = 1;

		LinkToGraphicsPipelineCreateInfo();
		UpdateAllInfoData();
	}

	return *this;
}

void FGraphicsPipelineCreateInfoPack::Update()
{
	ViewportStateCreateInfo.setViewportCount(
		Viewports.size() ? static_cast<std::uint32_t>(Viewports.size()) : DynamicViewportCount);
	ViewportStateCreateInfo.setScissorCount(
		Scissors.size() ? static_cast<std::uint32_t>(Scissors.size()) : DynamicScissorCount);

	UpdateAllInfoData();
}

void FGraphicsPipelineCreateInfoPack::LinkToGraphicsPipelineCreateInfo()
{
	GraphicsPipelineCreateInfo
		.setPVertexInputState(&VertexInputStateCreateInfo)
		.setPInputAssemblyState(&InputAssemblyStateCreateInfo)
		.setPTessellationState(&TessellationStateCreateInfo)
		.setPViewportState(&ViewportStateCreateInfo)
		.setPRasterizationState(&RasterizationStateCreateInfo)
		.setPMultisampleState(&MultisampleStateCreateInfo)
		.setPDepthStencilState(&DepthStencilStateCreateInfo)
		.setPColorBlendState(&ColorBlendStateCreateInfo)
		.setPDynamicState(&DynamicStateCreateInfo);
}

void FGraphicsPipelineCreateInfoPack::UpdateAllInfoData()
{
	if (Viewports.empty())
	{
		ViewportStateCreateInfo.setPViewports(nullptr);
	}
	else
	{
		ViewportStateCreateInfo.setViewports(Viewports);
	}

	if (Scissors.empty())
	{
		ViewportStateCreateInfo.setPScissors(nullptr);
	}
	else
	{
		ViewportStateCreateInfo.setScissors(Scissors);
	}

	GraphicsPipelineCreateInfo.setStages(ShaderStages);
	VertexInputStateCreateInfo.setVertexBindingDescriptions(VertexInputBindings);
	VertexInputStateCreateInfo.setVertexAttributeDescriptions(VertexInputAttributes);
	ColorBlendStateCreateInfo.setAttachments(ColorBlendAttachmentStates);
	DynamicStateCreateInfo.setDynamicStates(DynamicStates);
}

// Wrapper for vk::CommandBuffer
// -----------------------------
FVulkanCommandBuffer::FVulkanCommandBuffer(FVulkanCommandBuffer&& Other) noexcept
	: _CommandBuffer(std::move(Other._CommandBuffer))
{
	Other._CommandBuffer = vk::CommandBuffer();
}

FVulkanCommandBuffer& FVulkanCommandBuffer::operator=(FVulkanCommandBuffer&& Other) noexcept
{
	if (this != &Other)
	{
		_CommandBuffer = std::move(Other._CommandBuffer);
		Other._CommandBuffer = vk::CommandBuffer();
	}

	return *this;
}

vk::Result FVulkanCommandBuffer::Begin(const vk::CommandBufferInheritanceInfo& InheritanceInfo,
									   const vk::CommandBufferUsageFlags& Flags)
{
	vk::CommandBufferBeginInfo CommandBufferBeginInfo(Flags, &InheritanceInfo);
	try
	{
		_CommandBuffer.begin(CommandBufferBeginInfo);
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Failed to begin command buffer: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	return vk::Result::eSuccess;
}

vk::Result FVulkanCommandBuffer::Begin(const vk::CommandBufferUsageFlags& Flags)
{
	vk::CommandBufferBeginInfo CommandBufferBeginInfo(Flags);
	try
	{
		_CommandBuffer.begin(CommandBufferBeginInfo);
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Failed to begin command buffer: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	return vk::Result::eSuccess;
}

vk::Result FVulkanCommandBuffer::End()
{
	try
	{
		_CommandBuffer.end();
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Failed to end command buffer: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	return vk::Result::eSuccess;
}

// Wrapper for vk::CommandPool
// ---------------------------
FVulkanCommandPool::FVulkanCommandPool(const vk::Device& Device, vk::CommandPoolCreateInfo& CreateInfo)
	: _Device(&Device)
{
	CreateCommandPool(CreateInfo);
}

FVulkanCommandPool::FVulkanCommandPool(const vk::Device& Device, std::uint32_t QueueFamilyIndex, vk::CommandPoolCreateFlags Flags)
	: _Device(&Device)
{
	CreateCommandPool(QueueFamilyIndex, Flags);
}

FVulkanCommandPool::FVulkanCommandPool(FVulkanCommandPool&& Other) noexcept
	: _Device(Other._Device), _CommandPool(std::move(Other._CommandPool))
{
	Other._Device      = nullptr;
	Other._CommandPool = vk::CommandPool();
}

FVulkanCommandPool::~FVulkanCommandPool()
{
	if (_CommandPool)
	{
		_Device->destroyCommandPool(_CommandPool);
	}
}

FVulkanCommandPool& FVulkanCommandPool::operator=(FVulkanCommandPool&& Other) noexcept
{
	if (this != &Other)
	{
		if (_CommandPool)
		{
			_Device->destroyCommandPool(_CommandPool);
		}
		_Device            = Other._Device;
		_CommandPool       = std::move(Other._CommandPool);
		Other._Device      = nullptr;
		Other._CommandPool = vk::CommandPool();
	}

	return *this;
}

vk::Result FVulkanCommandPool::AllocateBuffer(vk::CommandBufferLevel Level, vk::CommandBuffer& Buffer)
{
	vk::CommandBufferAllocateInfo CommandBufferAllocateInfo(_CommandPool, Level, 1);
	try
	{
		Buffer = _Device->allocateCommandBuffers(CommandBufferAllocateInfo)[0];
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Failed to allocate command buffer: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	NpgsCoreInfo("Command buffer allocated successfully.");
	return vk::Result::eSuccess;
}

vk::Result FVulkanCommandPool::AllocateBuffer(vk::CommandBufferLevel Level, FVulkanCommandBuffer& Buffer)
{
	vk::CommandBufferAllocateInfo CommandBufferAllocateInfo(_CommandPool, Level, 1);
	try
	{
		Buffer.GetCommandBufferMutable() = _Device->allocateCommandBuffers(CommandBufferAllocateInfo)[0];
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Failed to allocate command buffer: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	NpgsCoreInfo("Command buffer allocated successfully.");
	return vk::Result::eSuccess;
}

vk::Result FVulkanCommandPool::AllocateBuffers(vk::CommandBufferLevel Level, std::vector<vk::CommandBuffer>& Buffers)
{
	vk::CommandBufferAllocateInfo CommandBufferAllocateInfo(_CommandPool, Level, static_cast<std::uint32_t>(Buffers.size()));
	try
	{
		Buffers = _Device->allocateCommandBuffers(CommandBufferAllocateInfo);
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Failed to allocate command buffers: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	NpgsCoreInfo("Command buffers allocated successfully.");
	return vk::Result::eSuccess;
}

vk::Result FVulkanCommandPool::AllocateBuffers(vk::CommandBufferLevel Level, std::vector<FVulkanCommandBuffer>& Buffers)
{
	vk::CommandBufferAllocateInfo CommandBufferAllocateInfo(_CommandPool, Level, static_cast<std::uint32_t>(Buffers.size()));
	std::vector<vk::CommandBuffer> CommandBuffers;
	try
	{
		CommandBuffers = _Device->allocateCommandBuffers(CommandBufferAllocateInfo);
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Failed to allocate command buffers: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	Buffers.resize(CommandBuffers.size());
	for (std::size_t i = 0; i != CommandBuffers.size(); ++i)
	{
		Buffers[i].GetCommandBufferMutable() = CommandBuffers[i];
	}

	NpgsCoreInfo("Command buffers allocated successfully.");
	return vk::Result::eSuccess;
}

vk::Result FVulkanCommandPool::FreeBuffer(vk::CommandBufferLevel Level, vk::CommandBuffer& Buffer)
{
	_Device->freeCommandBuffers(_CommandPool, Buffer);
	Buffer = vk::CommandBuffer();
	NpgsCoreInfo("Command buffer freed successfully.");
	return vk::Result::eSuccess;
}

vk::Result FVulkanCommandPool::FreeBuffer(vk::CommandBufferLevel Level, FVulkanCommandBuffer& Buffer)
{
	return FreeBuffer(Level, Buffer.GetCommandBufferMutable());
}

vk::Result FVulkanCommandPool::FreeBuffers(vk::CommandBufferLevel Level, std::vector<vk::CommandBuffer>& Buffers)
{
	_Device->freeCommandBuffers(_CommandPool, Buffers);
	Buffers.clear();
	NpgsCoreInfo("Command buffers freed successfully.");
	return vk::Result::eSuccess;
}

vk::Result FVulkanCommandPool::FreeBuffers(vk::CommandBufferLevel Level, std::vector<FVulkanCommandBuffer>& Buffers)
{
	std::vector<vk::CommandBuffer> CommandBuffers;
	CommandBuffers.reserve(Buffers.size());

	for (auto& Buffer : Buffers)
	{
		CommandBuffers.emplace_back(Buffer.GetCommandBuffer());
	}

	_Device->freeCommandBuffers(_CommandPool, CommandBuffers);
	Buffers.clear();

	NpgsCoreInfo("Command buffers freed successfully.");
	return vk::Result::eSuccess;
}

vk::Result FVulkanCommandPool::CreateCommandPool(vk::CommandPoolCreateInfo& CreateInfo)
{
	try
	{
		_CommandPool = _Device->createCommandPool(CreateInfo);
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Failed to create command pool: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	NpgsCoreInfo("Command pool created successfully.");
	return vk::Result::eSuccess;
}

vk::Result FVulkanCommandPool::CreateCommandPool(std::uint32_t QueueFamilyIndex, vk::CommandPoolCreateFlags Flags)
{
	vk::CommandPoolCreateInfo CreateInfo(Flags, QueueFamilyIndex);
	return CreateCommandPool(CreateInfo);
}

// Wrapper for vk::Fence
// ---------------------
FVulkanFence::FVulkanFence(const vk::Device& Device, const vk::FenceCreateInfo& CreateInfo)
	: _Device(&Device)
{
	CreateFence(CreateInfo);
}

FVulkanFence::FVulkanFence(const vk::Device& Device, const vk::FenceCreateFlags& Flags)
	: _Device(&Device)
{
	CreateFence(Flags);
}

FVulkanFence::FVulkanFence(FVulkanFence&& Other) noexcept
	: _Device(Other._Device), _Fence(std::move(Other._Fence))
{
	Other._Device = nullptr;
	Other._Fence  = vk::Fence();
}

FVulkanFence::~FVulkanFence()
{
	if (_Fence)
	{
		_Device->destroyFence(_Fence);
	}
}

FVulkanFence& FVulkanFence::operator=(FVulkanFence&& Other) noexcept
{
	if (this != &Other)
	{
		if (_Fence)
		{
			_Device->destroyFence(_Fence);
		}
		_Device       = Other._Device;
		_Fence        = std::move(Other._Fence);
		Other._Device = nullptr;
		Other._Fence  = vk::Fence();
	}

	return *this;
}

vk::Result FVulkanFence::Wait() const
{
	vk::Result Result;
	try
	{
		Result = _Device->waitForFences(_Fence, vk::True, std::numeric_limits<std::uint64_t>::max());
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Failed to wait for fence: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	return Result;
}

vk::Result FVulkanFence::Reset() const
{
	try
	{
		_Device->resetFences(_Fence);
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Failed to reset fence: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	return vk::Result::eSuccess;
}

vk::Result FVulkanFence::WaitAndReset() const
{
	vk::Result WaitResult = Wait();
	if (WaitResult != vk::Result::eSuccess)
	{
		return WaitResult;
	}

	vk::Result ResetResult = Reset();
	return ResetResult;
}

vk::Result FVulkanFence::GetStatus() const
{
	vk::Result Result = _Device->getFenceStatus(_Fence);
	if (Result != vk::Result::eSuccess)
	{
		NpgsCoreError("Failed to get fence status: {}.", vk::to_string(Result));
		return Result;
	}

	return vk::Result::eSuccess;
}

vk::Result FVulkanFence::CreateFence(const vk::FenceCreateInfo& CreateInfo)
{
	try
	{
		_Fence = _Device->createFence(CreateInfo);
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Failed to create fence: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	NpgsCoreInfo("Fence created successfully.");
	return vk::Result::eSuccess;
}

vk::Result FVulkanFence::CreateFence(const vk::FenceCreateFlags& Flags)
{
	vk::FenceCreateInfo FenceCreateInfo(Flags);
	return CreateFence(FenceCreateInfo);
}

// Wrapper for vk::Framebuffer
// ---------------------------
FVulkanFramebuffer::FVulkanFramebuffer(const vk::Device& Device, const vk::FramebufferCreateInfo& CreateInfo)
	: _Device(&Device)
{
	CreateFramebuffer(CreateInfo);
}

FVulkanFramebuffer::FVulkanFramebuffer(FVulkanFramebuffer&& Other) noexcept
	: _Device(Other._Device), _Framebuffer(std::move(Other._Framebuffer))
{
	Other._Device      = nullptr;
	Other._Framebuffer = vk::Framebuffer();
}

FVulkanFramebuffer::~FVulkanFramebuffer()
{
	if (_Framebuffer)
	{
		_Device->destroyFramebuffer(_Framebuffer);
	}
}

FVulkanFramebuffer& FVulkanFramebuffer::operator=(FVulkanFramebuffer&& Other) noexcept
{
	if (this != &Other)
	{
		if (_Framebuffer)
		{
			_Device->destroyFramebuffer(_Framebuffer);
		}
		_Device            = std::move(Other._Device);
		_Framebuffer       = std::move(Other._Framebuffer);
		Other._Device      = nullptr;
		Other._Framebuffer = vk::Framebuffer();
	}

	return *this;
}

vk::Result FVulkanFramebuffer::CreateFramebuffer(const vk::FramebufferCreateInfo& CreateInfo)
{
	try
	{
		_Framebuffer = _Device->createFramebuffer(CreateInfo);
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Failed to create framebuffer: {}.", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	NpgsCoreInfo("Framebuffer created successfully.");
	return vk::Result::eSuccess;
}

// Wrapper for vk::Pipeline
// ------------------------
FVulkanPipeline::FVulkanPipeline(const vk::Device& Device, const vk::GraphicsPipelineCreateInfo& CreateInfo)
	: _Device(&Device)
{
	CreateGraphicsPipeline(CreateInfo);
}

FVulkanPipeline::FVulkanPipeline(const vk::Device& Device, const vk::ComputePipelineCreateInfo& CreateInfo)
	: _Device(&Device)
{
	CreateComputePipeline(CreateInfo);
}

FVulkanPipeline::FVulkanPipeline(FVulkanPipeline&& Other) noexcept
	: _Device(Other._Device), _Pipeline(std::move(Other._Pipeline))
{
	Other._Device   = nullptr;
	Other._Pipeline = vk::Pipeline();
}

FVulkanPipeline::~FVulkanPipeline()
{
	if (_Pipeline)
	{
		_Device->destroyPipeline(_Pipeline);
	}
}

FVulkanPipeline& FVulkanPipeline::operator=(FVulkanPipeline&& Other) noexcept
{
	if (this != &Other)
	{
		if (_Pipeline)
		{
			_Device->destroyPipeline(_Pipeline);
		}
		_Device         = Other._Device;
		_Pipeline       = std::move(Other._Pipeline);
		Other._Device   = nullptr;
		Other._Pipeline = vk::Pipeline();
	}

	return *this;
}

vk::Result FVulkanPipeline::CreateGraphicsPipeline(const vk::GraphicsPipelineCreateInfo& CreateInfo)
{
	try
	{
		_Pipeline = _Device->createGraphicsPipeline(vk::PipelineCache(), CreateInfo).value;
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Failed to create graphics pipeline: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	NpgsCoreInfo("Graphics pipeline created successfully");
	return vk::Result::eSuccess;
}

vk::Result FVulkanPipeline::CreateComputePipeline(const vk::ComputePipelineCreateInfo& CreateInfo)
{
	try
	{
		_Pipeline = _Device->createComputePipeline(vk::PipelineCache(), CreateInfo).value;
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Failed to create compute pipeline: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	NpgsCoreInfo("Compute pipeline created successfully");
	return vk::Result::eSuccess;
}

// Wrapper for vk::PipelineLayout
// ------------------------------
FVulkanPipelineLayout::FVulkanPipelineLayout(const vk::Device& Device, const vk::PipelineLayoutCreateInfo& CreateInfo)
	: _Device(&Device)
{
	CreatePipelineLayout(CreateInfo);
}

FVulkanPipelineLayout::FVulkanPipelineLayout(FVulkanPipelineLayout& Other) noexcept
	: _Device(Other._Device), _PipelineLayout(std::move(Other._PipelineLayout))
{
	Other._Device         = nullptr;
	Other._PipelineLayout = vk::PipelineLayout();
}

FVulkanPipelineLayout::~FVulkanPipelineLayout()
{
	if (_PipelineLayout)
	{
		_Device->destroyPipelineLayout(_PipelineLayout);
	}
}

FVulkanPipelineLayout& FVulkanPipelineLayout::operator=(FVulkanPipelineLayout&& Other) noexcept
{
	if (this != &Other)
	{
		if (_PipelineLayout)
		{
			_Device->destroyPipelineLayout(_PipelineLayout);
		}
		_Device               = Other._Device;
		_PipelineLayout       = std::move(Other._PipelineLayout);
		Other._Device         = nullptr;
		Other._PipelineLayout = vk::PipelineLayout();
	}

	return *this;
}

vk::Result FVulkanPipelineLayout::CreatePipelineLayout(const vk::PipelineLayoutCreateInfo& CreateInfo)
{
	try
	{
		_PipelineLayout = _Device->createPipelineLayout(CreateInfo);
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

// Wrapper for vk::RenderPass
// --------------------------
FVulkanRenderPass::FVulkanRenderPass(const vk::Device& Device, const vk::RenderPassCreateInfo& CreateInfo)
	: _Device(&Device)
{
	CreateRenderPass(CreateInfo);
}

FVulkanRenderPass::FVulkanRenderPass(FVulkanRenderPass&& Other) noexcept
	: _Device(Other._Device), _RenderPass(std::move(Other._RenderPass))
{
	Other._Device     = nullptr;
	Other._RenderPass = vk::RenderPass();
}

FVulkanRenderPass::~FVulkanRenderPass()
{
	if (_RenderPass)
	{
		_Device->destroyRenderPass(_RenderPass);
	}
}

FVulkanRenderPass& FVulkanRenderPass::operator=(FVulkanRenderPass&& Other) noexcept
{
	if (this != &Other)
	{
		if (_RenderPass)
		{
			_Device->destroyRenderPass(_RenderPass);
		}
		_Device           = Other._Device;
		_RenderPass       = std::move(Other._RenderPass);
		Other._Device     = nullptr;
		Other._RenderPass = vk::RenderPass();
	}

	return *this;
}

void FVulkanRenderPass::CommandBegin(const FVulkanCommandBuffer& CommandBuffer, const FVulkanFramebuffer& Framebuffer,
									 const vk::Rect2D& RenderArea, const std::vector<vk::ClearValue>& ClearValues,
									 const vk::SubpassContents& SubpassContents) const
{
	vk::RenderPassBeginInfo RenderPassBeginInfo(_RenderPass, Framebuffer.GetFramebuffer(), RenderArea, ClearValues);
	CommandBegin(CommandBuffer, RenderPassBeginInfo, SubpassContents);
}

vk::Result FVulkanRenderPass::CreateRenderPass(const vk::RenderPassCreateInfo& CreateInfo)
{
	try
	{
		_RenderPass = _Device->createRenderPass(CreateInfo);
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Failed to create render pass: {}.", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	NpgsCoreInfo("Render pass created successfully.");
	return vk::Result::eSuccess;
}

// Wrapper for vk::Semaphore
// -------------------------
FVulkanSemaphore::FVulkanSemaphore(const vk::Device& Device, const vk::SemaphoreCreateInfo& CreateInfo)
	: _Device(&Device)
{
	CreateSemaphore(CreateInfo);
}

FVulkanSemaphore::FVulkanSemaphore(const vk::Device& Device, const vk::SemaphoreCreateFlags& Flags)
	: _Device(&Device)
{
	CreateSemaphore(Flags);
}

FVulkanSemaphore::FVulkanSemaphore(FVulkanSemaphore&& Other) noexcept
	: _Device(Other._Device), _Semaphore(std::move(Other._Semaphore))
{
	Other._Device    = nullptr;
	Other._Semaphore = vk::Semaphore();
}

FVulkanSemaphore::~FVulkanSemaphore()
{
	if (_Semaphore)
	{
		_Device->destroySemaphore(_Semaphore);
	}
}

FVulkanSemaphore& FVulkanSemaphore::operator=(FVulkanSemaphore&& Other) noexcept
{
	if (this != &Other)
	{
		if (_Semaphore)
		{
			_Device->destroySemaphore(_Semaphore);
		}
		_Device          = std::move(Other._Device);
		_Semaphore       = std::move(Other._Semaphore);
		Other._Device    = nullptr;
		Other._Semaphore = vk::Semaphore();
	}

	return *this;
}

vk::Result FVulkanSemaphore::CreateSemaphore(const vk::SemaphoreCreateInfo& CreateInfo)
{
	try
	{
		_Semaphore = _Device->createSemaphore(CreateInfo);
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Failed to create semaphore: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	NpgsCoreInfo("Semaphore created successfully.");
	return vk::Result::eSuccess;
}

vk::Result FVulkanSemaphore::CreateSemaphore(const vk::SemaphoreCreateFlags& Flags)
{
	vk::SemaphoreCreateInfo SemaphoreCreateInfo(Flags);
	return CreateSemaphore(SemaphoreCreateInfo);
}

// Wrapper for vk::ShaderModule
// ----------------------------
FVulkanShaderModule::FVulkanShaderModule(const vk::Device& Device, const vk::ShaderModuleCreateInfo& CreateInfo)
	: _Device(&Device)
{
	CreateShaderModule(CreateInfo);
}

FVulkanShaderModule::FVulkanShaderModule(const vk::Device& Device, const std::string& Filename)
	: _Device(&Device)
{
	CreateShaderModule(Filename);
}

FVulkanShaderModule::FVulkanShaderModule(FVulkanShaderModule& Other) noexcept
	: _Device(Other._Device), _ShaderModule(std::move(Other._ShaderModule))
{
	Other._Device       = nullptr;
	Other._ShaderModule = vk::ShaderModule();
}

FVulkanShaderModule::~FVulkanShaderModule()
{
	if (_ShaderModule)
	{
		_Device->destroyShaderModule(_ShaderModule);
	}
}

FVulkanShaderModule& FVulkanShaderModule::operator=(FVulkanShaderModule&& Other) noexcept
{
	if (this != &Other)
	{
		if (_ShaderModule)
		{
			_Device->destroyShaderModule(_ShaderModule);
		}
		_Device             = std::move(Other._Device);
		_ShaderModule       = std::move(Other._ShaderModule);
		Other._Device       = nullptr;
		Other._ShaderModule = vk::ShaderModule();
	}

	return *this;
}

vk::Result FVulkanShaderModule::CreateShaderModule(const vk::ShaderModuleCreateInfo& CreateInfo)
{
	try
	{
		_ShaderModule = _Device->createShaderModule(CreateInfo);
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Failed to create shader module: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	NpgsCoreInfo("Shader module created successfully.");
	return vk::Result::eSuccess;
}

vk::Result FVulkanShaderModule::CreateShaderModule(const std::string& Filename)
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

_NPGS_END
