#pragma once

#include "Engine/AssetLoader/AssetManager.h"

#include "Engine/Base/Astro/CelestialObject.h"
#include "Engine/Base/Astro/Planet.h"
#include "Engine/Base/Astro/Star.h"
#include "Engine/Base/Astro/StellarSystem.h"
#include "Engine/Base/Intelli/Artifact.h"
#include "Engine/Base/Intelli/Civilization.h"
#include "Engine/Base/NpgsObject.h"

#include "Engine/Core/Renderers/Buffers/Framebuffer.h"
#include "Engine/Core/Renderers/ScreenQuadRenderer.h"
#include "Engine/Core/Assert.h"
#include "Engine/Core/Base.h"
#include "Engine/Core/Camera.h"
#include "Engine/Core/Constants.h"
#include "Engine/Core/Octree.hpp"
#include "Engine/Core/ThreadPool.h"
#include "Engine/Core/Universe.h"

#include "Engine/Modules/Generators/CivilizationGenerator.h"
#include "Engine/Modules/Generators/OrbitalGenerator.h"
#include "Engine/Modules/Generators/StellarGenerator.h"

#include "Engine/Utilities/Logger.h"
#include "Engine/Utilities/Random.hpp"
#include "Engine/Utilities/ShaderBlockManager.h"
#include "Engine/Utilities/StellarClass.h"
#include "Engine/Utilities/Utilities.h"
