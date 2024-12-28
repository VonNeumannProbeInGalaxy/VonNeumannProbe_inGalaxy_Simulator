#include "VulkanBase.h"

#include <algorithm>
#include <limits>
#include <string>

#include "Engine/Core/Vulkan/VulkanExtFunctionsImpl.h"
#include "Engine/Utilities/Utilities.h"

#ifdef _DEBUG
#define ENABLE_CONSOLE_LOGGER
#endif // _DEBUG
#include "Engine/Utilities/Logger.h"

_NPGS_BEGIN

vk::Result FVulkanBase::CheckInstanceLayers()
{
	std::vector<vk::LayerProperties> AvailableLayers;
	try
	{
		AvailableLayers = vk::enumerateInstanceLayerProperties();
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Error: Failed to enumerate instance layer properties: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	// 移除所有不可用的层
	if (AvailableLayers.empty())
	{
		_InstanceLayers.clear();
		return vk::Result::eSuccess;
	}

	for (const char* RequestedLayer : _InstanceLayers)
	{
		bool bLayerFound = false;
		for (const auto& AvailableLayer : AvailableLayers)
		{
			if (Util::Equal(RequestedLayer, AvailableLayer.layerName))
			{
				bLayerFound = true;
				break;
			}
		}
		if (!bLayerFound)
		{
			RequestedLayer = nullptr;
		}
	}

	_InstanceLayers.erase(std::remove_if(_InstanceLayers.begin(), _InstanceLayers.end(),
						  [](const char* Layer) -> bool { return Layer == nullptr; }),
						  _InstanceLayers.end());
	return vk::Result::eSuccess;
}

vk::Result FVulkanBase::CheckInstanceExtensions(const std::string& LayerName)
{
	std::vector<vk::ExtensionProperties> AvailableExtensions;
	try
	{
		AvailableExtensions = vk::enumerateInstanceExtensionProperties(LayerName);
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Error: Failed to enumerate instance extension properties: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	// 移除所有不可用的扩展
	if (AvailableExtensions.empty())
	{
		_InstanceExtensions.clear();
		return vk::Result::eSuccess;
	}

	for (const char* RequestedExtension : _InstanceExtensions)
	{
		bool bExtensionFound = false;
		for (const auto& AvailableExtension : AvailableExtensions)
		{
			if (Util::Equal(RequestedExtension, AvailableExtension.extensionName))
			{
				bExtensionFound = true;
				break;
			}
		}
		if (!bExtensionFound)
		{
			RequestedExtension = nullptr;
		}
	}

	_InstanceExtensions.erase(std::remove_if(_InstanceExtensions.begin(), _InstanceExtensions.end(),
							  [](const char* Layer) -> bool { return Layer == nullptr; }),
							  _InstanceExtensions.end());
	return vk::Result::eSuccess;
}

vk::Result FVulkanBase::CheckDeviceExtensions()
{
	std::vector<vk::ExtensionProperties> AvailableExtensions;
	try
	{
		AvailableExtensions = _PhysicalDevice.enumerateDeviceExtensionProperties();
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Error: Failed to enumerate device extension properties: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	// 移除所有不可用的扩展
	if (AvailableExtensions.empty())
	{
		_DeviceExtensions.clear();
		return vk::Result::eSuccess;
	}

	for (const char* RequestedExtension : _DeviceExtensions)
	{
		bool bExtensionFound = false;
		for (const auto& AvailableExtension : AvailableExtensions)
		{
			if (Util::Equal(RequestedExtension, AvailableExtension.extensionName))
			{
				bExtensionFound = true;
				break;
			}
		}
		if (!bExtensionFound)
		{
			RequestedExtension = nullptr;
		}
	}

	_DeviceExtensions.erase(std::remove_if(_DeviceExtensions.begin(), _DeviceExtensions.end(),
							[](const char* Layer) -> bool { return Layer == nullptr; }),
							_DeviceExtensions.end());
	return vk::Result::eSuccess;
}

vk::Result FVulkanBase::CreateInstance(const vk::InstanceCreateFlags& Flags)
{
#ifdef _DEBUG
	AddInstanceLayer("VK_LAYER_KHRONOS_validation");
	AddInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif // _DEBUG

	vk::ApplicationInfo ApplicationInfo("Von-Neumann in Galaxy Simulator", VK_MAKE_VERSION(1, 0, 0),
										"NpgsEngine", VK_MAKE_VERSION(1, 0, 0), _ApiVersion);
	vk::InstanceCreateInfo InstanceCreateInfo(Flags, &ApplicationInfo, _InstanceLayers, _InstanceExtensions);

	try
	{
		_Instance = vk::createInstance(InstanceCreateInfo);
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Error: Failed to create Vulkan instance: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

#ifdef _DEBUG
	vk::Result Result;
	if ((Result = CreateDebugMessenger()) != vk::Result::eSuccess)
	{
		return Result;
	}
#endif // _DEBUG

	NpgsCoreInfo("[INFO] Vulkan instance created successfully.");
	NpgsCoreInfo("[INFO] Vulkan API version: {}.{}.{}",
				 VK_VERSION_MAJOR(_ApiVersion), VK_VERSION_MINOR(_ApiVersion), VK_VERSION_PATCH(_ApiVersion));

	return vk::Result::eSuccess;
}

vk::Result FVulkanBase::CreateDevice(const vk::DeviceCreateFlags& Flags, std::uint32_t PhysicalDeviceIndex)
{
	EnumeratePhysicalDevices();
	DeterminePhysicalDevice(PhysicalDeviceIndex, true, true);

	float QueuePriority = 1.0f;
	std::vector<vk::DeviceQueueCreateInfo> DeviceQueueCreateInfos;
	DeviceQueueCreateInfos.reserve(3);

	if (_GraphicsQueueFamilyIndex != VK_QUEUE_FAMILY_IGNORED)
	{
		DeviceQueueCreateInfos.emplace_back(vk::DeviceQueueCreateInfo({}, _GraphicsQueueFamilyIndex, 1, &QueuePriority));
	}
	if (_PresentQueueFamilyIndex != VK_QUEUE_FAMILY_IGNORED &&
		_PresentQueueFamilyIndex != _GraphicsQueueFamilyIndex)
	{
		DeviceQueueCreateInfos.emplace_back(vk::DeviceQueueCreateInfo({}, _PresentQueueFamilyIndex, 1, &QueuePriority));
	}
	if (_ComputeQueueFamilyIndex != VK_QUEUE_FAMILY_IGNORED &&
		_ComputeQueueFamilyIndex != _GraphicsQueueFamilyIndex &&
		_ComputeQueueFamilyIndex != _PresentQueueFamilyIndex)
	{
		DeviceQueueCreateInfos.emplace_back(vk::DeviceQueueCreateInfo({}, _ComputeQueueFamilyIndex, 1, &QueuePriority));
	}

	vk::PhysicalDeviceFeatures PhysicalDeviceFeatures = _PhysicalDevice.getFeatures();
	vk::DeviceCreateInfo DeviceCreateInfo(Flags, DeviceQueueCreateInfos, {}, _DeviceExtensions, &PhysicalDeviceFeatures);

	try
	{
		_Device = _PhysicalDevice.createDevice(DeviceCreateInfo);
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Error: Failed to create logical device: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	// 由于创建的队列族指定了每个队列族只有一个队列，所以这里直接获取第一个队列
	if (_GraphicsQueueFamilyIndex != VK_QUEUE_FAMILY_IGNORED)
	{
		_GraphicsQueue = _Device.getQueue(_GraphicsQueueFamilyIndex, 0);
	}
	if (_PresentQueueFamilyIndex != VK_QUEUE_FAMILY_IGNORED)
	{
		_PresentQueue = _Device.getQueue(_PresentQueueFamilyIndex, 0);
	}
	if (_ComputeQueueFamilyIndex != VK_QUEUE_FAMILY_IGNORED)
	{
		_ComputeQueue = _Device.getQueue(_ComputeQueueFamilyIndex, 0);
	}

	NpgsCoreInfo("[INFO] Logical device created successfully.");

	_PhysicalDeviceProperties       = _PhysicalDevice.getProperties();
	_PhysicalDeviceMemoryProperties = _PhysicalDevice.getMemoryProperties();
	NpgsCoreInfo("[INFO] Renderer: {}", _PhysicalDeviceProperties.deviceName.data());

	for (auto& Callback : _CreateDeviceCallbacks)
	{
		Callback();
	}

	return vk::Result::eSuccess;
}

vk::Result FVulkanBase::RecreateDevice(const vk::DeviceCreateFlags& Flags, std::uint32_t PhysicalDeviceIndex)
{
	WaitIdle();

	if (_Swapchain)
	{
		for (auto& Callback : _DestroySwapchainCallbacks)
		{
			Callback();
		}
		for (auto& ImageView : _SwapchainImageViews)
		{
			if (ImageView)
			{
				_Device.destroyImageView(ImageView);
			}
		}
		_SwapchainImageViews.clear();

		_Device.destroySwapchainKHR(_Swapchain);
		_Swapchain           = vk::SwapchainKHR();
		_SwapchainCreateInfo = vk::SwapchainCreateInfoKHR();
	}

	for (auto& Callback : _DestroyDeviceCallbacks)
	{
		Callback();
	}
	if (_Device)
	{
		_Device.destroy();
		_Device = vk::Device();
	}

	return CreateDevice(Flags, PhysicalDeviceIndex);
}

vk::Result FVulkanBase::SetSurfaceFormat(const vk::SurfaceFormatKHR& SurfaceFormat)
{
	bool bFormatAvailable = false;
	if (SurfaceFormat.format == vk::Format::eUndefined)
	{
		for (const auto& AvailableSurfaceFormat : _AvailableSurfaceFormats)
		{
			if (AvailableSurfaceFormat.colorSpace == SurfaceFormat.colorSpace)
			{
				_SwapchainCreateInfo.setImageFormat(AvailableSurfaceFormat.format);
				_SwapchainCreateInfo.setImageColorSpace(AvailableSurfaceFormat.colorSpace);
				bFormatAvailable = true;
				break;
			}
		}
	}
	else
	{
		for (const auto& AvailableSurfaceFormat : _AvailableSurfaceFormats)
		{
			if (AvailableSurfaceFormat.format     == SurfaceFormat.format &&
				AvailableSurfaceFormat.colorSpace == SurfaceFormat.colorSpace)
			{
				_SwapchainCreateInfo.setImageFormat(AvailableSurfaceFormat.format);
				_SwapchainCreateInfo.setImageColorSpace(AvailableSurfaceFormat.colorSpace);
				bFormatAvailable = true;
				break;
			}
		}
	}

	if (!bFormatAvailable)
	{
		return vk::Result::eErrorFormatNotSupported;
	}

	if (_Swapchain)
	{
		return RecreateSwapchain();
	}

	return vk::Result::eSuccess;
}

vk::Result FVulkanBase::CreateSwapchain(const vk::Extent2D& Extent, const vk::SwapchainCreateFlagsKHR& Flags, bool bLimitFps)
{
	vk::SurfaceCapabilitiesKHR SurfaceCapabilities;
	try
	{
		SurfaceCapabilities = _PhysicalDevice.getSurfaceCapabilitiesKHR(_Surface);
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Error: Failed to get surface capabilities: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	vk::Extent2D SwapchainExtent;
	if (SurfaceCapabilities.currentExtent.width == std::numeric_limits<std::uint32_t>::max())
	{
		SwapchainExtent = vk::Extent2D
		{
			glm::clamp(Extent.width,  SurfaceCapabilities.minImageExtent.width,  SurfaceCapabilities.maxImageExtent.width),
			glm::clamp(Extent.height, SurfaceCapabilities.minImageExtent.height, SurfaceCapabilities.maxImageExtent.height)
		};
	}
	else
	{
		SwapchainExtent = SurfaceCapabilities.currentExtent;
	}

	_SwapchainExtent = SwapchainExtent;
	_SwapchainCreateInfo.setFlags(Flags);
	_SwapchainCreateInfo.setSurface(_Surface);
	_SwapchainCreateInfo.setMinImageCount(
		SurfaceCapabilities.minImageCount + (SurfaceCapabilities.maxImageCount > SurfaceCapabilities.minImageCount));
	_SwapchainCreateInfo.setImageExtent(SwapchainExtent);
	_SwapchainCreateInfo.setImageArrayLayers(1);
	_SwapchainCreateInfo.setImageSharingMode(vk::SharingMode::eExclusive);
	_SwapchainCreateInfo.setPreTransform(SurfaceCapabilities.currentTransform);
	_SwapchainCreateInfo.setClipped(VK_TRUE);

	if (SurfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit) // 优先使用继承模式
	{
		_SwapchainCreateInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eInherit);
	}
	else
	{
		// 找不到集成模式，随便挑一个凑合用的
		const vk::CompositeAlphaFlagBitsKHR CompositeAlphaFlags[]
		{
			vk::CompositeAlphaFlagBitsKHR::eOpaque,
			vk::CompositeAlphaFlagBitsKHR::ePreMultiplied,
			vk::CompositeAlphaFlagBitsKHR::ePostMultiplied
		};

		for (auto CompositeAlpha : CompositeAlphaFlags)
		{
			if (SurfaceCapabilities.supportedCompositeAlpha & CompositeAlpha)
			{
				_SwapchainCreateInfo.setCompositeAlpha(CompositeAlpha);
				break;
			}
		}
	}

	_SwapchainCreateInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
	if (SurfaceCapabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eTransferSrc)
	{
		_SwapchainCreateInfo.setImageUsage(_SwapchainCreateInfo.imageUsage | vk::ImageUsageFlagBits::eTransferSrc);
	}
	if (SurfaceCapabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eTransferDst)
	{
		_SwapchainCreateInfo.setImageUsage(_SwapchainCreateInfo.imageUsage | vk::ImageUsageFlagBits::eTransferDst);
	}
	else
	{
		NpgsCoreError("Error: Failed to get supported usage flags.");
		return static_cast<vk::Result>(VK_RESULT_MAX_ENUM);
	}

	vk::Result Result;
	if (_AvailableSurfaceFormats.empty())
	{
		if ((Result = ObtainPhysicalDeviceSurfaceFormats()) != vk::Result::eSuccess)
		{
			return Result;
		}
	}

	if (_SwapchainCreateInfo.imageFormat == vk::Format::eUndefined)
	{
		if (SetSurfaceFormat({ vk::Format::eR8G8B8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear }) != vk::Result::eSuccess &&
			SetSurfaceFormat({ vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear }) != vk::Result::eSuccess)
		{
			_SwapchainCreateInfo.imageFormat     = _AvailableSurfaceFormats[0].format;
			_SwapchainCreateInfo.imageColorSpace = _AvailableSurfaceFormats[0].colorSpace;
			NpgsCoreWarn("Warning: Failed to select a four-component unsigned normalized surface format.");
		}
	}

	std::vector<vk::PresentModeKHR> SurfacePresentModes;
	try
	{
		SurfacePresentModes = _PhysicalDevice.getSurfacePresentModesKHR(_Surface);
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Error: Failed to get surface present modes: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	_SwapchainCreateInfo.setPresentMode(vk::PresentModeKHR::eMailbox);
	if (!bLimitFps)
	{
		for (const auto& SurfacePresentMode : SurfacePresentModes)
		{
			if (SurfacePresentMode == vk::PresentModeKHR::eMailbox)
			{
				_SwapchainCreateInfo.setPresentMode(vk::PresentModeKHR::eMailbox);
				break;
			}
		}
	}

	if ((Result = CreateSwapchainInternal()) != vk::Result::eSuccess)
	{
		return Result;
	}

	for (auto& Callback : _CreateSwapchainCallbacks)
	{
		Callback();
	}

	NpgsCoreInfo("[INFO] Swapchain created successfully.");
	return vk::Result::eSuccess;
}

vk::Result FVulkanBase::RecreateSwapchain()
{
	vk::SurfaceCapabilitiesKHR SurfaceCapabilities;
	try
	{
		SurfaceCapabilities = _PhysicalDevice.getSurfaceCapabilitiesKHR(_Surface);
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Error: Failed to get surface capabilities: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	if (SurfaceCapabilities.currentExtent.width == 0 || SurfaceCapabilities.currentExtent.height == 0)
	{
		return vk::Result::eSuboptimalKHR;
	}

	_SwapchainCreateInfo.setImageExtent(SurfaceCapabilities.currentExtent);
	_SwapchainCreateInfo.setOldSwapchain(_Swapchain);

	try
	{
		_GraphicsQueue.waitIdle();
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Error: Failed to wait for graphics queue to be idle: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	if (_GraphicsQueueFamilyIndex != _PresentQueueFamilyIndex)
	{
		try
		{
			_PresentQueue.waitIdle();
		}
		catch (const vk::SystemError& Error)
		{
			NpgsCoreError("Error: Failed to wait for present queue to be idle: {}", Error.what());
			return static_cast<vk::Result>(Error.code().value());
		}
	}

	for (auto& Callback : _DestroySwapchainCallbacks)
	{
		Callback();
	}

	for (auto& ImageView : _SwapchainImageViews)
	{
		if (ImageView)
		{
			_Device.destroyImageView(ImageView);
		}
	}
	_SwapchainImageViews.clear();

	vk::Result Result;
	if ((Result = CreateSwapchainInternal()) != vk::Result::eSuccess)
	{
		return Result;
	}

	for (auto& Callback : _CreateSwapchainCallbacks)
	{
		Callback();
	}

	return Result;
}

vk::Result FVulkanBase::WaitIdle()
{
	try
	{
		_Device.waitIdle();
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Error: Failed to wait for device to be idle: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	return vk::Result::eSuccess;
}

FVulkanBase* FVulkanBase::GetVulkanBaseInstance()
{
	static FVulkanBase Instance;
	return &Instance;
}

FVulkanBase::FVulkanBase()
	:
	_GraphicsQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED),
	_PresentQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED),
	_ComputeQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED),
	_ApiVersion(VK_API_VERSION_1_3)
{
	UseLatestApiVersion();
}

FVulkanBase::~FVulkanBase()
{
}

vk::Result FVulkanBase::UseLatestApiVersion()
{
	// 首先检查vkEnumerateInstanceVersion函数是否存在
	auto pfnVkEnumerateInstanceVersion =
		reinterpret_cast<PFN_vkEnumerateInstanceVersion>(
			vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion"));

	if (pfnVkEnumerateInstanceVersion)
	{
		// 如果函数存在(Vulkan 1.1+),使用它获取最新版本
		uint32_t apiVersion;
		if (pfnVkEnumerateInstanceVersion(&apiVersion) == VK_SUCCESS)
		{
			_ApiVersion = apiVersion;
		}
		else
		{
			NpgsCoreError("Error: Failed to get the latest Vulkan API version");
			return vk::Result::eErrorInitializationFailed;
		}
	}
	else
	{
		_ApiVersion = VK_API_VERSION_1_0;
		NpgsCoreInfo("Vulkan 1.1+ not available, using Vulkan 1.0");
	}

	return vk::Result::eSuccess;
}

void FVulkanBase::AddElementChecked(std::vector<const char*>& Vector, const char* Element)
{
	auto it = std::find_if(Vector.begin(), Vector.end(),
	[&Element](const char* ElementInVector) -> bool
	{
		return Util::Equal(Element, ElementInVector);
	});

	if (it == Vector.end())
	{
		Vector.emplace_back(Element);
	}
}

vk::Result FVulkanBase::CreateDebugMessenger()
{
	PFN_vkDebugUtilsMessengerCallbackEXT DebugCallback =
	[](VkDebugUtilsMessageSeverityFlagBitsEXT      MessageSeverity,
	   VkDebugUtilsMessageTypeFlagsEXT             MessageType,
	   const VkDebugUtilsMessengerCallbackDataEXT* CallbackData,
	   void* UserData) -> VkBool32
	{
		std::string Severity;
		if (MessageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) Severity = "VERBOSE";
		if (MessageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)    Severity = "INFO";
		if (MessageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) Severity = "WARNING";
		if (MessageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)   Severity = "ERROR";

		if (Severity == "VERBOSE")
			NpgsCoreTrace("[{}] Validation layer: {}", Severity, CallbackData->pMessage);
		else if (Severity == "INFO")
			NpgsCoreInfo("[{}] Validation layer: {}", Severity, CallbackData->pMessage);
		else if (Severity == "ERROR")
			NpgsCoreError("[{}] Validation layer: {}", Severity, CallbackData->pMessage);
		else if (Severity == "WARNING")
			NpgsCoreWarn("[{}] Validation layer: {}", Severity, CallbackData->pMessage);

		// if (CallbackData->queueLabelCount > 0)
		// 	NpgsCoreTrace("Queue Labels: {}", CallbackData->queueLabelCount);
		// if (CallbackData->cmdBufLabelCount > 0)
		// 	NpgsCoreTrace("Command Buffer Labels: {}", CallbackData->cmdBufLabelCount);
		// if (CallbackData->objectCount > 0)
		// 	NpgsCoreTrace("Objects: {}", CallbackData->objectCount);

		return VK_FALSE;
	};

	auto MessageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
						   vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo    |
						   vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
						   vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
	auto MessageType     = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral     |
						   vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation  |
						   vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;

	vk::DebugUtilsMessengerCreateInfoEXT DebugUtilsMessengerCreateInfo({}, MessageSeverity, MessageType, DebugCallback);

	kVkCreateDebugUtilsMessengerExt =
		reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(_Instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
	if (vkCreateDebugUtilsMessengerEXT == nullptr)
	{
		NpgsCoreError("Error: Failed to get vkCreateDebugUtilsMessengerEXT function pointer.");
		return static_cast<vk::Result>(VK_RESULT_MAX_ENUM);
	}

	kVkDestroyDebugUtilsMessengerExt =
		reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(_Instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
	if (vkDestroyDebugUtilsMessengerEXT == nullptr)
	{
		NpgsCoreError("Error: Failed to get vkDestroyDebugUtilsMessengerEXT function pointer.");
		return static_cast<vk::Result>(VK_RESULT_MAX_ENUM);
	}

	try
	{
		_DebugMessenger = _Instance.createDebugUtilsMessengerEXT(DebugUtilsMessengerCreateInfo);
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Error: Failed to create debug messenger: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	NpgsCoreInfo("[INFO] Debug messenger created successfully.");
	return vk::Result::eSuccess;
}

vk::Result FVulkanBase::EnumeratePhysicalDevices()
{
	try
	{
		_AvailablePhysicalDevices = _Instance.enumeratePhysicalDevices();
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Error: Failed to enumerate physical devices: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	NpgsCoreInfo("[INFO] Physical devices enumerated successfully.");
	return vk::Result::eSuccess;
}

vk::Result FVulkanBase::DeterminePhysicalDevice(std::uint32_t DeviceIndex, bool bEnableGraphicsQueue, bool bEnableComputeQueue)
{
	// kNotFound 在与其进行与运算最高位是 0 的数结果还是数本身
	// 但是对于 -1U，结果是 kNotFound
	static constexpr std::uint32_t kNotFound = std::numeric_limits<std::int32_t>::max();
	std::vector<FQueueFamilyIndexComplex> QueueFamilyIndexComplexes(_AvailablePhysicalDevices.size());
	auto& [GraphicsIndex, PresentIndex, ComputeIndex] = QueueFamilyIndexComplexes[DeviceIndex];

	// 如果任何索引已经搜索过但还是找不到，直接报错
	if ((GraphicsIndex == kNotFound && bEnableGraphicsQueue) ||
		(PresentIndex  == kNotFound && _Surface) ||
		(ComputeIndex  == kNotFound && bEnableGraphicsQueue))
	{
		return static_cast<vk::Result>(VK_RESULT_MAX_ENUM);
	}

	if ((GraphicsIndex == VK_QUEUE_FAMILY_IGNORED && bEnableGraphicsQueue) ||
		(PresentIndex  == VK_QUEUE_FAMILY_IGNORED && _Surface) ||
		(ComputeIndex  == VK_QUEUE_FAMILY_IGNORED && bEnableGraphicsQueue))
	{
		FQueueFamilyIndexComplex Indices;
		vk::Result Result = ObtainQueueFamilyIndices(_AvailablePhysicalDevices[DeviceIndex],
													 bEnableGraphicsQueue, bEnableComputeQueue, Indices);
		if (Result == vk::Result::eSuccess || Result == static_cast<vk::Result>(VK_RESULT_MAX_ENUM))
		{
			if (bEnableGraphicsQueue)
			{
				GraphicsIndex = Indices.GraphicsIndex & kNotFound;
			}
			if (_Surface)
			{
				PresentIndex = Indices.PresentIndex & kNotFound;
			}
			if (bEnableComputeQueue)
			{
				ComputeIndex = Indices.ComputeIndex & kNotFound;
			}

			_GraphicsQueueFamilyIndex = bEnableGraphicsQueue ? GraphicsIndex : VK_QUEUE_FAMILY_IGNORED;
			_PresentQueueFamilyIndex  = _Surface             ? PresentIndex  : VK_QUEUE_FAMILY_IGNORED;
			_ComputeQueueFamilyIndex  = bEnableComputeQueue  ? ComputeIndex  : VK_QUEUE_FAMILY_IGNORED;
		}

		if (Result != vk::Result::eSuccess)
		{
			return Result;
		}
	}
	else
	{
		_GraphicsQueueFamilyIndex = bEnableGraphicsQueue ? GraphicsIndex : VK_QUEUE_FAMILY_IGNORED;
		_PresentQueueFamilyIndex  = _Surface             ? PresentIndex : VK_QUEUE_FAMILY_IGNORED;
		_ComputeQueueFamilyIndex  = bEnableComputeQueue  ? ComputeIndex : VK_QUEUE_FAMILY_IGNORED;
	}

	_PhysicalDevice = _AvailablePhysicalDevices[DeviceIndex];
	return vk::Result::eSuccess;
}

vk::Result FVulkanBase::ObtainPhysicalDeviceSurfaceFormats()
{
	try
	{
		_AvailableSurfaceFormats = _PhysicalDevice.getSurfaceFormatsKHR(_Surface);
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Error: Failed to obtain surface formats: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	NpgsCoreInfo("[INFO] Surface formats obtained successfully.");
	return vk::Result::eSuccess;
}

vk::Result FVulkanBase::ObtainQueueFamilyIndices(const vk::PhysicalDevice& PhysicalDevice, bool bEnableGraphicsQueue,
												 bool bEnableComputeQueue, FQueueFamilyIndexComplex& Indices)
{
	std::vector<vk::QueueFamilyProperties> QueueFamilyProperties = PhysicalDevice.getQueueFamilyProperties();
	if (QueueFamilyProperties.empty())
	{
		NpgsCoreError("Error: Failed to get queue family properties.");
		return static_cast<vk::Result>(VK_RESULT_MAX_ENUM);
	}

	auto& [GraphicsIndex, PresentIndex, ComputeIndex] = Indices;
	GraphicsIndex = PresentIndex = ComputeIndex = VK_QUEUE_FAMILY_IGNORED;

	// 队列族的位置是和队列族本身的索引是对应的
	for (std::uint32_t i = 0; i != QueueFamilyProperties.size(); ++i)
	{
		vk::Bool32 bSupportGraphics = bEnableGraphicsQueue && QueueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics;
		vk::Bool32 bSupportPresent  = false;
		vk::Bool32 bSupportCompute  = bEnableComputeQueue && QueueFamilyProperties[i].queueFlags  & vk::QueueFlagBits::eCompute;

		if (_Surface)
		{
			try
			{
				bSupportPresent = PhysicalDevice.getSurfaceSupportKHR(i, _Surface);
			}
			catch (const vk::SystemError& Error)
			{
				NpgsCoreError("Error: Failed to determine if the queue family supports presentation: {}", Error.what());
				return static_cast<vk::Result>(Error.code().value());
			}
		}

		if (bSupportGraphics && bSupportCompute) // 如果同时支持图形和计算队列
		{
			if (bSupportPresent)
			{
				GraphicsIndex = PresentIndex = ComputeIndex = i; // 三个队列族都支持，将它们的索引设置为相同的值
				break;
			}
			if (GraphicsIndex != ComputeIndex || GraphicsIndex == VK_QUEUE_FAMILY_IGNORED)
			{
				GraphicsIndex = ComputeIndex = i; // 确保图形和计算队列族的索引相同
			}
			if (!_Surface)
			{
				break; // 如果不需要呈现，那么只需要找到一个支持图形和计算队列族的索引即可
			}
		}

		// 将支持对应功能的队列族的索引设置为找到的第一个值
		if (bSupportGraphics && GraphicsIndex == VK_QUEUE_FAMILY_IGNORED)
		{
			GraphicsIndex = i;
		}
		if (bSupportPresent && PresentIndex == VK_QUEUE_FAMILY_IGNORED)
		{
			PresentIndex = i;
		}
		if (bSupportCompute && ComputeIndex == VK_QUEUE_FAMILY_IGNORED)
		{
			ComputeIndex = i;
		}
	}

	// 如果有任意一个需要启用的队列族的索引是 VK_QUEUE_FAMILY_IGNORED，返回错误
	if ((GraphicsIndex == VK_QUEUE_FAMILY_IGNORED && bEnableGraphicsQueue) ||
		(PresentIndex  == VK_QUEUE_FAMILY_IGNORED && _Surface) ||
		(ComputeIndex  == VK_QUEUE_FAMILY_IGNORED && bEnableComputeQueue))
	{
		NpgsCoreError("Error: Failed to obtain queue family indices.");
		return static_cast<vk::Result>(VK_RESULT_MAX_ENUM);
	}

	NpgsCoreInfo("[INFO] Queue family indices obtained successfully.");
	return vk::Result::eSuccess;
}

vk::Result FVulkanBase::CreateSwapchainInternal()
{
	try
	{
		_Swapchain = _Device.createSwapchainKHR(_SwapchainCreateInfo);
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Error: Failed to create swapchain: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	try
	{
		_SwapchainImages = _Device.getSwapchainImagesKHR(_Swapchain);
	}
	catch (const vk::SystemError& Error)
	{
		NpgsCoreError("Error: Failed to get swapchain images: {}", Error.what());
		return static_cast<vk::Result>(Error.code().value());
	}

	vk::ImageViewCreateInfo ImageViewCreateInfo;
	ImageViewCreateInfo.setViewType(vk::ImageViewType::e2D);
	ImageViewCreateInfo.setFormat(_SwapchainCreateInfo.imageFormat);
	ImageViewCreateInfo.setSubresourceRange({ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });

	for (const auto& SwapchainImage : _SwapchainImages)
	{
		ImageViewCreateInfo.setImage(SwapchainImage);
		vk::ImageView ImageView;
		try
		{
			ImageView = _Device.createImageView(ImageViewCreateInfo);
		}
		catch (const vk::SystemError& Error)
		{
			NpgsCoreError("Error: Failed to create image view: {}", Error.what());
			return static_cast<vk::Result>(Error.code().value());
		}

		_SwapchainImageViews.emplace_back(ImageView);
	}

	return vk::Result::eSuccess;
}

_NPGS_END
