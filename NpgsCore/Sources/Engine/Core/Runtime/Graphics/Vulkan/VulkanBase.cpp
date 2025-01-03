#include "VulkanBase.h"

_NPGS_BEGIN
_RUNTIME_BEGIN
_GRAPHICS_BEGIN

namespace
{
    vk::SubmitInfo CreateSubmitInfo(const vk::CommandBuffer& CommandBuffer, const vk::Semaphore& WaitSemaphore,
                                    const vk::Semaphore& SignalSemaphore, vk::PipelineStageFlags Flags = {});
}

vk::Result FVulkanBase::ExecuteGraphicsCommand(const FVulkanCommandBuffer& CommandBuffer) const
{
    vk::SubmitInfo SubmitInfo = CreateSubmitInfo(*CommandBuffer, {}, {});

    FVulkanFence Fence;
    vk::Result Result;
    if ((Result = SubmitCommandBufferToGraphics(CommandBuffer, Fence)) != vk::Result::eSuccess)
    {
        Fence.Wait();
    }

    return Result;
}

vk::Result FVulkanBase::SubmitCommandBufferToGraphics(const vk::SubmitInfo& SubmitInfo, const FVulkanFence& Fence) const
{
    try
    {
        _VulkanCore->GetGraphicsQueue().submit(SubmitInfo, *Fence);
    }
    catch (const vk::SystemError& Error)
    {
        NpgsCoreError("Failed to submit command buffer to graphics queue: {}", Error.what());
        return static_cast<vk::Result>(Error.code().value());
    }

    return vk::Result::eSuccess;
}

vk::Result FVulkanBase::SubmitCommandBufferToGraphics(const FVulkanCommandBuffer& Buffer, const FVulkanFence& Fence) const
{
    vk::SubmitInfo SubmitInfo(1, nullptr, nullptr, 1, &(*Buffer), 0, nullptr);
    return SubmitCommandBufferToGraphics(SubmitInfo, Fence);
}

vk::Result FVulkanBase::SubmitCommandBufferToGraphics(const FVulkanCommandBuffer& Buffer, const FVulkanSemaphore& WaitSemaphore,
                                                      const FVulkanSemaphore& SignalSemaphore, const FVulkanFence& Fence,
                                                      vk::PipelineStageFlags Flags) const
{
    vk::SubmitInfo SubmitInfo = CreateSubmitInfo(*Buffer, *WaitSemaphore, *SignalSemaphore, Flags);
    return SubmitCommandBufferToGraphics(SubmitInfo, Fence);
}

vk::Result FVulkanBase::SubmitCommandBufferToPresent(const FVulkanCommandBuffer& Buffer, const FVulkanSemaphore& WaitSemaphore,
                                                     const FVulkanSemaphore& SignalSemaphore, const FVulkanFence& Fence) const
{
    vk::SubmitInfo SubmitInfo =
        CreateSubmitInfo(*Buffer, *WaitSemaphore, *SignalSemaphore, vk::PipelineStageFlagBits::eAllCommands);

    try
    {
        _VulkanCore->GetPresentQueue().submit(SubmitInfo, *Fence);
    }
    catch (const vk::SystemError& Error)
    {
        NpgsCoreError("Failed to submit command buffer to present queue: {}", Error.what());
        return static_cast<vk::Result>(Error.code().value());
    }

    return vk::Result::eSuccess;
}

vk::Result FVulkanBase::SubmitCommandBufferToCompute(const vk::SubmitInfo& SubmitInfo, const FVulkanFence& Fence) const
{
    try
    {
        _VulkanCore->GetComputeQueue().submit(SubmitInfo, *Fence);
    }
    catch (const vk::SystemError& Error)
    {
        NpgsCoreError("Failed to submit command buffer to compute queue: {}", Error.what());
        return static_cast<vk::Result>(Error.code().value());
    }

    return vk::Result::eSuccess;
}

vk::Result FVulkanBase::SubmitCommandBufferToCompute(const FVulkanCommandBuffer& Buffer, const FVulkanFence& Fence) const
{
    vk::SubmitInfo SubmitInfo(1, nullptr, nullptr, 1, &(*Buffer), 0, nullptr);
    return SubmitCommandBufferToCompute(SubmitInfo, Fence);
}

vk::Result FVulkanBase::SubmitCommandBufferToCompute(const FVulkanCommandBuffer& Buffer, const FVulkanSemaphore& WaitSemaphore,
                                                     const FVulkanSemaphore& SignalSemaphore, const FVulkanFence& Fence,
                                                     vk::PipelineStageFlags Flags) const
{
    vk::SubmitInfo SubmitInfo = CreateSubmitInfo(*Buffer, *WaitSemaphore, *SignalSemaphore, Flags);
    return SubmitCommandBufferToCompute(SubmitInfo, Fence);
}

