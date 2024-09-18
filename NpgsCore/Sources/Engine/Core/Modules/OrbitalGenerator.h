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
    struct PlanetaryDisk {
        float InterRadiusAu;
        float OuterRadiusAu;
        float DiskMassSol;
        float DustMassSol;
    };

public:
    OrbitalGenerator() = delete;
    OrbitalGenerator(const std::seed_seq& SeedSequence, float UniverseAge = 1.38e10f, float AsteroidUpperLimit = 1e21f, float LifeOccurrenceProbatility = 0.0114514f, bool bContainUltravioletChz = false, bool bEnableAsiFilter = true);
    ~OrbitalGenerator() = default;

public:
    void GenerateOrbitals(StellarSystem& System);

public: // private:
    // Processor functions, as member functions to access class members
    // ----------------------------------------------------------------
    void GeneratePlanets(StellarSystem& System);
    void GenOrbitElements(StellarSystem::OrbitalElements& Orbit);
    float JudgePlanets(float InterChzRadiusAu, float FrostLineAu, const Astro::Star* Star, std::vector<float>& CoreMassesSol, std::vector<float>& NewCoreMassesSol, std::vector<StellarSystem::OrbitalElements>& Orbits, std::vector<std::unique_ptr<Astro::Planet>>& Planets);
    float CalcPlanetMass(float CoreMass, float NewCoreMass, float SemiMajorAxisAu, const PlanetaryDisk& PlanetaryDiskTempData, const Astro::Star* Star, std::unique_ptr<Astro::Planet>& Planet);
    void GenerateRings(std::size_t Index, float FrostLineAu, const Astro::Star* Star, std::vector<StellarSystem::OrbitalElements>& Orbits, std::vector<std::unique_ptr<Astro::Planet>>& Planets, std::vector<std::unique_ptr<Astro::AsteroidCluster>>& AsteroidClusters);
    void GenerateSpin(float SemiMajorAxis, const Astro::Star* Star, std::unique_ptr<Astro::Planet>& Planet);
    void CalcTemperature(float PoyntingVector, const Astro::Star* Star, std::unique_ptr<Astro::Planet>& Planet);

private:
    std::mt19937 _RandomEngine;
    std::array<BernoulliDistribution<>, 2> _RingsProbabilities;
    BernoulliDistribution<>                _AsteroidBeltProbability;
    BernoulliDistribution<>                _MigrationProbability;
    BernoulliDistribution<>                _ScatteringProbability;
    BernoulliDistribution<>                _WalkInProbability;
    UniformRealDistribution<>              _CommonGenerator;

    std::unique_ptr<CivilizationGenerator> _CivilizationGenerator;

    float _AsteroidUpperLimit;
    float _UniverseAge;
    bool  _bContainUltravioletChz;
};

_MODULES_END
_NPGS_END
