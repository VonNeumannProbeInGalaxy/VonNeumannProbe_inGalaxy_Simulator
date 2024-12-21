#pragma once

#include <array>
#include <memory>
#include <random>

#include "Engine/Base/Astro/Planet.h"
#include "Engine/Base/Astro/Star.h"
#include "Engine/Core/Base.h"
#include "Engine/Utilities/Random.hpp"

_NPGS_BEGIN
_MODULE_BEGIN

class FCivilizationGenerator
{
public:
	FCivilizationGenerator() = delete;
	explicit FCivilizationGenerator(const std::seed_seq& SeedSequence, float LifeOccurrenceProbability,
								   bool bEnableAsiFilter = false, float DestroyedByDisasterProbability = 0.001f);

	~FCivilizationGenerator() = default;

	void GenerateCivilization(const Astro::AStar* Star, float PoyntingVector, Astro::APlanet* Planet);

private:
	void GenerateLife(double StarAge, float PoyntingVector, Astro::APlanet* Planet);
	void GenerateCivilizationDetails(const Astro::AStar* Star, float PoyntingVector, Astro::APlanet* Planet);

private:
	std::mt19937                     _RandomEngine;
	Util::TBernoulliDistribution<>   _AsiFiltedProbability;
	Util::TBernoulliDistribution<>   _DestroyedByDisasterProbability;
	Util::TBernoulliDistribution<>   _LifeOccurrenceProbability;
	Util::TUniformRealDistribution<> _CommonGenerator;

	static const std::array<float, 7> _kProbabilityListForCenoziocEra;
	static const std::array<float, 7> _kProbabilityListForSatTeeTouyButAsi;
};

_MODULE_END
_NPGS_END
