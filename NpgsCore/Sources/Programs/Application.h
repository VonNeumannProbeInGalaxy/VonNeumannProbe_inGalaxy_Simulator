#pragma once

#include <functional>
#include <string>
#include <vector>
#include <utility>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "Engine/Core/Vulkan/VulkanBase.h"
#include "Engine/Core/Vulkan/VulkanCore.h"
#include "Engine/Core/Vulkan/VulkanWrappers.h"
#include "Engine/Core/Base/Base.h"

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
		std::vector<FVulkanFramebuffer> Framebuffers;
		FVulkanRenderPass RenderPass;
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

	FVulkanBase*          _VulkanBase;
	FVulkanCore*          _VulkanCore;
	FVulkanPipelineLayout _VulkanPipelineLayout;
	FVulkanPipeline       _VulkanPipeline;
    FRenderer             _Renderer;

	std::string           _WindowTitle;
	vk::Extent2D          _WindowSize;
	GLFWwindow*           _Window;
	bool                  _bEnableVSync;
	bool                  _bEnableFullscreen;
};

_NPGS_END
