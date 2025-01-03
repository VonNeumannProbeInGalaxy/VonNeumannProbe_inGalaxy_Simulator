#include "VulkanCore.h"

#include <algorithm>
#include <limits>

#include "Engine/Core/Runtime/Graphics/Vulkan/VulkanExtFunctionsImpl.h"
#include "Engine/Utils/Logger.h"
#include "Engine/Utils/Utils.h"

_NPGS_BEGIN
_RUNTIME_BEGIN
_GRAPHICS_BEGIN

FVulkanCore::FVulkanCore()
    :
    _GraphicsQueueFamilyIndex(vk::QueueFamilyIgnored),
    _PresentQueueFamilyIndex(vk::QueueFamilyIgnored),
    _ComputeQueueFamilyIndex(vk::QueueFamilyIgnored),
    _CurrentImageIndex(std::numeric_limits<std::uint32_t>::max()),
    _ApiVersion(VK_API_VERSION_1_3)
{
    UseLatestApiVersion();
}

FVulkanCore::~FVulkanCore()
{
    if (_Instance)
    {
        if (_Device)
        {
            WaitIdle();
            if (_Swapchain)
            {
                for (auto& Callback : _DestroySwapchainCallbacks)
                {
                    Callback.second();
                }
                for (auto& ImageView : _SwapchainImageViews)
                {
                    if (ImageView)
                    {
                        _Device.destroyImageView(ImageView);
                    }
                }
                _SwapchainImageViews.clear();
                NpgsCoreInfo("Destroyed image views.");
                _Device.destroySwapchainKHR(_Swapchain);
                NpgsCoreInfo("Destroyed swapchain.");
            }

            for (auto& Callback : _DestroyDeviceCallbacks)
            {
                Callback.second();
            }
            _Device.destroy();
            NpgsCoreInfo("Destroyed logical device.");
        }

        if (_Surface)
        {
            _Instance.destroySurfaceKHR(_Surface);
            NpgsCoreInfo("Destroyed surface.");
        }

        if (_DebugMessenger)
        {
            _Instance.destroyDebugUtilsMessengerEXT(_DebugMessenger);
            NpgsCoreInfo("Destroyed debug messenger.");
        }

        _CreateSwapchainCallbacks.clear();
        _DestroySwapchainCallbacks.clear();
        _CreateDeviceCallbacks.clear();
        _DestroyDeviceCallbacks.clear();

        _DebugMessenger      = vk::DebugUtilsMessengerEXT();
        _Surface             = vk::SurfaceKHR();
        _PhysicalDevice      = vk::PhysicalDevice();
        _Device              = vk::Device();
        _Swapchain           = vk::SwapchainKHR();
        _SwapchainCreateInfo = vk::SwapchainCreateInfoKHR();

        _Instance.destroy();
        NpgsCoreInfo("Destroyed Vulkan instance.");
    }
}

