#pragma once

#include <array>
#include <memory>
#include <random>

#include "Engine/Base/Astro/Planet.h"
#include "Engine/Core/Base.h"
#include "Engine/Utilities/Random.hpp"

_NPGS_BEGIN
_MODULE_BEGIN

class CivilizationGenerator {
public:
    CivilizationGenerator() = delete;
    explicit CivilizationGenerator(const std::seed_seq& SeedSequence, float LifeOccurrenceProbability, bool bEnableAsiFilter = false);
    ~CivilizationGenerator() = default;

    void GenerateCivilization(double StarAge, float PoyntingVector, Astro::Planet* Planet);

private:
    std::mt19937                    _RandomEngine;
    Util::BernoulliDistribution<>   _AsiFiltedProbability;
    Util::BernoulliDistribution<>   _LifeOccurrenceProbability;
    Util::UniformRealDistribution<> _CommonGenerator;

    static const std::array<float, 7> _kProbabilityListForCenoziocEra;
    static const std::array<float, 7> _kProbabilityListForSatTeeTouyButAsi;
};

_MODULE_END
_NPGS_END