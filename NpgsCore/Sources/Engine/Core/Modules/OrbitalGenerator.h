#pragma once

#include <random>
#include <tuple>
#include <vector>

#include "Engine/Base/StellarSystem.h"
#include "Engine/Core/Base.h"
#include "Engine/Core/Logger.h"
#include "Engine/Core/Random.hpp"

_NPGS_BEGIN
_MODULES_BEGIN

class NPGS_API OrbitalGenerator {
public:
    OrbitalGenerator() = default;
    OrbitalGenerator(const std::seed_seq& SeedSeq, float AsteroidUpperLimit = 1e21f, float LifeProbatility = 0.0114514f, bool bContainUltravioletChz = false, bool bEnableAsiFilter = true);
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

    float _AsteroidUpperLimit;
    bool  _bContainUltravioletChz;
    std::mt19937 _RandomEngine;
    UniformRealDistribution<float> _CommonGenerator;
    BernoulliDistribution _MigrationProbability;
    BernoulliDistribution _WalkInProbability;
    BernoulliDistribution _ScatteringProbability;
    BernoulliDistribution _AsteroidBeltProbability;
    BernoulliDistribution _LifeOccurrenceProbability;
    BernoulliDistribution _AsiFiltedProbability;

    static const std::array<float, 7> _kProbabilityListForCenoziocEra;
    static const std::array<float, 7> _kProbabilityListForSatTeeTouyButAsi;
};

_MODULES_END
_NPGS_END