vk::Result FVulkanCore::CheckInstanceLayers()
{
    std::vector<vk::LayerProperties> AvailableLayers;
    try
    {
        AvailableLayers = vk::enumerateInstanceLayerProperties();
    }
    catch (const vk::SystemError& Error)
    {
        NpgsCoreError("Failed to enumerate instance layer properties: {}", Error.what());
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

vk::Result FVulkanCore::CheckInstanceExtensions(const std::string& LayerName)
{
    std::vector<vk::ExtensionProperties> AvailableExtensions;
    try
    {
        AvailableExtensions = vk::enumerateInstanceExtensionProperties(LayerName);
    }
    catch (const vk::SystemError& Error)
    {
        NpgsCoreError("Failed to enumerate instance extension properties: {}", Error.what());
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

vk::Result FVulkanCore::CheckDeviceExtensions()
{
    std::vector<vk::ExtensionProperties> AvailableExtensions;
    try
    {
        AvailableExtensions = _PhysicalDevice.enumerateDeviceExtensionProperties();
    }
    catch (const vk::SystemError& Error)
    {
        NpgsCoreError("Failed to enumerate device extension properties: {}", Error.what());
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

vk::Result FVulkanCore::CreateInstance(vk::InstanceCreateFlags Flags)
{
#ifdef _DEBUG
    AddInstanceLayer("VK_LAYER_KHRONOS_validation");
    AddInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif // _DEBUG

    vk::ApplicationInfo ApplicationInfo("Von-Neumann in Galaxy Simulator", VK_MAKE_VERSION(1, 0, 0),
                                        "No Engine", VK_MAKE_VERSION(1, 0, 0), _ApiVersion);
    vk::InstanceCreateInfo InstanceCreateInfo(Flags, &ApplicationInfo, _InstanceLayers, _InstanceExtensions);

    try
    {
        _Instance = vk::createInstance(InstanceCreateInfo);
    }
    catch (const vk::SystemError& Error)
    {
        NpgsCoreError("Failed to create Vulkan instance: {}", Error.what());
        return static_cast<vk::Result>(Error.code().value());
    }

#ifdef _DEBUG
    vk::Result Result;
    if ((Result = CreateDebugMessenger()) != vk::Result::eSuccess)
    {
        return Result;
    }
#endif // _DEBUG

    NpgsCoreInfo("Vulkan instance created successfully.");
    NpgsCoreInfo("Vulkan API version: {}.{}.{}",
                 VK_VERSION_MAJOR(_ApiVersion), VK_VERSION_MINOR(_ApiVersion), VK_VERSION_PATCH(_ApiVersion));

    return vk::Result::eSuccess;
}

vk::Result FVulkanCore::CreateDevice(std::uint32_t PhysicalDeviceIndex, vk::DeviceCreateFlags Flags)
{
    EnumeratePhysicalDevices();
    DeterminePhysicalDevice(PhysicalDeviceIndex, true, true);

    float QueuePriority = 1.0f;
    std::vector<vk::DeviceQueueCreateInfo> DeviceQueueCreateInfos;
    DeviceQueueCreateInfos.reserve(3);

    if (_GraphicsQueueFamilyIndex != vk::QueueFamilyIgnored)
    {
        DeviceQueueCreateInfos.emplace_back(vk::DeviceQueueCreateInfo({}, _GraphicsQueueFamilyIndex, 1, &QueuePriority));
    }
    if (_PresentQueueFamilyIndex != vk::QueueFamilyIgnored &&
        _PresentQueueFamilyIndex != _GraphicsQueueFamilyIndex)
    {
        DeviceQueueCreateInfos.emplace_back(vk::DeviceQueueCreateInfo({}, _PresentQueueFamilyIndex, 1, &QueuePriority));
    }
    if (_ComputeQueueFamilyIndex != vk::QueueFamilyIgnored &&
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
        NpgsCoreError("Failed to create logical device: {}", Error.what());
        return static_cast<vk::Result>(Error.code().value());
    }

    // 由于创建的队列族指定了每个队列族只有一个队列，所以这里直接获取第一个队列
    if (_GraphicsQueueFamilyIndex != vk::QueueFamilyIgnored)
    {
        _GraphicsQueue = _Device.getQueue(_GraphicsQueueFamilyIndex, 0);
    }
    if (_PresentQueueFamilyIndex != vk::QueueFamilyIgnored)
    {
        _PresentQueue = _Device.getQueue(_PresentQueueFamilyIndex, 0);
    }
    if (_ComputeQueueFamilyIndex != vk::QueueFamilyIgnored)
    {
        _ComputeQueue = _Device.getQueue(_ComputeQueueFamilyIndex, 0);
    }

    NpgsCoreInfo("Logical device created successfully.");

    _PhysicalDeviceProperties       = _PhysicalDevice.getProperties();
    _PhysicalDeviceMemoryProperties = _PhysicalDevice.getMemoryProperties();
    NpgsCoreInfo("Renderer: {}", _PhysicalDeviceProperties.deviceName.data());

    for (auto& Callback : _CreateDeviceCallbacks)
    {
        Callback.second();
    }

    return vk::Result::eSuccess;
}

vk::Result FVulkanCore::RecreateDevice(std::uint32_t PhysicalDeviceIndex, vk::DeviceCreateFlags Flags)
{
    WaitIdle();

    if (_Swapchain)
    {
        for (auto& Callback : _DestroySwapchainCallbacks)
        {
            Callback.second();
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
        Callback.second();
    }
    if (_Device)
    {
        _Device.destroy();
        _Device = vk::Device();
    }

    return CreateDevice(PhysicalDeviceIndex, Flags);
}

vk::Result FVulkanCore::SetSurfaceFormat(const vk::SurfaceFormatKHR& SurfaceFormat)
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

vk::Result FVulkanCore::CreateSwapchain(const vk::Extent2D& Extent, bool bLimitFps, vk::SwapchainCreateFlagsKHR Flags)
{
    // Swapchain 需要的信息：
    // 1.基本 Surface 能力（Swapchain 中图像的最小/最大数量、宽度和高度）
    vk::SurfaceCapabilitiesKHR SurfaceCapabilities;
    try
    {
        SurfaceCapabilities = _PhysicalDevice.getSurfaceCapabilitiesKHR(_Surface);
    }
    catch (const vk::SystemError& Error)
    {
        NpgsCoreError("Failed to get surface capabilities: {}", Error.what());
        return static_cast<vk::Result>(Error.code().value());
    }

    vk::Extent2D SwapchainExtent;
    if (SurfaceCapabilities.currentExtent.width == std::numeric_limits<std::uint32_t>::max())
    {
        SwapchainExtent = vk::Extent2D
        {
            // 限制 Swapchain 的大小在支持的范围内
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
    _SwapchainCreateInfo.setClipped(vk::True);

    // 设置图像格式
    if (SurfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit) // 优先使用继承模式
    {
        _SwapchainCreateInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eInherit);
    }
    else
    {
        // 找不到继承模式，随便挑一个凑合用的
        static const vk::CompositeAlphaFlagBitsKHR kCompositeAlphaFlags[]
        {
            vk::CompositeAlphaFlagBitsKHR::eOpaque,
            vk::CompositeAlphaFlagBitsKHR::ePreMultiplied,
            vk::CompositeAlphaFlagBitsKHR::ePostMultiplied
        };

        for (auto CompositeAlphaFlag : kCompositeAlphaFlags)
        {
            if (SurfaceCapabilities.supportedCompositeAlpha & CompositeAlphaFlag)
            {
                _SwapchainCreateInfo.setCompositeAlpha(CompositeAlphaFlag);
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
        NpgsCoreError("Failed to get supported usage flags.");
        return static_cast<vk::Result>(VK_RESULT_MAX_ENUM);
    }

    // 2.设置 Swapchain 像素格式和色彩空间
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
            _SwapchainCreateInfo.setImageFormat(_AvailableSurfaceFormats[0].format);
            _SwapchainCreateInfo.setImageColorSpace(_AvailableSurfaceFormats[0].colorSpace);
            NpgsCoreWarn("Failed to select a four-component unsigned normalized surface format.");
        }
    }

    // 3.设置 Swapchain 呈现模式
    std::vector<vk::PresentModeKHR> SurfacePresentModes;
    try
    {
        SurfacePresentModes = _PhysicalDevice.getSurfacePresentModesKHR(_Surface);
    }
    catch (const vk::SystemError& Error)
    {
        NpgsCoreError("Failed to get surface present modes: {}", Error.what());
        return static_cast<vk::Result>(Error.code().value());
    }

    if (bLimitFps)
    {
        _SwapchainCreateInfo.setPresentMode(vk::PresentModeKHR::eFifo);
    }
    else
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
        Callback.second();
    }

    NpgsCoreInfo("Swapchain created successfully.");
    return vk::Result::eSuccess;
}

vk::Result FVulkanCore::RecreateSwapchain()
{
    vk::SurfaceCapabilitiesKHR SurfaceCapabilities;
    try
    {
        SurfaceCapabilities = _PhysicalDevice.getSurfaceCapabilitiesKHR(_Surface);
    }
    catch (const vk::SystemError& Error)
    {
        NpgsCoreError("Failed to get surface capabilities: {}", Error.what());
        return static_cast<vk::Result>(Error.code().value());
    }

    if (SurfaceCapabilities.currentExtent.width == 0 || SurfaceCapabilities.currentExtent.height == 0)
    {
        return vk::Result::eSuboptimalKHR;
    }
    _SwapchainCreateInfo.setImageExtent(SurfaceCapabilities.currentExtent);

    if (_SwapchainCreateInfo.oldSwapchain)
    {
        _Device.destroySwapchainKHR(_SwapchainCreateInfo.oldSwapchain);
    }
    _SwapchainCreateInfo.setOldSwapchain(_Swapchain);

    try
    {
        _GraphicsQueue.waitIdle();
    }
    catch (const vk::SystemError& Error)
    {
        NpgsCoreError("Failed to wait for graphics queue to be idle: {}", Error.what());
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
            NpgsCoreError("Failed to wait for present queue to be idle: {}", Error.what());
            return static_cast<vk::Result>(Error.code().value());
        }
    }

    for (auto& Callback : _DestroySwapchainCallbacks)
    {
        Callback.second();
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
        Callback.second();
    }

    NpgsCoreInfo("Swapchain recreated successfully.");
    return vk::Result::eSuccess;
}

vk::Result FVulkanCore::SwapImage(const FVulkanSemaphore& Semaphore)
{
    if (_SwapchainCreateInfo.oldSwapchain &&
        _SwapchainCreateInfo.oldSwapchain != _Swapchain) [[unlikely]]
    {
        _Device.destroySwapchainKHR(_SwapchainCreateInfo.oldSwapchain);
        _SwapchainCreateInfo.setOldSwapchain(vk::SwapchainKHR());
    }

    vk::Result Result;
    while ((Result = _Device.acquireNextImageKHR(_Swapchain, std::numeric_limits<std::uint64_t>::max(),
           *Semaphore, vk::Fence(), &_CurrentImageIndex)) != vk::Result::eSuccess)
    {
        switch (Result)
        {
        case vk::Result::eSuboptimalKHR:
        case vk::Result::eErrorOutOfDateKHR:
            if ((Result = RecreateSwapchain()) != vk::Result::eSuccess)
            {
                return Result;
            }
            break;
        default:
            NpgsCoreError("Failed to acquire next image: {}.", vk::to_string(Result));
            return Result;
        }
    }

    return vk::Result::eSuccess;
}

vk::Result FVulkanCore::PresentImage(const vk::PresentInfoKHR& PresentInfo)
{
    try
    {
        vk::Result Result = _PresentQueue.presentKHR(PresentInfo);
        switch (Result)
        {
        case vk::Result::eSuccess:
            return vk::Result::eSuccess;
        case vk::Result::eSuboptimalKHR:
            return RecreateSwapchain();
        default:
            NpgsCoreError("Failed to present image: {}.", vk::to_string(Result));
            return Result;
        }
    }
    catch (const vk::SystemError& Error)
    {
        vk::Result ErrorResult = static_cast<vk::Result>(Error.code().value());
        switch (ErrorResult)
        {
        case vk::Result::eErrorOutOfDateKHR:
            return RecreateSwapchain();
        default:
            NpgsCoreError("Failed to present image: {}", Error.what());
            return ErrorResult;
        }
    }
}

vk::Result FVulkanCore::PresentImage(const FVulkanSemaphore& Semaphore)
{
    vk::PresentInfoKHR PresentInfo;
    PresentInfo.setSwapchainCount(1);
    PresentInfo.setSwapchains(_Swapchain);
    PresentInfo.setImageIndices(_CurrentImageIndex);

    if (Semaphore)
    {
        PresentInfo.setWaitSemaphores(*Semaphore);
    }

    return PresentImage(PresentInfo);
}

vk::Result FVulkanCore::WaitIdle() const
{
    try
    {
        _Device.waitIdle();
    }
    catch (const vk::SystemError& Error)
    {
        NpgsCoreError("Failed to wait for device to be idle: {}", Error.what());
        return static_cast<vk::Result>(Error.code().value());
    }

    return vk::Result::eSuccess;
}

vk::Result FVulkanCore::UseLatestApiVersion()
{
    if (reinterpret_cast<PFN_vkEnumerateInstanceVersion>(vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion")))
    {
        try
        {
            _ApiVersion = vk::enumerateInstanceVersion();
        }
        catch (const vk::SystemError& Error)
        {
            NpgsCoreError("Failed to get the latest Vulkan API version: {}", Error.what());
            return static_cast<vk::Result>(Error.code().value());
        }
    }
    else
    {
        _ApiVersion = VK_API_VERSION_1_0;
        NpgsCoreInfo("Vulkan 1.1+ not available, using Vulkan 1.0");
    }

    return vk::Result::eSuccess;
}

void FVulkanCore::AddElementChecked(std::vector<const char*>& Vector, const char* Element)
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

vk::Result FVulkanCore::CreateDebugMessenger()
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
            NpgsCoreTrace("Validation layer: {}", CallbackData->pMessage);
        else if (Severity == "INFO")
            NpgsCoreInfo("Validation layer: {}", CallbackData->pMessage);
        else if (Severity == "ERROR")
            NpgsCoreError("Validation layer: {}", CallbackData->pMessage);
        else if (Severity == "WARNING")
            NpgsCoreWarn("Validation layer: {}", CallbackData->pMessage);

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
        NpgsCoreError("Failed to get vkCreateDebugUtilsMessengerEXT function pointer.");
        return static_cast<vk::Result>(VK_RESULT_MAX_ENUM);
    }

    kVkDestroyDebugUtilsMessengerExt =
        reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(_Instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
    if (vkDestroyDebugUtilsMessengerEXT == nullptr)
    {
        NpgsCoreError("Failed to get vkDestroyDebugUtilsMessengerEXT function pointer.");
        return static_cast<vk::Result>(VK_RESULT_MAX_ENUM);
    }

    try
    {
        _DebugMessenger = _Instance.createDebugUtilsMessengerEXT(DebugUtilsMessengerCreateInfo);
    }
    catch (const vk::SystemError& Error)
    {
        NpgsCoreError("Failed to create debug messenger: {}", Error.what());
        return static_cast<vk::Result>(Error.code().value());
    }

    NpgsCoreInfo("Debug messenger created successfully.");
    return vk::Result::eSuccess;
}

vk::Result FVulkanCore::EnumeratePhysicalDevices()
{
    try
    {
        _AvailablePhysicalDevices = _Instance.enumeratePhysicalDevices();
    }
    catch (const vk::SystemError& Error)
    {
        NpgsCoreError("Failed to enumerate physical devices: {}", Error.what());
        return static_cast<vk::Result>(Error.code().value());
    }

    NpgsCoreInfo("Physical devices enumerated successfully.");
    return vk::Result::eSuccess;
}

vk::Result FVulkanCore::DeterminePhysicalDevice(std::uint32_t DeviceIndex, bool bEnableGraphicsQueue, bool bEnableComputeQueue)
{
    // kNotFound 在与其进行与运算最高位是 0 的数结果还是数本身
    // 但是对于 -1U，结果是 kNotFound
    static constexpr std::uint32_t kNotFound = std::numeric_limits<std::int32_t>::max();
    std::vector<FQueueFamilyIndicesComplex> QueueFamilyIndeicesComplexes(_AvailablePhysicalDevices.size());
    auto [GraphicsQueueFamilyIndex, PresentQueueFamilyIndex, ComputeQueueFamilyIndex] = QueueFamilyIndeicesComplexes[DeviceIndex];

    // 如果任何索引已经搜索过但还是找不到，直接报错
    if ((GraphicsQueueFamilyIndex == kNotFound && bEnableGraphicsQueue) ||
        (PresentQueueFamilyIndex  == kNotFound && _Surface) ||
        (ComputeQueueFamilyIndex  == kNotFound && bEnableGraphicsQueue))
    {
        return static_cast<vk::Result>(VK_RESULT_MAX_ENUM);
    }

    if ((GraphicsQueueFamilyIndex == vk::QueueFamilyIgnored && bEnableGraphicsQueue) ||
        (PresentQueueFamilyIndex  == vk::QueueFamilyIgnored && _Surface) ||
        (ComputeQueueFamilyIndex  == vk::QueueFamilyIgnored && bEnableGraphicsQueue))
    {
        FQueueFamilyIndicesComplex Indices;
        vk::Result Result = ObtainQueueFamilyIndices(_AvailablePhysicalDevices[DeviceIndex],
                                                     bEnableGraphicsQueue, bEnableComputeQueue, Indices);
        if (Result == vk::Result::eSuccess || Result == static_cast<vk::Result>(VK_RESULT_MAX_ENUM))
        { // 如果结果是 vk::QueueFamilyIgnored，那么和 kNotFound 做与运算结果还是 kNotFound
            if (bEnableGraphicsQueue)
            {
                GraphicsQueueFamilyIndex = Indices.GraphicsQueueFamilyIndex & kNotFound;
            }
            if (_Surface)
            {
                PresentQueueFamilyIndex = Indices.PresentQueueFamilyIndex & kNotFound;
            }
            if (bEnableComputeQueue)
            {
                ComputeQueueFamilyIndex = Indices.ComputeQueueFamilyIndex & kNotFound;
            }

            _GraphicsQueueFamilyIndex = bEnableGraphicsQueue ? GraphicsQueueFamilyIndex : vk::QueueFamilyIgnored;
            _PresentQueueFamilyIndex  = _Surface             ? PresentQueueFamilyIndex  : vk::QueueFamilyIgnored;
            _ComputeQueueFamilyIndex  = bEnableComputeQueue  ? ComputeQueueFamilyIndex  : vk::QueueFamilyIgnored;
        }

        if (Result != vk::Result::eSuccess)
        {
            return Result;
        }
    }
    else // 如果已经找到了，直接设置索引
    {
        _GraphicsQueueFamilyIndex = bEnableGraphicsQueue ? GraphicsQueueFamilyIndex : vk::QueueFamilyIgnored;
        _PresentQueueFamilyIndex  = _Surface             ? PresentQueueFamilyIndex  : vk::QueueFamilyIgnored;
        _ComputeQueueFamilyIndex  = bEnableComputeQueue  ? ComputeQueueFamilyIndex  : vk::QueueFamilyIgnored;
    }

    _PhysicalDevice = _AvailablePhysicalDevices[DeviceIndex];
    return vk::Result::eSuccess;
}

vk::Result FVulkanCore::ObtainPhysicalDeviceSurfaceFormats()
{
    try
    {
        _AvailableSurfaceFormats = _PhysicalDevice.getSurfaceFormatsKHR(_Surface);
    }
    catch (const vk::SystemError& Error)
    {
        NpgsCoreError("Failed to obtain surface formats: {}", Error.what());
        return static_cast<vk::Result>(Error.code().value());
    }

    NpgsCoreInfo("Surface formats obtained successfully.");
    return vk::Result::eSuccess;
}

vk::Result FVulkanCore::ObtainQueueFamilyIndices(const vk::PhysicalDevice& PhysicalDevice, bool bEnableGraphicsQueue,
                                                 bool bEnableComputeQueue, FQueueFamilyIndicesComplex& Indices)
{
    std::vector<vk::QueueFamilyProperties> QueueFamilyProperties = PhysicalDevice.getQueueFamilyProperties();
    if (QueueFamilyProperties.empty())
    {
        NpgsCoreError("Failed to get queue family properties.");
        return static_cast<vk::Result>(VK_RESULT_MAX_ENUM);
    }

    auto& [GraphicsQueueFamilyIndex, PresentQueueFamilyIndex, ComputeQueueFamilyIndex] = Indices;
    GraphicsQueueFamilyIndex = PresentQueueFamilyIndex = ComputeQueueFamilyIndex = vk::QueueFamilyIgnored;

    // 队列族的位置是和队列族本身的索引是对应的
    for (std::uint32_t i = 0; i != QueueFamilyProperties.size(); ++i)
    {
        vk::Bool32 bSupportGraphics = bEnableGraphicsQueue && QueueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics;
        vk::Bool32 bSupportPresent  = vk::False;
        vk::Bool32 bSupportCompute  = bEnableComputeQueue  && QueueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eCompute;

        if (_Surface)
        {
            try
            {
                bSupportPresent = PhysicalDevice.getSurfaceSupportKHR(i, _Surface);
            }
            catch (const vk::SystemError& Error)
            {
                NpgsCoreError("Failed to determine if the queue family supports presentation: {}", Error.what());
                return static_cast<vk::Result>(Error.code().value());
            }
        }

        if (bSupportGraphics && bSupportCompute) // 如果同时支持图形和计算队列
        {
            if (bSupportPresent) // 三个队列族都支持，将它们的索引设置为相同的值
            {
                GraphicsQueueFamilyIndex = PresentQueueFamilyIndex = ComputeQueueFamilyIndex = i;
                break;
            }
            if (GraphicsQueueFamilyIndex != ComputeQueueFamilyIndex || GraphicsQueueFamilyIndex == vk::QueueFamilyIgnored)
            {
                GraphicsQueueFamilyIndex = ComputeQueueFamilyIndex = i; // 确保图形和计算队列族的索引相同
            }
            if (!_Surface)
            {
                break; // 如果不需要呈现，那么只需要找到一个支持图形和计算队列族的索引即可
            }
        }

        // 将支持对应功能的队列族的索引设置为找到的第一个值
        if (bSupportGraphics && GraphicsQueueFamilyIndex == vk::QueueFamilyIgnored)
        {
            GraphicsQueueFamilyIndex = i;
        }
        if (bSupportPresent && PresentQueueFamilyIndex == vk::QueueFamilyIgnored)
        {
            PresentQueueFamilyIndex = i;
        }
        if (bSupportCompute && ComputeQueueFamilyIndex == vk::QueueFamilyIgnored)
        {
            ComputeQueueFamilyIndex = i;
        }
    }

    // 如果有任意一个需要启用的队列族的索引是 vk::QueueFamilyIgnored，报错
    if ((GraphicsQueueFamilyIndex == vk::QueueFamilyIgnored && bEnableGraphicsQueue) ||
        (PresentQueueFamilyIndex  == vk::QueueFamilyIgnored && _Surface) ||
        (ComputeQueueFamilyIndex  == vk::QueueFamilyIgnored && bEnableComputeQueue))
    {
        NpgsCoreError("Failed to obtain queue family indices.");
        return static_cast<vk::Result>(VK_RESULT_MAX_ENUM);
    }

    NpgsCoreInfo("Queue family indices obtained successfully.");
    return vk::Result::eSuccess;
}

vk::Result FVulkanCore::CreateSwapchainInternal()
{
    try
    {
        _Swapchain = _Device.createSwapchainKHR(_SwapchainCreateInfo);
    }
    catch (const vk::SystemError& Error)
    {
        NpgsCoreError("Failed to create swapchain: {}", Error.what());
        return static_cast<vk::Result>(Error.code().value());
    }

    try
    {
        _SwapchainImages = _Device.getSwapchainImagesKHR(_Swapchain);
    }
    catch (const vk::SystemError& Error)
    {
        NpgsCoreError("Failed to get swapchain images: {}", Error.what());
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
            NpgsCoreError("Failed to create image view: {}", Error.what());
            return static_cast<vk::Result>(Error.code().value());
        }

        _SwapchainImageViews.emplace_back(ImageView);
    }

    return vk::Result::eSuccess;
}

_GRAPHICS_END
_RUNTIME_END
_NPGS_END
