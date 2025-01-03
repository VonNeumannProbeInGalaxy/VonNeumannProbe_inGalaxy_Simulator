#pragma once

#include "VulkanBase.h"

_NPGS_BEGIN

NPGS_INLINE const FVulkanCommandBuffer& FVulkanBase::GetTransferCommandBuffer() const
{
	return _TransferCommandBuffer;
}

NPGS_INLINE const FVulkanCommandBuffer& FVulkanBase::GetPresentCommandBuffer() const
{
	return _PresentCommandBuffer;
}

NPGS_INLINE const FVulkanCommandPool& FVulkanBase::GetGraphicsCommandPool() const
{
	return _GraphicsCommandPool;
}

NPGS_INLINE const FVulkanCommandPool& FVulkanBase::GetPresentCommandPool() const
{
	return _PresentCommandPool;
}

NPGS_INLINE const FVulkanCommandPool& FVulkanBase::GetComputeCommandPool() const
{
	return _ComputeCommandPool;
}

NPGS_INLINE FVulkanCore* FVulkanBase::GetVulkanCore() const
{
	return _VulkanCore.get();
}

_NPGS_END
