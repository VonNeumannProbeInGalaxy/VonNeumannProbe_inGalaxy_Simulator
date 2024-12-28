#include "Application.h"

#include <cstdint>

#include "Engine/Core/Vulkan/VulkanBase.h"
#ifdef _DEBUG
#define ENABLE_CONSOLE_LOGGER
#endif // _DEBUG
#include "Engine/Utilities/Logger.h"

_NPGS_BEGIN

FApplication::FApplication(const vk::Extent2D& WindowSize, const std::string& WindowTitle,
						   bool bEnableVSync, bool bEnableFullscreen)
	:
	_WindowTitle(WindowTitle),
	_WindowSize(WindowSize),
	_Window(nullptr),
	_bEnableVSync(bEnableVSync),
	_bEnableFullscreen(bEnableFullscreen)
{
	if (!InitWindow())
	{
		NpgsCoreError("Error: Failed to create window.");
	}
}

void FApplication::ExecuteMainRender()
{
	while (!glfwWindowShouldClose(_Window))
	{
		ProcessInput();

		glfwSwapBuffers(_Window);
		glfwPollEvents();

		ShowTitleFps();
	}
}

void FApplication::Terminate()
{
	glfwTerminate();
}

bool FApplication::InitWindow()
{
	if (glfwInit() == GLFW_FALSE)
	{
		NpgsCoreError("Error: Failed to initialize GLFW.");
		return false;
	};

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);

	_Window = glfwCreateWindow(_WindowSize.width, _WindowSize.height, _WindowTitle.c_str(), nullptr, nullptr);
	if (_Window == nullptr)
	{
		NpgsCoreError("Error: Failed to create GLFW window.");
		glfwTerminate();
		return false;
	}

	std::uint32_t ExtensionCount = 0;
	const char** Extensions = glfwGetRequiredInstanceExtensions(&ExtensionCount);
	if (Extensions == nullptr)
	{
		NpgsCoreError("Error: Failed to get required instance extensions.");
		glfwDestroyWindow(_Window);
		glfwTerminate();
		return false;
	}

	FVulkanBase* VulkanBase = FVulkanBase::GetVulkanBaseInstance();
	for (std::uint32_t i = 0; i != ExtensionCount; ++i)
	{
		VulkanBase->AddInstanceExtension(Extensions[i]);
	}

	VulkanBase->AddDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	vk::Result Result;
	if ((Result = VulkanBase->CreateInstance(vk::InstanceCreateFlags())) != vk::Result::eSuccess)
	{
		glfwDestroyWindow(_Window);
		glfwTerminate();
		return false;
	}

	vk::SurfaceKHR Surface;
	if (glfwCreateWindowSurface(VulkanBase->GetInstance(), _Window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&Surface)) != VK_SUCCESS)
	{
		NpgsCoreError("Error: Failed to create window surface.");
		glfwDestroyWindow(_Window);
		glfwTerminate();
		return false;
	}
	VulkanBase->SetSurface(Surface);

	if (VulkanBase->CreateDevice({}, 0) != vk::Result::eSuccess ||
		VulkanBase->CreateSwapchain(_WindowSize, {}, false) != vk::Result::eSuccess)
	{
		return false;
	}

	return true;
}

void FApplication::ShowTitleFps()
{
	static double CurrentTime   = 0.0;
	static double PreviousTime  = glfwGetTime();
	static double LastFrameTime = 0.0;
	static double DeltaTime     = 0.0;
	static int    FrameCount    = 0;

	CurrentTime   = glfwGetTime();
	DeltaTime     = CurrentTime - LastFrameTime;
	LastFrameTime = CurrentTime;
	++FrameCount;
	if (CurrentTime - PreviousTime >= 1.0)
	{
		glfwSetWindowTitle(_Window, (std::string(_WindowTitle) + " " + std::to_string(FrameCount)).c_str());
		FrameCount   = 0;
		PreviousTime = CurrentTime;
	}
}

void FApplication::ProcessInput()
{
	if (glfwGetKey(_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(_Window, GL_TRUE);
	}
}

_NPGS_END
