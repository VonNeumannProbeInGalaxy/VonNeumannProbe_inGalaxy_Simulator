#pragma once

#include "VulkanCore.h"

#include <algorithm>

_NPGS_BEGIN
_RUNTIME_BEGIN
_GRAPHICS_BEGIN

#define CompareCallback [&Name](const auto& Callback) -> bool \
{                                                             \
	return Name == Callback.first;                            \
}

NPGS_INLINE void FVulkanCore::AddInstanceLayer(const char* Layer)
{
    AddElementChecked(_InstanceLayers, Layer);
}

NPGS_INLINE void FVulkanCore::SetInstanceLayers(const std::vector<const char*>& Layers)
{
    _InstanceLayers = Layers;
}

NPGS_INLINE void FVulkanCore::AddInstanceExtension(const char* Extension)
{
    AddElementChecked(_InstanceExtensions, Extension);
}

NPGS_INLINE void FVulkanCore::SetInstanceExtensions(const std::vector<const char*>& Extensions)
{
    _InstanceExtensions = Extensions;
}

NPGS_INLINE void FVulkanCore::AddDeviceExtension(const char* Extension)
{
    AddElementChecked(_DeviceExtensions, Extension);
}

NPGS_INLINE void FVulkanCore::SetDeviceExtensions(const std::vector<const char*>& Extensions)
{
    _DeviceExtensions = Extensions;
}

NPGS_INLINE void FVulkanCore::SetSurface(const vk::SurfaceKHR& Surface)
{
    _Surface = Surface;
}

NPGS_INLINE void FVulkanCore::AddCreateSwapchainCallback(const std::string& Name, const std::function<void()>& Callback)
{
    _CreateSwapchainCallbacks.emplace_back(Name, Callback);
}

NPGS_INLINE void FVulkanCore::AddDestroySwapchainCallback(const std::string& Name, const std::function<void()>& Callback)
{
    _DestroySwapchainCallbacks.emplace_back(Name, Callback);
}

NPGS_INLINE void FVulkanCore::AddCreateDeviceCallback(const std::string& Name, const std::function<void()>& Callback)
{
    _CreateDeviceCallbacks.emplace_back(Name, Callback);
}

NPGS_INLINE void FVulkanCore::AddDestroyDeviceCallback(const std::string& Name, const std::function<void()>& Callback)
{
    _DestroyDeviceCallbacks.emplace_back(Name, Callback);
}

NPGS_INLINE void FVulkanCore::RemoveCreateSwapchainCallback(const std::string& Name)
{
    _CreateSwapchainCallbacks.erase(
        std::remove_if(_CreateSwapchainCallbacks.begin(), _CreateSwapchainCallbacks.end(), CompareCallback),
        _CreateSwapchainCallbacks.end()
    );
}

NPGS_INLINE void FVulkanCore::RemoveDestroySwapchainCallback(const std::string& Name)
{
    _DestroySwapchainCallbacks.erase(
        std::remove_if(_DestroySwapchainCallbacks.begin(), _DestroySwapchainCallbacks.end(), CompareCallback),
        _DestroySwapchainCallbacks.end()
    );
}

NPGS_INLINE void FVulkanCore::RemoveCreateDeviceCallback(const std::string& Name)
{
    _CreateDeviceCallbacks.erase(
        std::remove_if(_CreateDeviceCallbacks.begin(), _CreateDeviceCallbacks.end(), CompareCallback),
        _CreateDeviceCallbacks.end()
    );
}

NPGS_INLINE void FVulkanCore::RemoveDestroyDeviceCallback(const std::string& Name)
{
    _DestroyDeviceCallbacks.erase(
        std::remove_if(_DestroyDeviceCallbacks.begin(), _DestroyDeviceCallbacks.end(), CompareCallback),
        _DestroyDeviceCallbacks.end()
    );
}

NPGS_INLINE const std::vector<const char*>& FVulkanCore::GetInstanceLayers() const
{
    return _InstanceLayers;
}

NPGS_INLINE const std::vector<const char*>& FVulkanCore::GetInstanceExtensions() const
{
    return _InstanceExtensions;
}

NPGS_INLINE const std::vector<const char*>& FVulkanCore::GetDeviceExtensions() const
{
    return _DeviceExtensions;
}

