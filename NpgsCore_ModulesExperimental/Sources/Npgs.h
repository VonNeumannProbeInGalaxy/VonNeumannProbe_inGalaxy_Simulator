#pragma once

#include "Engine/Base/NpgsObject/Astro/CelestialObject.h"
#include "Engine/Base/NpgsObject/Astro/Planet.h"
#include "Engine/Base/NpgsObject/Astro/Star.h"
#include "Engine/Base/NpgsObject/Civilization.h"
#include "Engine/Base/Octree.hpp"

#include "Engine/Core/AssetLoader/AssetManager.h"

#include "Engine/Core/Modules/CivilizationGenerator.h"
#include "Engine/Core/Modules/OrbitalGenerator.h"
#include "Engine/Core/Modules/StellarClass.h"
#include "Engine/Core/Modules/StellarGenerator.h"

#include "Engine/Core/Assert.h"
#include "Engine/Core/Base.h"
#include "Engine/Core/Constants.h"
#include "Engine/Core/Logger.h"
#include "Engine/Core/Random.hpp"
#include "Engine/Core/ThreadPool.h"
#include "Engine/Core/Universe/Universe.h"