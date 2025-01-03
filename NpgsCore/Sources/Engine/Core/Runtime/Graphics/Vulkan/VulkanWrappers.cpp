#include "VulkanWrappers.h"

#include <cstddef>
#include <limits>
#include <utility>
#include "Engine/Utils/Logger.h"

_NPGS_BEGIN
_RUNTIME_BEGIN
_GRAPHICS_BEGIN

FGraphicsPipelineCreateInfoPack::FGraphicsPipelineCreateInfoPack()
{
    LinkToGraphicsPipelineCreateInfo();
    GraphicsPipelineCreateInfo.setBasePipelineIndex(-1);
}

FGraphicsPipelineCreateInfoPack::FGraphicsPipelineCreateInfoPack(FGraphicsPipelineCreateInfoPack&& Other) noexcept
    :
    GraphicsPipelineCreateInfo(std::exchange(Other.GraphicsPipelineCreateInfo, {})),
    DepthStencilStateCreateInfo(std::exchange(Other.DepthStencilStateCreateInfo, {})),
    RasterizationStateCreateInfo(std::exchange(Other.RasterizationStateCreateInfo, {})),
    ColorBlendStateCreateInfo(std::exchange(Other.ColorBlendStateCreateInfo, {})),
    VertexInputStateCreateInfo(std::exchange(Other.VertexInputStateCreateInfo, {})),
    ViewportStateCreateInfo(std::exchange(Other.ViewportStateCreateInfo, {})),
    MultisampleStateCreateInfo(std::exchange(Other.MultisampleStateCreateInfo, {})),
    InputAssemblyStateCreateInfo(std::exchange(Other.InputAssemblyStateCreateInfo, {})),
    DynamicStateCreateInfo(std::exchange(Other.DynamicStateCreateInfo, {})),
    TessellationStateCreateInfo(std::exchange(Other.TessellationStateCreateInfo, {})),

    ShaderStages(std::move(Other.ShaderStages)),
    VertexInputBindings(std::move(Other.VertexInputBindings)),
    VertexInputAttributes(std::move(Other.VertexInputAttributes)),
    Viewports(std::move(Other.Viewports)),
    Scissors(std::move(Other.Scissors)),
    ColorBlendAttachmentStates(std::move(Other.ColorBlendAttachmentStates)),
    DynamicStates(std::move(Other.DynamicStates)),

    DynamicViewportCount(std::exchange(Other.DynamicViewportCount, 1)),
    DynamicScissorCount(std::exchange(Other.DynamicScissorCount, 1))
{
    LinkToGraphicsPipelineCreateInfo();
    UpdateAllInfoData();
}

