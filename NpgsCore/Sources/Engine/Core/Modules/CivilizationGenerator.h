#pragma once

#include <array>
#include <memory>
#include <random>

#include "Engine/Base/NpgsObject/Astro/Planet.h"
#include "Engine/Core/Base.h"
#include "Engine/Core/Random.hpp"

_NPGS_BEGIN
_MODULE_BEGIN

class CivilizationGenerator {
public:
    CivilizationGenerator() = delete;
    explicit CivilizationGenerator(const std::seed_seq& SeedSequence, float LifeOccurrenceProbability, bool bEnableAsiFilter = false);
    ~CivilizationGenerator() = default;

    void GenerateCivilization(double StarAge, float PoyntingVector, float PlanetRadius, float PlanetMass, std::unique_ptr<Astro::Planet>& Planet);

private:
    std::mt19937              _RandomEngine;
    BernoulliDistribution<>   _AsiFiltedProbability;
    BernoulliDistribution<>   _LifeOccurrenceProbability;
    UniformRealDistribution<> _CommonGenerator;

    static const std::array<float, 7> _kProbabilityListForCenoziocEra;
    static const std::array<float, 7> _kProbabilityListForSatTeeTouyButAsi;
};

_MODULE_END
_NPGS_END
