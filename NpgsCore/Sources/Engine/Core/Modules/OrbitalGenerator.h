#pragma once

#include <array>
#include <memory>
#include <random>
#include <tuple>
#include <vector>

#include "Engine/Base/StellarSystem.h"
#include "Engine/Core/Modules/CivilizationGenerator.h"
#include "Engine/Core/Base.h"
#include "Engine/Core/Random.hpp"

_NPGS_BEGIN
_MODULE_BEGIN

class OrbitalGenerator {
public:
    enum class GenerateOption {
        kStars,
        kPlanets
    };

    struct PlanetaryDisk {
        float InterRadiusAu = 0.0f;
        float OuterRadiusAu = 0.0f;
        float DiskMassSol   = 0.0f;
        float DustMassSol   = 0.0f;
    };

public:
    OrbitalGenerator() = delete;
    explicit OrbitalGenerator(
        const std::seed_seq& SeedSequence,
        float UniverseAge                      = 1.38e10f,
        float BinaryPeriodMean                 = 5.03f,
        float BinaryPeriodSigma                = 2.28f,
        float AsteroidUpperLimit               = 1e21f,
        float LifeOccurrenceProbability        = 0.0114514f,
        bool  bContainUltravioletHabitableZone = false,
        bool  bEnableAsiFilter                 = true
    );

    OrbitalGenerator(const OrbitalGenerator&) = delete;
    OrbitalGenerator(OrbitalGenerator&&) noexcept = default;
    ~OrbitalGenerator() = default;

    OrbitalGenerator& operator=(const OrbitalGenerator&) = delete;
    OrbitalGenerator& operator=(OrbitalGenerator&&) noexcept = default;

    void GenerateOrbitals(StellarSystem& System);

private:
    // Processor functions, as member functions to access class members
    // ----------------------------------------------------------------
    void GenerateBinaryOrbit(StellarSystem& System);
    void GeneratePlanets(std::size_t StarIndex, StellarSystem& System);
    void GeneratePlanetOrbitElements(StellarSystem::OrbitalElements& Orbit);
    std::size_t JudgeLargePlanets(std::size_t StarIndex, const std::vector<std::unique_ptr<Astro::Star>>& StarData, float BinarySemiMajorAxis, float InterHabitableZoneRadiusAu, float FrostLineAu, std::vector<float>& CoreMassesSol, std::vector<float>& NewCoreMassesSol, std::vector<StellarSystem::OrbitalElements>& Orbits, std::vector<std::unique_ptr<Astro::Planet>>& Planets);
    float CalculatePlanetMass(float CoreMass, float NewCoreMass, float SemiMajorAxisAu, const PlanetaryDisk& PlanetaryDiskTempData, const Astro::Star* Star, Astro::Planet* Planet);
    void CalculatePlanetRadius(float MassEarth, Astro::Planet* Planet);
    void GenerateRings(std::size_t PlanetIndex, float FrostLineAu, const Astro::Star* Star, std::vector<StellarSystem::OrbitalElements>& Orbits, std::vector<std::unique_ptr<Astro::Planet>>& Planets, std::vector<std::unique_ptr<Astro::AsteroidCluster>>& AsteroidClusters);
    void GenerateTerra(const Astro::Star* Star, float PoyntingVector, const std::pair<float, float>& HabitableZoneAu, const StellarSystem::OrbitalElements& Orbit, Astro::Planet* Planet);
    void GenerateSpin(float SemiMajorAxis, const Astro::Star* Star, Astro::Planet* Planet);
    void CalculateTemperature(float PoyntingVector, const Astro::Star* Star, Astro::Planet* Planet);

private:
    std::mt19937                           _RandomEngine;
    std::array<BernoulliDistribution<>, 2> _RingsProbabilities;
    BernoulliDistribution<>                _AsteroidBeltProbability;
    BernoulliDistribution<>                _MigrationProbability;
    BernoulliDistribution<>                _ScatteringProbability;
    BernoulliDistribution<>                _WalkInProbability;
    NormalDistribution<>                   _BinaryPeriodDistribution;
    UniformRealDistribution<>              _CommonGenerator;

    std::unique_ptr<CivilizationGenerator> _CivilizationGenerator;

    float _AsteroidUpperLimit;
    float _UniverseAge;
    bool  _bContainUltravioletHabitableZone;
};

_MODULE_END
_NPGS_END
