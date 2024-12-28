#pragma once

#include <string>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include "Engine/Core/Base.h"

_NPGS_BEGIN

class FApplication
{
public:
	FApplication(const vk::Extent2D& WindowSize, const std::string& WindowTitle, bool bEnableVSync, bool bEnableFullscreen);
	~FApplication() = default;

	void ExecuteMainRender();
	void Terminate();

private:
	bool InitWindow();
	void ShowTitleFps();
	void ProcessInput();

private:
	std::string  _WindowTitle;
	vk::Extent2D _WindowSize;
	GLFWwindow*  _Window;
	bool         _bEnableVSync;
	bool         _bEnableFullscreen;
};

_NPGS_END
