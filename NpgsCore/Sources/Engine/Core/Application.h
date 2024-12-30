#pragma once

#include <atomic>
#include <string>
#include <vector>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "Engine/Core/Vulkan/VulkanBase.h"
#include "Engine/Core/Vulkan/VulkanWrappers.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN

class FApplication
{
private:
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
	void CreatePipeline();
	void CreateLayout();
	void ShowTitleFps();
	void ProcessInput();

	static void FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height);

private:
	std::string           _WindowTitle;
    FRenderer             _Renderer;
	FVulkanPipeline       _Pipeline;
	FVulkanPipelineLayout _Layout;
	vk::Extent2D          _WindowSize;
	GLFWwindow*           _Window;
	FVulkanBase*          _VulkanBase;
	std::atomic<bool>     _IsValid;
	bool                  _bEnableVSync;
	bool                  _bEnableFullscreen;
};

_NPGS_END
