#pragma once

#include <cstdint>
#include <array>
#include <functional>
#include <string>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "Engine/Core/Base.h"

_NPGS_BEGIN

class FVulkanBase
{
private:
	struct FQueueFamilyIndexComplex
	{
		std::uint32_t GraphicsIndex{ VK_QUEUE_FAMILY_IGNORED };
		std::uint32_t PresentIndex{ VK_QUEUE_FAMILY_IGNORED };
		std::uint32_t ComputeIndex{ VK_QUEUE_FAMILY_IGNORED };
	};

public:
	void AddInstanceLayer(const char* Layer);
	void SetInstanceLayers(const std::vector<const char*>& Layers);
	void AddInstanceExtension(const char* Extension);
	void SetInstanceExtensions(const std::vector<const char*>& Extensions);
	void AddDeviceExtension(const char* Extension);
	void SetDeviceExtensions(const std::vector<const char*>& Extensions);
	vk::Result CheckInstanceLayers();
	vk::Result CheckInstanceExtensions(const std::string& LayerName);
	vk::Result CheckDeviceExtensions();

	vk::Result CreateInstance(const vk::InstanceCreateFlags& Flags);
	vk::Result CreateDevice(const vk::DeviceCreateFlags& Flags, std::uint32_t PhysicalDeviceIndex);
	vk::Result RecreateDevice(const vk::DeviceCreateFlags& Flags, std::uint32_t PhysicalDeviceIndex);
	void       SetSurface(const vk::SurfaceKHR& Surface);
	vk::Result SetSurfaceFormat(const vk::SurfaceFormatKHR& SurfaceFormat);
	vk::Result CreateSwapchain(const vk::Extent2D& Extent, const vk::SwapchainCreateFlagsKHR& Flags, bool bLimitFps = true);
	vk::Result RecreateSwapchain();
	vk::Result WaitIdle();

	const std::vector<const char*>& GetInstanceLayers() const;
	const std::vector<const char*>& GetInstanceExtensions() const;
	const std::vector<const char*>& GetDeviceExtensions() const;

	const vk::Instance& GetInstance() const;
	const vk::SurfaceKHR& GetSurface() const;
	const vk::PhysicalDevice& GetPhysicalDevice() const;
	const vk::PhysicalDeviceProperties& GetPhysicalDeviceProperties() const;
	const vk::PhysicalDeviceMemoryProperties& GetPhysicalDeviceMemoryProperties() const;
	const vk::Device& GetDevice() const;
	const vk::Queue& GetGraphicsQueue() const;
	const vk::Queue& GetPresentQueue() const;
	const vk::Queue& GetComputeQueue() const;
	const vk::SwapchainKHR& GetSwapchain() const;
	const vk::SwapchainCreateInfoKHR& GetSwapchainCreateInfo() const;

	std::uint32_t GetAvailablePhysicalDeviceCount() const;
	std::uint32_t GetAvailableSurfaceFormatCount() const;
	std::uint32_t GetSwapchainImageCount() const;
	std::uint32_t GetSwapchainImageViewCount() const;

	const vk::PhysicalDevice& GetAvailablePhysicalDevice(std::uint32_t Index) const;
	const vk::Format& GetAvailableSurfaceFormat(std::uint32_t Index) const;
	const vk::ColorSpaceKHR& GetAvailableSurfaceColorSpace(std::uint32_t Index) const;
	const vk::Image& GetSwapchainImage(std::uint32_t Index) const;
	const vk::ImageView& GetSwapchainImageView(std::uint32_t Index) const;

	std::uint32_t GetGraphicsQueueFamilyIndex() const;
	std::uint32_t GetPresentQueueFamilyIndex() const;
	std::uint32_t GetComputeQueueFamilyIndex() const;

	std::uint32_t GetApiVersion() const;

	static FVulkanBase* GetVulkanBaseInstance();

private:
	explicit FVulkanBase();
	FVulkanBase(const FVulkanBase&&) = delete;
	FVulkanBase(FVulkanBase&&)       = delete;
	~FVulkanBase();

	FVulkanBase& operator=(const FVulkanBase&&) = delete;
	FVulkanBase& operator=(FVulkanBase&&)       = delete;

	vk::Result UseLatestApiVersion();

	void AddElementChecked(std::vector<const char*>& Vector, const char* Element);

	vk::Result CreateDebugMessenger();
	vk::Result EnumeratePhysicalDevices();
	vk::Result DeterminePhysicalDevice(std::uint32_t DeviceIndex, bool bEnableGraphicsQueue, bool bEnableComputeQueue = true);
	vk::Result ObtainPhysicalDeviceSurfaceFormats();

	vk::Result ObtainQueueFamilyIndices(const vk::PhysicalDevice& PhysicalDevice, bool bEnableGraphicsQueue,
										bool bEnableComputeQueue, FQueueFamilyIndexComplex& Indices);

	vk::Result CreateSwapchainInternal();

private:
	std::vector<const char*>           _InstanceLayers;
	std::vector<const char*>           _InstanceExtensions;
	std::vector<const char*>           _DeviceExtensions;
	std::vector<vk::PhysicalDevice>    _AvailablePhysicalDevices;
	std::vector<vk::SurfaceFormatKHR>  _AvailableSurfaceFormats;
	std::vector<vk::Image>             _SwapchainImages;
	std::vector<vk::ImageView>         _SwapchainImageViews;
	std::vector<std::function<void()>> _CreateSwapchainCallbacks;
	std::vector<std::function<void()>> _DestroySwapchainCallbacks;
	std::vector<std::function<void()>> _CreateDeviceCallbacks;
	std::vector<std::function<void()>> _DestroyDeviceCallbacks;

	vk::Instance                       _Instance;
	vk::DebugUtilsMessengerEXT         _DebugMessenger;
	vk::SurfaceKHR                     _Surface;
	vk::PhysicalDevice                 _PhysicalDevice;
	vk::PhysicalDeviceProperties       _PhysicalDeviceProperties;
	vk::PhysicalDeviceMemoryProperties _PhysicalDeviceMemoryProperties;
	vk::Device                         _Device;
	vk::Queue                          _GraphicsQueue;
	vk::Queue                          _PresentQueue;
	vk::Queue                          _ComputeQueue;
	vk::SwapchainKHR                   _Swapchain;
	vk::SwapchainCreateInfoKHR		   _SwapchainCreateInfo;
	vk::Extent2D                       _SwapchainExtent;

	std::uint32_t                      _GraphicsQueueFamilyIndex;
	std::uint32_t                      _PresentQueueFamilyIndex;
	std::uint32_t                      _ComputeQueueFamilyIndex;

	std::uint32_t                      _ApiVersion;
};

_NPGS_END

#include "VulkanBase.inl"
