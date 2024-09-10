#pragma once

#include <array>
#include <random>

#include "Engine/Base/NpgsObject/Astro/Planet.h"
#include "Engine/Core/Base.h"
#include "Engine/Core/Random.hpp"

_NPGS_BEGIN
_MODULES_BEGIN

class CivilizationGenerator {
public:
    CivilizationGenerator() = default;
    CivilizationGenerator(const std::seed_seq& SeedSequence, double StarAge, float PoyntingVector, float PlanetRadius, float PlanetMass, float LifeOccurrenceProbability, bool bEnableAsiFilter = false);
    ~CivilizationGenerator() = default;

    void GenerateCivilization(Astro::Planet& Planet);

private:
    std::mt19937 _RandomEngine;
    BernoulliDistribution<> _AsiFiltedProbability;
    BernoulliDistribution<> _LifeOccurrenceProbability;
    UniformRealDistribution<> _CommonGenerator;

    double _StarAge;

    static const std::array<float, 7> _kProbabilityListForCenoziocEra;
    static const std::array<float, 7> _kProbabilityListForSatTeeTouyButAsi;
};

_MODULES_END
_NPGS_END
