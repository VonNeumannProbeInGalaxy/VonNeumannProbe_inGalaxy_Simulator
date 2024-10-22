module;

#include "Engine/Core/Base.h"

export module Module.CivilizationGenerator;

import <array>;
import <memory>;
import <random>;

import Base.Astro.Planet;
import Core.Random;

_NPGS_BEGIN
_MODULE_BEGIN

export class CivilizationGenerator {
public:
    CivilizationGenerator() = delete;
    explicit CivilizationGenerator(const std::seed_seq& SeedSequence, float LifeOccurrenceProbability, bool bEnableAsiFilter = false);
    ~CivilizationGenerator() = default;

    void GenerateCivilization(double StarAge, float PoyntingVector, float PlanetRadius, float PlanetMass, std::unique_ptr<Astro::Planet>& Planet);

private:
    std::mt19937 _RandomEngine;
    BernoulliDistribution<> _AsiFiltedProbability;
    BernoulliDistribution<> _LifeOccurrenceProbability;
    UniformRealDistribution<> _CommonGenerator;

    static const std::array<float, 7> _kProbabilityListForCenoziocEra;
    static const std::array<float, 7> _kProbabilityListForSatTeeTouyButAsi;
};

_MODULE_END
_NPGS_END
