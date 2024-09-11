#pragma once

#include <array>
#include <memory>
#include <random>
#include <tuple>
#include <vector>

#include "Engine/Base/StellarSystem.h"
#include "Engine/Core/Modules/CivilizationGenerator.h"
#include "Engine/Core/Base.h"
#include "Engine/Core/Logger.h"
#include "Engine/Core/Random.hpp"

_NPGS_BEGIN
_MODULES_BEGIN

class NPGS_API OrbitalGenerator {
public:
    OrbitalGenerator() = delete;
    OrbitalGenerator(const std::seed_seq& SeedSequence, float AsteroidUpperLimit = 1e21f, float LifeOccurrenceProbatility = 0.0114514f, bool bContainUltravioletChz = false, bool bEnableAsiFilter = true);
    ~OrbitalGenerator() = default;

public:
    void GenerateOrbitals(StellarSystem& System);

public: // private:
    void GeneratePlanets(StellarSystem& System);
    void GenOrbitElements(StellarSystem::OrbitalElements& Orbit);

private:
    struct PlanetaryDisk {
        float InterRadiusAu;
        float OuterRadiusAu;
        float DiskMassSol;
        float DustMassSol;
    };

    std::mt19937 _RandomEngine;
    std::array<BernoulliDistribution<>, 2> _RingsProbabilities;
    BernoulliDistribution<>                _AsteroidBeltProbability;
    BernoulliDistribution<>                _MigrationProbability;
    BernoulliDistribution<>                _ScatteringProbability;
    BernoulliDistribution<>                _WalkInProbability;
    UniformRealDistribution<>              _CommonGenerator;

    std::unique_ptr<CivilizationGenerator> _CivilizationGenerator;

    float _AsteroidUpperLimit;
    bool  _bContainUltravioletChz;
};

_MODULES_END
_NPGS_END
