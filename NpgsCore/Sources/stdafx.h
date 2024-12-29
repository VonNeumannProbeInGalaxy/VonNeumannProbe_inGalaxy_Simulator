#pragma once

#include <xstdafx.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <fast-cpp-csv-parser/csv.h>
#include <glad/glad.h>

#ifdef VULKAN
#define GLFW_INCLUDE_VULKAN
#define VK_USE_PLATFORM_WIN32_KHR
#endif // VULKAN
#include <GLFW/glfw3.h>

#ifdef VULKAN
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif // VULKAN
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#ifdef _DEBUG
#define ENABLE_CONSOLE_LOGGER
#endif // _DEBUG
#include "Engine/Utilities/Logger.h"