FGraphicsPipelineCreateInfoPack& FGraphicsPipelineCreateInfoPack::operator=(FGraphicsPipelineCreateInfoPack&& Other) noexcept
{
    if (this != &Other)
    {
        GraphicsPipelineCreateInfo   = std::exchange(Other.GraphicsPipelineCreateInfo, {});
        DepthStencilStateCreateInfo  = std::exchange(Other.DepthStencilStateCreateInfo, {});
        RasterizationStateCreateInfo = std::exchange(Other.RasterizationStateCreateInfo, {});
        ColorBlendStateCreateInfo    = std::exchange(Other.ColorBlendStateCreateInfo, {});
        VertexInputStateCreateInfo   = std::exchange(Other.VertexInputStateCreateInfo, {});
        ViewportStateCreateInfo      = std::exchange(Other.ViewportStateCreateInfo, {});
        MultisampleStateCreateInfo   = std::exchange(Other.MultisampleStateCreateInfo, {});
        InputAssemblyStateCreateInfo = std::exchange(Other.InputAssemblyStateCreateInfo, {});
        DynamicStateCreateInfo       = std::exchange(Other.DynamicStateCreateInfo, {});
        TessellationStateCreateInfo  = std::exchange(Other.TessellationStateCreateInfo, {});

        ShaderStages                 = std::move(Other.ShaderStages);
        VertexInputBindings          = std::move(Other.VertexInputBindings);
        VertexInputAttributes        = std::move(Other.VertexInputAttributes);
        Viewports                    = std::move(Other.Viewports);
        Scissors                     = std::move(Other.Scissors);
        ColorBlendAttachmentStates   = std::move(Other.ColorBlendAttachmentStates);
        DynamicStates                = std::move(Other.DynamicStates);

        DynamicViewportCount         = std::exchange(Other.DynamicViewportCount, 1);
        DynamicScissorCount          = std::exchange(Other.DynamicScissorCount, 1);

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
    : _Handle(std::exchange(Other._Handle, {}))
{
    Other._Handle = vk::CommandBuffer();
}

FVulkanCommandBuffer& FVulkanCommandBuffer::operator=(FVulkanCommandBuffer&& Other) noexcept
{
    if (this != &Other)
    {
        _Handle = std::exchange(Other._Handle, {});
    }

    return *this;
}

vk::Result FVulkanCommandBuffer::Begin(const vk::CommandBufferInheritanceInfo& InheritanceInfo,
                                       vk::CommandBufferUsageFlags Flags) const
{
    vk::CommandBufferBeginInfo CommandBufferBeginInfo(Flags, &InheritanceInfo);
    try
    {
        _Handle.begin(CommandBufferBeginInfo);
    }
    catch (const vk::SystemError& Error)
    {
        NpgsCoreError("Failed to begin command buffer: {}", Error.what());
        return static_cast<vk::Result>(Error.code().value());
    }

    return vk::Result::eSuccess;
}

vk::Result FVulkanCommandBuffer::Begin(vk::CommandBufferUsageFlags Flags) const
{
    vk::CommandBufferBeginInfo CommandBufferBeginInfo(Flags);
    try
    {
        _Handle.begin(CommandBufferBeginInfo);
    }
    catch (const vk::SystemError& Error)
    {
        NpgsCoreError("Failed to begin command buffer: {}", Error.what());
        return static_cast<vk::Result>(Error.code().value());
    }

    return vk::Result::eSuccess;
}

vk::Result FVulkanCommandBuffer::End() const
{
    try
    {
        _Handle.end();
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
FVulkanCommandPool::FVulkanCommandPool()
    : _Device(nullptr)
{
}

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
    :
    _Device(std::exchange(Other._Device, nullptr)),
    _Handle(std::exchange(Other._Handle, {}))
{
}

FVulkanCommandPool::~FVulkanCommandPool()
{
    if (_Handle)
    {
        _Device->destroyCommandPool(_Handle);
        NpgsCoreInfo("Command pool destroyed successfully.");
    }
}

FVulkanCommandPool& FVulkanCommandPool::operator=(FVulkanCommandPool&& Other) noexcept
{
    if (this != &Other)
    {
        if (_Handle)
        {
            _Device->destroyCommandPool(_Handle);
        }
        _Device = std::exchange(Other._Device, nullptr);
        _Handle = std::exchange(Other._Handle, {});
    }

    return *this;
}

vk::Result FVulkanCommandPool::AllocateBuffer(vk::CommandBufferLevel Level, vk::CommandBuffer& Buffer)
{
    vk::CommandBufferAllocateInfo CommandBufferAllocateInfo(_Handle, Level, 1);
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
    vk::CommandBufferAllocateInfo CommandBufferAllocateInfo(_Handle, Level, 1);
    try
    {
        *Buffer = _Device->allocateCommandBuffers(CommandBufferAllocateInfo)[0];
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
    vk::CommandBufferAllocateInfo CommandBufferAllocateInfo(_Handle, Level, static_cast<std::uint32_t>(Buffers.size()));
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
    vk::CommandBufferAllocateInfo CommandBufferAllocateInfo(_Handle, Level, static_cast<std::uint32_t>(Buffers.size()));
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
        *Buffers[i] = CommandBuffers[i];
    }

    NpgsCoreInfo("Command buffers allocated successfully.");
    return vk::Result::eSuccess;
}

vk::Result FVulkanCommandPool::FreeBuffer(vk::CommandBufferLevel Level, vk::CommandBuffer& Buffer)
{
    _Device->freeCommandBuffers(_Handle, Buffer);
    Buffer = vk::CommandBuffer();
    NpgsCoreInfo("Command buffer freed successfully.");
    return vk::Result::eSuccess;
}

vk::Result FVulkanCommandPool::FreeBuffer(vk::CommandBufferLevel Level, FVulkanCommandBuffer& Buffer)
{
    return FreeBuffer(Level, *Buffer);
}

vk::Result FVulkanCommandPool::FreeBuffers(vk::CommandBufferLevel Level, std::vector<vk::CommandBuffer>& Buffers)
{
    _Device->freeCommandBuffers(_Handle, Buffers);
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
        CommandBuffers.emplace_back(*Buffer);
    }

    _Device->freeCommandBuffers(_Handle, CommandBuffers);
    Buffers.clear();

    NpgsCoreInfo("Command buffers freed successfully.");
    return vk::Result::eSuccess;
}

vk::Result FVulkanCommandPool::CreateCommandPool(vk::CommandPoolCreateInfo& CreateInfo)
{
    try
    {
        _Handle = _Device->createCommandPool(CreateInfo);
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
FVulkanFence::FVulkanFence()
    : _Device(nullptr)
{
}

FVulkanFence::FVulkanFence(const vk::Device& Device, const vk::FenceCreateInfo& CreateInfo)
    : _Device(&Device)
{
    CreateFence(CreateInfo);
}

FVulkanFence::FVulkanFence(const vk::Device& Device, vk::FenceCreateFlags Flags)
    : _Device(&Device)
{
    CreateFence(Flags);
}

FVulkanFence::FVulkanFence(FVulkanFence&& Other) noexcept
    :
    _Device(std::exchange(Other._Device, nullptr)),
    _Handle(std::exchange(Other._Handle, {}))
{
}

FVulkanFence::~FVulkanFence()
{
    if (_Handle)
    {
        _Device->destroyFence(_Handle);
        NpgsCoreInfo("Fence destroyed successfully.");
    }
}

FVulkanFence& FVulkanFence::operator=(FVulkanFence&& Other) noexcept
{
    if (this != &Other)
    {
        if (_Handle)
        {
            _Device->destroyFence(_Handle);
        }
        _Device = std::exchange(Other._Device, nullptr);
        _Handle = std::exchange(Other._Handle, {});
    }

    return *this;
}

vk::Result FVulkanFence::Wait() const
{
    vk::Result Result;
    try
    {
        Result = _Device->waitForFences(_Handle, vk::True, std::numeric_limits<std::uint64_t>::max());
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
        _Device->resetFences(_Handle);
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
    vk::Result Result = _Device->getFenceStatus(_Handle);
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
        _Handle = _Device->createFence(CreateInfo);
    }
    catch (const vk::SystemError& Error)
    {
        NpgsCoreError("Failed to create fence: {}", Error.what());
        return static_cast<vk::Result>(Error.code().value());
    }

    NpgsCoreInfo("Fence created successfully.");
    return vk::Result::eSuccess;
}

vk::Result FVulkanFence::CreateFence(vk::FenceCreateFlags Flags)
{
    vk::FenceCreateInfo FenceCreateInfo(Flags);
    return CreateFence(FenceCreateInfo);
}

// Wrapper for vk::Framebuffer
// ---------------------------
FVulkanFramebuffer::FVulkanFramebuffer()
    : _Device(nullptr)
{
}

FVulkanFramebuffer::FVulkanFramebuffer(const vk::Device& Device, const vk::FramebufferCreateInfo& CreateInfo)
    : _Device(&Device)
{
    CreateFramebuffer(CreateInfo);
}

FVulkanFramebuffer::FVulkanFramebuffer(FVulkanFramebuffer&& Other) noexcept
    :
    _Device(std::exchange(Other._Device, nullptr)),
    _Handle(std::exchange(Other._Handle, {}))
{
}

FVulkanFramebuffer::~FVulkanFramebuffer()
{
    if (_Handle)
    {
        _Device->destroyFramebuffer(_Handle);
        NpgsCoreInfo("Framebuffer destroyed successfully.");
    }
}

FVulkanFramebuffer& FVulkanFramebuffer::operator=(FVulkanFramebuffer&& Other) noexcept
{
    if (this != &Other)
    {
        if (_Handle)
        {
            _Device->destroyFramebuffer(_Handle);
        }
        _Device = std::exchange(Other._Device, nullptr);
        _Handle = std::exchange(Other._Handle, {});
    }

    return *this;
}

vk::Result FVulkanFramebuffer::CreateFramebuffer(const vk::FramebufferCreateInfo& CreateInfo)
{
    try
    {
        _Handle = _Device->createFramebuffer(CreateInfo);
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
FVulkanPipeline::FVulkanPipeline()
    : _Device(nullptr)
{
}

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
    :
    _Device(std::exchange(Other._Device, nullptr)),
    _Handle(std::exchange(Other._Handle, {}))
{
}

FVulkanPipeline::~FVulkanPipeline()
{
    if (_Handle)
    {
        _Device->destroyPipeline(_Handle);
        NpgsCoreInfo("Pipeline destroyed successfully.");
    }
}

FVulkanPipeline& FVulkanPipeline::operator=(FVulkanPipeline&& Other) noexcept
{
    if (this != &Other)
    {
        if (_Handle)
        {
            _Device->destroyPipeline(_Handle);
        }
        _Device = std::exchange(Other._Device, nullptr);
        _Handle = std::exchange(Other._Handle, {});
    }

    return *this;
}

vk::Result FVulkanPipeline::CreateGraphicsPipeline(const vk::GraphicsPipelineCreateInfo& CreateInfo)
{
    try
    {
        _Handle = _Device->createGraphicsPipeline(vk::PipelineCache(), CreateInfo).value;
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
        _Handle = _Device->createComputePipeline(vk::PipelineCache(), CreateInfo).value;
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
FVulkanPipelineLayout::FVulkanPipelineLayout()
    : _Device(nullptr)
{
}

FVulkanPipelineLayout::FVulkanPipelineLayout(const vk::Device& Device, const vk::PipelineLayoutCreateInfo& CreateInfo)
    : _Device(&Device)
{
    CreatePipelineLayout(CreateInfo);
}

FVulkanPipelineLayout::FVulkanPipelineLayout(FVulkanPipelineLayout& Other) noexcept
    :
    _Device(std::exchange(Other._Device, nullptr)),
    _Handle(std::exchange(Other._Handle, {}))
{
}

FVulkanPipelineLayout::~FVulkanPipelineLayout()
{
    if (_Handle)
    {
        _Device->destroyPipelineLayout(_Handle);
        NpgsCoreInfo("Pipeline layout destroyed successfully.");
    }
}

FVulkanPipelineLayout& FVulkanPipelineLayout::operator=(FVulkanPipelineLayout&& Other) noexcept
{
    if (this != &Other)
    {
        if (_Handle)
        {
            _Device->destroyPipelineLayout(_Handle);
        }
        _Device = std::exchange(Other._Device, nullptr);
        _Handle = std::exchange(Other._Handle, {});
    }

    return *this;
}

vk::Result FVulkanPipelineLayout::CreatePipelineLayout(const vk::PipelineLayoutCreateInfo& CreateInfo)
{
    try
    {
        _Handle = _Device->createPipelineLayout(CreateInfo);
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
FVulkanRenderPass::FVulkanRenderPass()
    : _Device(nullptr)
{
}

FVulkanRenderPass::FVulkanRenderPass(const vk::Device& Device, const vk::RenderPassCreateInfo& CreateInfo)
    : _Device(&Device)
{
    CreateRenderPass(CreateInfo);
}

FVulkanRenderPass::FVulkanRenderPass(FVulkanRenderPass&& Other) noexcept
    :
    _Device(std::exchange(Other._Device, nullptr)),
    _Handle(std::exchange(Other._Handle, {}))
{
}

FVulkanRenderPass::~FVulkanRenderPass()
{
    if (_Handle)
    {
        _Device->destroyRenderPass(_Handle);
        NpgsCoreInfo("Render pass destroyed successfully.");
    }
}

FVulkanRenderPass& FVulkanRenderPass::operator=(FVulkanRenderPass&& Other) noexcept
{
    if (this != &Other)
    {
        if (_Handle)
        {
            _Device->destroyRenderPass(_Handle);
        }
        _Device = std::exchange(Other._Device, nullptr);
        _Handle = std::exchange(Other._Handle, {});
    }

    return *this;
}

void FVulkanRenderPass::CommandBegin(const FVulkanCommandBuffer& CommandBuffer, const FVulkanFramebuffer& Framebuffer,
                                     const vk::Rect2D& RenderArea, const std::vector<vk::ClearValue>& ClearValues,
                                     const vk::SubpassContents& SubpassContents) const
{
    vk::RenderPassBeginInfo RenderPassBeginInfo(_Handle, *Framebuffer, RenderArea, ClearValues);
    CommandBegin(CommandBuffer, RenderPassBeginInfo, SubpassContents);
}

vk::Result FVulkanRenderPass::CreateRenderPass(const vk::RenderPassCreateInfo& CreateInfo)
{
    try
    {
        _Handle = _Device->createRenderPass(CreateInfo);
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
FVulkanSemaphore::FVulkanSemaphore()
    : _Device(nullptr)
{
}

FVulkanSemaphore::FVulkanSemaphore(const vk::Device& Device, const vk::SemaphoreCreateInfo& CreateInfo)
    : _Device(&Device)
{
    CreateSemaphore(CreateInfo);
}

FVulkanSemaphore::FVulkanSemaphore(const vk::Device& Device, vk::SemaphoreCreateFlags Flags)
    : _Device(&Device)
{
    CreateSemaphore(Flags);
}

FVulkanSemaphore::FVulkanSemaphore(FVulkanSemaphore&& Other) noexcept
    :
    _Device(std::exchange(Other._Device, nullptr)),
    _Handle(std::exchange(Other._Handle, {}))
{
}

FVulkanSemaphore::~FVulkanSemaphore()
{
    if (_Handle)
    {
        _Device->destroySemaphore(_Handle);
        NpgsCoreInfo("Semaphore destroyed successfully.");
    }
}

FVulkanSemaphore& FVulkanSemaphore::operator=(FVulkanSemaphore&& Other) noexcept
{
    if (this != &Other)
    {
        if (_Handle)
        {
            _Device->destroySemaphore(_Handle);
        }
        _Device = std::exchange(Other._Device, nullptr);
        _Handle = std::exchange(Other._Handle, {});
    }

    return *this;
}

vk::Result FVulkanSemaphore::CreateSemaphore(const vk::SemaphoreCreateInfo& CreateInfo)
{
    try
    {
        _Handle = _Device->createSemaphore(CreateInfo);
    }
    catch (const vk::SystemError& Error)
    {
        NpgsCoreError("Failed to create semaphore: {}", Error.what());
        return static_cast<vk::Result>(Error.code().value());
    }

    NpgsCoreInfo("Semaphore created successfully.");
    return vk::Result::eSuccess;
}

vk::Result FVulkanSemaphore::CreateSemaphore(vk::SemaphoreCreateFlags Flags)
{
    vk::SemaphoreCreateInfo SemaphoreCreateInfo(Flags);
    return CreateSemaphore(SemaphoreCreateInfo);
}

// Wrapper for vk::ShaderModule
// ----------------------------
FVulkanShaderModule::FVulkanShaderModule()
    : _Device(nullptr)
{
}

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
    :
    _Device(std::exchange(Other._Device, nullptr)),
    _Handle(std::exchange(Other._Handle, {}))
{
}

FVulkanShaderModule::~FVulkanShaderModule()
{
    if (_Handle)
    {
        _Device->destroyShaderModule(_Handle);
        NpgsCoreInfo("Shader module destroyed successfully.");
    }
}

FVulkanShaderModule& FVulkanShaderModule::operator=(FVulkanShaderModule&& Other) noexcept
{
    if (this != &Other)
    {
        if (_Handle)
        {
            _Device->destroyShaderModule(_Handle);
        }
        _Device = std::exchange(Other._Device, nullptr);
        _Handle = std::exchange(Other._Handle, {});
    }

    return *this;
}

vk::Result FVulkanShaderModule::CreateShaderModule(const vk::ShaderModuleCreateInfo& CreateInfo)
{
    try
    {
        _Handle = _Device->createShaderModule(CreateInfo);
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

_GRAPHICS_END
_RUNTIME_END
_NPGS_END
