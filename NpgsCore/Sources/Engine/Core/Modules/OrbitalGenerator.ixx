module;

#include "Engine/Core/Base.h"

export module Module.OrbitalGenerator;

import <array>;
import <memory>;
import <random>;
import <tuple>;
import <vector>;

import Base.StellarSystem;
import Core.Random;
import Module.CivilizationGenerator;

_NPGS_BEGIN
_MODULES_BEGIN

export class OrbitalGenerator {
public:
    enum class GenerateOption {
        kStars,
        kPlanets
    };

    struct PlanetaryDisk {
        float InterRadiusAu;
        float OuterRadiusAu;
        float DiskMassSol;
        float DustMassSol;
    };

public:
    OrbitalGenerator() = delete;
    explicit OrbitalGenerator(const std::seed_seq& SeedSequence, float UniverseAge = 1.38e10f, float AsteroidUpperLimit = 1e21f, float LifeOccurrenceProbatility = 0.0114514f, bool bContainUltravioletHabitableZone = false, bool bEnableAsiFilter = true);
    ~OrbitalGenerator() = default;

public:
    void GenerateOrbitals(StellarSystem& System);

private:
    // Processor functions, as member functions to access class members
    // ----------------------------------------------------------------
    void GeneratePlanets(StellarSystem& System);
    void GenerateOrbitElements(StellarSystem::OrbitalElements& Orbit);
    std::size_t JudgePlanets(float InterHabitableZoneRadiusAu, float FrostLineAu, const Astro::Star* Star, std::vector<float>& CoreMassesSol, std::vector<float>& NewCoreMassesSol, std::vector<StellarSystem::OrbitalElements>& Orbits, std::vector<std::unique_ptr<Astro::Planet>>& Planets);
    float CalculatePlanetMass(float CoreMass, float NewCoreMass, float SemiMajorAxisAu, const PlanetaryDisk& PlanetaryDiskTempData, const Astro::Star* Star, std::unique_ptr<Astro::Planet>& Planet);
    void CalculatePlanetRadius(float MassEarth, std::unique_ptr<Astro::Planet>& Planet);
    void GenerateRings(std::size_t Index, float FrostLineAu, const Astro::Star* Star, std::vector<StellarSystem::OrbitalElements>& Orbits, std::vector<std::unique_ptr<Astro::Planet>>& Planets, std::vector<std::unique_ptr<Astro::AsteroidCluster>>& AsteroidClusters);
    void GenerateSpin(float SemiMajorAxis, const Astro::Star* Star, std::unique_ptr<Astro::Planet>& Planet);
    void CalculateTemperature(float PoyntingVector, const Astro::Star* Star, std::unique_ptr<Astro::Planet>& Planet);

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
    bool  _bContainUltravioletHabitableZone;
};

_MODULES_END
_NPGS_END
