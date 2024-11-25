#pragma once

#include <array>
#include <memory>
#include <random>
#include <tuple>
#include <vector>

#include "Engine/Base/NpgsObject/Astro/StellarSystem.h"
#include "Engine/Core/Modules/Generators/CivilizationGenerator.h"
#include "Engine/Core/Utilities/Random.hpp"
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_MODULE_BEGIN

class OrbitalGenerator {
public:
    enum class GenerateOption {
        kStars,
        kPlanets
    };

    struct PlanetaryDisk {
        float InterRadiusAu{ 0.0f };
        float OuterRadiusAu{ 0.0f };
        float DiskMassSol{ 0.0f };
        float DustMassSol{ 0.0f };
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

    void GenerateOrbitals(Astro::StellarSystem& System);

private:
    void GenerateBinaryOrbit(Astro::StellarSystem& System);
    void GeneratePlanets(std::size_t StarIndex, Astro::StellarSystem& System);
    void GenerateOrbitElements(Astro::StellarSystem::Orbit& Orbit);

    std::size_t JudgeLargePlanets(
        std::size_t StarIndex,
        const std::vector<std::unique_ptr<Astro::Star>>& StarData,
        float BinarySemiMajorAxis,
        float InterHabitableZoneRadiusAu,
        float FrostLineAu,
        std::vector<float>& CoreMassesSol,
        std::vector<float>& NewCoreMassesSol,
        std::vector<Astro::StellarSystem::Orbit>& Orbits,
        std::vector<std::unique_ptr<Astro::Planet>>& Planets
    );
    
    float CalculatePlanetMass(
        float CoreMass,
        float NewCoreMass,
        float SemiMajorAxisAu,
        const PlanetaryDisk& PlanetaryDiskTempData,
        const Astro::Star* Star,
        Astro::Planet* Planet
    );
    
    void CalculatePlanetRadius(float MassEarth, Astro::Planet* Planet);
    void GenerateSpin(float SemiMajorAxis, const Astro::Star* Star, Astro::Planet* Planet);
    void CalculateTemperature(float PoyntingVector, const Astro::Star* Star, Astro::Planet* Planet);

    void GenerateMoons(
        std::size_t PlanetIndex,
        float FrostLineAu,
        const Astro::Star* Star,
        float PoyntingVector,
        const Astro::Planet* Planet,
        std::vector<Astro::StellarSystem::Orbit>& Orbits,
        std::vector<std::unique_ptr<Astro::Planet>>& Planets
    );

    void GenerateRings(
        std::size_t PlanetIndex,
        float FrostLineAu,
        const Astro::Star* Star,
        const Astro::Planet* Planet,
        std::vector<Astro::StellarSystem::Orbit>& Orbits,
        std::vector<std::unique_ptr<Astro::AsteroidCluster>>& AsteroidClusters
    );

    void GenerateTerra(
        const Astro::Star* Star,
        float PoyntingVector,
        const std::pair<float, float>& HabitableZoneAu,
        const Astro::StellarSystem::Orbit& Orbit,
        Astro::Planet* Planet
    );

    void GenerateCivilization(
        const Astro::Star* Star,
        float PoyntingVector,
        const std::pair<float, float>& HabitableZoneAu,
        const Astro::StellarSystem::Orbit& Orbit,
        Astro::Planet* Planet
    );

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
