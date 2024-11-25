#pragma once

#include "Engine/AssetLoader/AssetManager.h"
#include "Engine/AssetLoader/Csv.hpp"

#include "Engine/Base/NpgsObject/Astro/CelestialObject.h"
#include "Engine/Base/NpgsObject/Astro/Planet.h"
#include "Engine/Base/NpgsObject/Astro/Star.h"
#include "Engine/Base/NpgsObject/Astro/StellarSystem.h"
#include "Engine/Base/NpgsObject/Civilization.h"
#include "Engine/Base/NpgsObject/NpgsObject.h"

#include "Engine/Core/Assert.h"
#include "Engine/Core/Base.h"
#include "Engine/Core/Constants.h"
#include "Engine/Core/Octree.hpp"
#include "Engine/Core/ThreadPool.h"
#include "Engine/Core/Universe.h"

#include "Engine/Modules/Generators/CivilizationGenerator.h"
#include "Engine/Modules/Generators/OrbitalGenerator.h"
#include "Engine/Modules/Generators/StellarGenerator.h"

#include "Engine/Utilities/Logger.h"
#include "Engine/Utilities/Random.hpp"
#include "Engine/Utilities/StellarClass.h"
#include "Engine/Utilities/Utilities.h"