vk::Result FVulkanBase::TransferImageOwnershipToPresent(const FVulkanCommandBuffer& PresentCommandBuffer,
                                                        const FVulkanSemaphore& WaitSemaphore,
                                                        const FVulkanSemaphore& SignalSemaphore,
                                                        const FVulkanFence& Fence) const
{
    vk::Result Result;
    if ((Result = PresentCommandBuffer.Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit)) != vk::Result::eSuccess)
    {
        return Result;
    }

    TransferImageOwnershipToPresentImpl(PresentCommandBuffer);
    return vk::Result::eSuccess;
}

FVulkanBase* FVulkanBase::GetInstance()
{
    static FVulkanBase Instance;
    return &Instance;
}

FVulkanBase::FVulkanBase()
    : _VulkanCore(std::make_unique<FVulkanCore>())
{
    auto InitCommandPool = [this, VulkanCore = _VulkanCore.get()]() -> void
    {
        if (VulkanCore->GetGraphicsQueueFamilyIndex() != vk::QueueFamilyIgnored)
        {
            _GraphicsCommandPool = FVulkanCommandPool(VulkanCore->GetDevice(), VulkanCore->GetGraphicsQueueFamilyIndex(),
                                                      vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
            _GraphicsCommandPool.AllocateBuffer(vk::CommandBufferLevel::ePrimary, _TransferCommandBuffer);
        }
        if (VulkanCore->GetComputeQueueFamilyIndex() != vk::QueueFamilyIgnored)
        {
            _ComputeCommandPool = FVulkanCommandPool(VulkanCore->GetDevice(), VulkanCore->GetComputeQueueFamilyIndex(),
                                                     vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
        }
        if (VulkanCore->GetPresentQueueFamilyIndex() != vk::QueueFamilyIgnored &&
            VulkanCore->GetPresentQueueFamilyIndex() != VulkanCore->GetGraphicsQueueFamilyIndex() &&
            VulkanCore->GetSwapchainCreateInfo().imageSharingMode == vk::SharingMode::eExclusive)
        {
            _PresentCommandPool = FVulkanCommandPool(VulkanCore->GetDevice(), VulkanCore->GetPresentQueueFamilyIndex(),
                                                     vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
            _PresentCommandPool.AllocateBuffer(vk::CommandBufferLevel::ePrimary, _PresentCommandBuffer);
        }
    };

    _VulkanCore->AddCreateDeviceCallback("InitCommandPool", InitCommandPool);
}

FVulkanBase::~FVulkanBase()
{
}

void FVulkanBase::TransferImageOwnershipToPresentImpl(const FVulkanCommandBuffer& PresentCommandBuffer) const
{
    vk::ImageMemoryBarrier ImageMemoryBarrier = vk::ImageMemoryBarrier()
        .setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
        .setDstAccessMask(vk::AccessFlagBits::eNone)
        .setOldLayout(vk::ImageLayout::ePresentSrcKHR)
        .setNewLayout(vk::ImageLayout::ePresentSrcKHR)
        .setSrcQueueFamilyIndex(_VulkanCore->GetGraphicsQueueFamilyIndex())
        .setDstQueueFamilyIndex(_VulkanCore->GetPresentQueueFamilyIndex())
        .setImage(_VulkanCore->GetSwapchainImage(_VulkanCore->GetCurrentImageIndex()))
        .setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

    PresentCommandBuffer->pipelineBarrier(vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                          vk::PipelineStageFlagBits::eBottomOfPipe, {}, {}, {}, ImageMemoryBarrier);
}

namespace
{
    vk::SubmitInfo CreateSubmitInfo(const vk::CommandBuffer& Buffer, const vk::Semaphore& WaitSemaphore,
                                    const vk::Semaphore& SignalSemaphore, vk::PipelineStageFlags Flags)
    {
        vk::SubmitInfo SubmitInfo;
        SubmitInfo.setCommandBuffers(Buffer);

        if (WaitSemaphore)
        {
            SubmitInfo.setWaitSemaphores(WaitSemaphore);
            SubmitInfo.setWaitDstStageMask(Flags);
        }

        if (SignalSemaphore)
        {
            SubmitInfo.setSignalSemaphores(SignalSemaphore);
        }

        return SubmitInfo;
    }
}

_GRAPHICS_END
_RUNTIME_END
_NPGS_END