NPGS_INLINE const vk::Instance& FVulkanCore::GetInstance() const
{
    return _Instance;
}

NPGS_INLINE const vk::SurfaceKHR& FVulkanCore::GetSurface() const
{
    return _Surface;
}

NPGS_INLINE const vk::PhysicalDevice& FVulkanCore::GetPhysicalDevice() const
{
    return _PhysicalDevice;
}

NPGS_INLINE const vk::PhysicalDeviceProperties& FVulkanCore::GetPhysicalDeviceProperties() const
{
    return _PhysicalDeviceProperties;
}

NPGS_INLINE const vk::PhysicalDeviceMemoryProperties& FVulkanCore::GetPhysicalDeviceMemoryProperties() const
{
    return _PhysicalDeviceMemoryProperties;
}

NPGS_INLINE const vk::Device& FVulkanCore::GetDevice() const
{
    return _Device;
}

NPGS_INLINE const vk::Queue& FVulkanCore::GetGraphicsQueue() const
{
    return _GraphicsQueue;
}

NPGS_INLINE const vk::Queue& FVulkanCore::GetPresentQueue() const
{
    return _PresentQueue;
}

NPGS_INLINE const vk::Queue& FVulkanCore::GetComputeQueue() const
{
    return _ComputeQueue;
}

NPGS_INLINE const vk::SwapchainKHR& FVulkanCore::GetSwapchain() const
{
    return _Swapchain;
}

NPGS_INLINE const vk::SwapchainCreateInfoKHR& FVulkanCore::GetSwapchainCreateInfo() const
{
    return _SwapchainCreateInfo;
}

NPGS_INLINE std::uint32_t FVulkanCore::GetAvailablePhysicalDeviceCount() const
{
    return static_cast<std::uint32_t>(_AvailablePhysicalDevices.size());
}

NPGS_INLINE std::uint32_t FVulkanCore::GetAvailableSurfaceFormatCount() const
{
    return static_cast<std::uint32_t>(_AvailableSurfaceFormats.size());
}

NPGS_INLINE std::uint32_t FVulkanCore::GetSwapchainImageCount() const
{
    return static_cast<std::uint32_t>(_SwapchainImages.size());
}

NPGS_INLINE std::uint32_t FVulkanCore::GetSwapchainImageViewCount() const
{
    return static_cast<std::uint32_t>(_SwapchainImageViews.size());
}

NPGS_INLINE const vk::PhysicalDevice& FVulkanCore::GetAvailablePhysicalDevice(std::uint32_t Index) const
{
    return _AvailablePhysicalDevices[Index];
}

NPGS_INLINE const vk::Format& FVulkanCore::GetAvailableSurfaceFormat(std::uint32_t Index) const
{
    return _AvailableSurfaceFormats[Index].format;
}

NPGS_INLINE const vk::ColorSpaceKHR& FVulkanCore::GetAvailableSurfaceColorSpace(std::uint32_t Index) const
{
    return _AvailableSurfaceFormats[Index].colorSpace;
}

NPGS_INLINE const vk::Image& FVulkanCore::GetSwapchainImage(std::uint32_t Index) const
{
    return _SwapchainImages[Index];
}

NPGS_INLINE const vk::ImageView& FVulkanCore::GetSwapchainImageView(std::uint32_t Index) const
{
    return _SwapchainImageViews[Index];
}

NPGS_INLINE std::uint32_t FVulkanCore::GetGraphicsQueueFamilyIndex() const
{
    return _GraphicsQueueFamilyIndex;
}

NPGS_INLINE std::uint32_t FVulkanCore::GetPresentQueueFamilyIndex() const
{
    return _PresentQueueFamilyIndex;
}

NPGS_INLINE std::uint32_t FVulkanCore::GetComputeQueueFamilyIndex() const
{
    return _ComputeQueueFamilyIndex;
}

NPGS_INLINE std::uint32_t FVulkanCore::GetCurrentImageIndex() const
{
    return _CurrentImageIndex;
}

NPGS_INLINE std::uint32_t FVulkanCore::GetApiVersion() const
{
    return _ApiVersion;
}

#undef CompareCallback

_GRAPHICS_END
_RUNTIME_END
_NPGS_END

