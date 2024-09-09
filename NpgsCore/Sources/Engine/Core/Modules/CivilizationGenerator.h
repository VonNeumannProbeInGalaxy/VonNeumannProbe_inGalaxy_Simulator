#pragma once

#include <random>

#include "Engine/Core/Base.h"
#include "Engine/Core/Random.hpp"

_NPGS_BEGIN
_MODULES_BEGIN

class CivilizationGenerator {
public:
    CivilizationGenerator() = default;
    CivilizationGenerator(double StarAge, float PoyntingVector, float PlanetRadius, float PlanetMass);
    ~CivilizationGenerator() = default;

private:
};

_MODULES_END
_NPGS_END
