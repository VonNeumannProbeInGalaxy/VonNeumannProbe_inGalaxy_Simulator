#pragma once

#include <functional>
#include <string>
#include <vector>
#include <utility>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "Engine/Core/Base/Base.h"
#include "Engine/Core/Runtime/Graphics/Vulkan/VulkanBase.h"
#include "Engine/Core/Runtime/Graphics/Vulkan/VulkanCore.h"
#include "Engine/Core/Runtime/Graphics/Vulkan/VulkanWrappers.h"

_NPGS_BEGIN

class FApplication
{
private:
    enum class ECallbackType
    {
        kCreateSwapchain,
        kDestroySwapchain,
        kCreateDevice,
        kDestroyDevice
    };

    struct FRenderer
    {
        std::vector<Runtime::Graphics::FVulkanFramebuffer> Framebuffers;
        Runtime::Graphics::FVulkanRenderPass RenderPass;
    };

public:
    FApplication(const vk::Extent2D& WindowSize, const std::string& WindowTitle, bool bEnableVSync, bool bEnableFullscreen);
    ~FApplication();

    void ExecuteMainRender();
    void Terminate();

private:
    bool InitWindow();
    void CreateScreenRender();
    void CreatePipelineLayout();
    void CreatePipeline();
    void ShowTitleFps();
    void ProcessInput();

    void RegisterAutoRemovedCallbacks(ECallbackType Type, const std::string& Name, const std::function<void()>& Callback);
    void RemoveRegisteredCallbacks();

    static void FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height);

private:
    std::vector<std::pair<ECallbackType, std::string>> _AutoRemovedCallbacks;

    Runtime::Graphics::FVulkanBase*          _VulkanBase;
    Runtime::Graphics::FVulkanCore*          _VulkanCore;
    Runtime::Graphics::FVulkanPipelineLayout _VulkanPipelineLayout;
    Runtime::Graphics::FVulkanPipeline       _VulkanPipeline;
    FRenderer                                _Renderer;

    std::string  _WindowTitle;
    vk::Extent2D _WindowSize;
    GLFWwindow*  _Window;
    bool         _bEnableVSync;
    bool         _bEnableFullscreen;
};

_NPGS_END
