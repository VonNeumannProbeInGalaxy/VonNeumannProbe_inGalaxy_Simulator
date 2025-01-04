#pragma once

#include "Engine/Core/Base/Assert.h"
#include "Engine/Core/Base/Base.h"

#include "Engine/Core/Math/NumericConstants.h"

#include "Engine/Core/Runtime/Assets/AssetManager.h"
#include "Engine/Core/Runtime/Assets/CommaSeparatedValues.hpp"
#include "Engine/Core/Runtime/Assets/GetAssetFullPath.h"
#include "Engine/Core/Runtime/Assets/Mesh.h"
#include "Engine/Core/Runtime/Assets/Model.h"
#include "Engine/Core/Runtime/Assets/Shader.h"
#include "Engine/Core/Runtime/Assets/Texture.h"

#include "Engine/Core/Runtime/Graphics/OpenGL/Framebuffer.h"
#include "Engine/Core/Runtime/Graphics/OpenGL/ShaderBlockManager.h"
#include "Engine/Core/Runtime/Graphics/Renderers/ScreenQuadRenderer.h"
#include "Engine/Core/Runtime/Graphics/Vulkan/VulkanBase.h"
#include "Engine/Core/Runtime/Graphics/Vulkan/VulkanCore.h"
#include "Engine/Core/Runtime/Graphics/Vulkan/VulkanWrappers.h"

#include "Engine/Core/Runtime/Threads/ThreadPool.h"

#include "Engine/Core/System/Generators/CivilizationGenerator.h"
#include "Engine/Core/System/Generators/OrbitalGenerator.h"
#include "Engine/Core/System/Generators/StellarGenerator.h"

#include "Engine/Core/System/Spatial/Camera.h"
#include "Engine/Core/System/Spatial/Octree.hpp"

#include "Engine/Core/Types/Entries/Astro/CelestialObject.h"
#include "Engine/Core/Types/Entries/Astro/Planet.h"
#include "Engine/Core/Types/Entries/Astro/Star.h"
#include "Engine/Core/Types/Entries/Astro/StellarSystem.h"
#include "Engine/Core/Types/Entries/NpgsObject.h"

#include "Engine/Core/Types/Properties/Intelli/Artifact.h"
#include "Engine/Core/Types/Properties/Intelli/Civilization.h"
#include "Engine/Core/Types/Properties/StellarClass.h"

#include "Engine/Utils/Logger.h"
#include "Engine/Utils/Random.hpp"
#include "Engine/Utils/Utils.h"
