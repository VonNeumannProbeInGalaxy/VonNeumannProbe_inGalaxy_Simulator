#pragma once

#include "VulkanBase.h"

_NPGS_BEGIN

NPGS_INLINE void FVulkanBase::AddInstanceLayer(const char* Layer)
{
	AddElementChecked(_InstanceLayers, Layer);
}

NPGS_INLINE void FVulkanBase::SetInstanceLayers(const std::vector<const char*>& Layers)
{
	_InstanceLayers = Layers;
}

NPGS_INLINE void FVulkanBase::AddInstanceExtension(const char* Extension)
{
	AddElementChecked(_InstanceExtensions, Extension);
}

NPGS_INLINE void FVulkanBase::SetInstanceExtensions(const std::vector<const char*>& Extensions)
{
	_InstanceExtensions = Extensions;
}

NPGS_INLINE void FVulkanBase::AddDeviceExtension(const char* Extension)
{
	AddElementChecked(_DeviceExtensions, Extension);
}

NPGS_INLINE void FVulkanBase::SetDeviceExtensions(const std::vector<const char*>& Extensions)
{
	_DeviceExtensions = Extensions;
}

NPGS_INLINE void FVulkanBase::SetSurface(const vk::SurfaceKHR& Surface)
{
	_Surface = Surface;
}

NPGS_INLINE const std::vector<const char*>& FVulkanBase::GetInstanceLayers() const
{
	return _InstanceLayers;
}

NPGS_INLINE const std::vector<const char*>& FVulkanBase::GetInstanceExtensions() const
{
	return _InstanceExtensions;
}

NPGS_INLINE const std::vector<const char*>& FVulkanBase::GetDeviceExtensions() const
{
	return _DeviceExtensions;
}

NPGS_INLINE const vk::Instance& FVulkanBase::GetInstance() const
{
	return _Instance;
}

NPGS_INLINE const vk::SurfaceKHR& FVulkanBase::GetSurface() const
{
	return _Surface;
}

NPGS_INLINE const vk::PhysicalDevice& FVulkanBase::GetPhysicalDevice() const
{
	return _PhysicalDevice;
}

NPGS_INLINE const vk::PhysicalDeviceProperties& FVulkanBase::GetPhysicalDeviceProperties() const
{
	return _PhysicalDeviceProperties;
}

NPGS_INLINE const vk::PhysicalDeviceMemoryProperties& FVulkanBase::GetPhysicalDeviceMemoryProperties() const
{
	return _PhysicalDeviceMemoryProperties;
}

NPGS_INLINE const vk::Device& FVulkanBase::GetDevice() const
{
	return _Device;
}

NPGS_INLINE const vk::Queue& FVulkanBase::GetGraphicsQueue() const
{
	return _GraphicsQueue;
}

NPGS_INLINE const vk::Queue& FVulkanBase::GetPresentQueue() const
{
	return _PresentQueue;
}

NPGS_INLINE const vk::Queue& FVulkanBase::GetComputeQueue() const
{
	return _ComputeQueue;
}

NPGS_INLINE const vk::SwapchainKHR& FVulkanBase::GetSwapchain() const
{
	return _Swapchain;
}

NPGS_INLINE const vk::SwapchainCreateInfoKHR& FVulkanBase::GetSwapchainCreateInfo() const
{
	return _SwapchainCreateInfo;
}

NPGS_INLINE std::uint32_t FVulkanBase::GetAvailablePhysicalDeviceCount() const
{
	return static_cast<std::uint32_t>(_AvailablePhysicalDevices.size());
}

NPGS_INLINE std::uint32_t FVulkanBase::GetAvailableSurfaceFormatCount() const
{
	return static_cast<std::uint32_t>(_AvailableSurfaceFormats.size());
}

NPGS_INLINE std::uint32_t FVulkanBase::GetSwapchainImageCount() const
{
	return static_cast<std::uint32_t>(_SwapchainImages.size());
}

NPGS_INLINE std::uint32_t FVulkanBase::GetSwapchainImageViewCount() const
{
	return static_cast<std::uint32_t>(_SwapchainImageViews.size());
}

NPGS_INLINE const vk::PhysicalDevice& FVulkanBase::GetAvailablePhysicalDevice(std::uint32_t Index) const
{
	return _AvailablePhysicalDevices[Index];
}

NPGS_INLINE const vk::Format& FVulkanBase::GetAvailableSurfaceFormat(std::uint32_t Index) const
{
	return _AvailableSurfaceFormats[Index].format;
}

NPGS_INLINE const vk::ColorSpaceKHR& FVulkanBase::GetAvailableSurfaceColorSpace(std::uint32_t Index) const
{
	return _AvailableSurfaceFormats[Index].colorSpace;
}

NPGS_INLINE const vk::Image& FVulkanBase::GetSwapchainImage(std::uint32_t Index) const
{
	return _SwapchainImages[Index];
}

NPGS_INLINE const vk::ImageView& FVulkanBase::GetSwapchainImageView(std::uint32_t Index) const
{
	return _SwapchainImageViews[Index];
}

NPGS_INLINE std::uint32_t FVulkanBase::GetGraphicsQueueFamilyIndex() const
{
	return _GraphicsQueueFamilyIndex;
}

NPGS_INLINE std::uint32_t FVulkanBase::GetPresentQueueFamilyIndex() const
{
	return _PresentQueueFamilyIndex;
}

NPGS_INLINE std::uint32_t FVulkanBase::GetComputeQueueFamilyIndex() const
{
	return _ComputeQueueFamilyIndex;
}

NPGS_INLINE std::uint32_t FVulkanBase::GetApiVersion() const
{
	return _ApiVersion;
}

_NPGS_END
