#pragma once

#include <random>
#include <tuple>
#include <vector>

#include "Engine/Base/AstroObject/Star.h"
#include "Engine/Base/StellarSystem.h"
#include "Engine/Core/Base.h"
#include "Engine/Core/Logger.h"
#include "Engine/Core/Random.hpp"

_NPGS_BEGIN
_MODULES_BEGIN

class OrbitalGenerator {
public:
    OrbitalGenerator() = default;
    OrbitalGenerator(const std::seed_seq& SeedSeq);
    ~OrbitalGenerator() = default;

public:
    void GenerateOrbitals(StellarSystem& System);

private:
    void GeneratePlanetaryDisk(StellarSystem& System);
    void GeneratePlanetaryCore(StellarSystem& System);

private:
    struct PlanetaryDisk {
        float InterRadius;
        float OuterRadius;
        float DiskMass;
        float DustMass;
    };

    std::mt19937 _RandomEngine;
    UniformRealDistribution<float> _CommonGenerator;
    PlanetaryDisk _PlanetaryDiskTempData;
};

_MODULES_END
_NPGS_END
