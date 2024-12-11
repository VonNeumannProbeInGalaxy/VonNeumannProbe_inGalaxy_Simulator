#pragma once

#include <array>
#include <memory>
#include <random>
#include <tuple>
#include <vector>

#include "Engine/Base/Astro/StellarSystem.h"
#include "Engine/Core/Base.h"
#include "Engine/Modules/Generators/CivilizationGenerator.h"
#include "Engine/Utilities/Random.hpp"

_NPGS_BEGIN
_MODULE_BEGIN

class NPGS_API OrbitalGenerator
{
public:
	enum class GenerateOption
	{
		kStars,
		kPlanets
	};

	struct PlanetaryDisk
	{
		float InterRadiusAu{};
		float OuterRadiusAu{};
		float DiskMassSol{};
		float DustMassSol{};
	};

public:
	OrbitalGenerator() = delete;
	explicit OrbitalGenerator(const std::seed_seq& SeedSequence,
							  float UniverseAge                      = 1.38e10f,
							  float BinaryPeriodMean                 = 5.03f,
							  float BinaryPeriodSigma                = 2.28f,
							  float CoilTemperatureLimit             = 1514.114f,
							  float AsteroidUpperLimit               = 1e21f,
							  float RingsParentLowerLimit            = 1e23f,
							  float LifeOccurrenceProbability        = 0.0114514f,
							  bool  bContainUltravioletHabitableZone = false,
							  bool  bEnableAsiFilter                 = true);

	OrbitalGenerator(const OrbitalGenerator&) = delete;
	OrbitalGenerator(OrbitalGenerator&&) noexcept = default;
	~OrbitalGenerator() = default;

	OrbitalGenerator& operator=(const OrbitalGenerator&) = delete;
	OrbitalGenerator& operator=(OrbitalGenerator&&) noexcept = default;

	void GenerateOrbitals(Astro::StellarSystem& System);

private:
	void GenerateBinaryOrbit(Astro::StellarSystem& System);

	void GeneratePlanets(std::size_t StarIndex,
						 Astro::Orbit::ObjectDetails& ParentStar,
						 Astro::StellarSystem& System);

	void GenerateOrbitElements(Astro::Orbit& Orbit);

	std::size_t JudgeLargePlanets(std::size_t StarIndex,
								  const std::vector<std::unique_ptr<Astro::Star>>& StarData,
								  float BinarySemiMajorAxis,
								  float InterHabitableZoneRadiusAu,
								  float FrostLineAu,
								  std::vector<float>& CoreMassesSol,
								  std::vector<float>& NewCoreMassesSol,
								  std::vector<std::unique_ptr<Astro::Orbit>>& Orbits,
								  std::vector<std::unique_ptr<Astro::Planet>>& Planets);

	float CalculatePlanetMass(float CoreMass,
							  float NewCoreMass,
							  float SemiMajorAxisAu,
							  const PlanetaryDisk& PlanetaryDiskTempData,
							  const Astro::Star* Star,
							  Astro::Planet* Planet);

	void CalculatePlanetRadius(float MassEarth, Astro::Planet* Planet);
	void GenerateSpin(float SemiMajorAxis, const Astro::Orbit::OrbitalObject& Parent, Astro::Planet* Planet);

	void CalculateTemperature(const Astro::Orbit::ObjectType ParentType,
							  float PoyntingVector,
							  Astro::Planet* Planet);

	void GenerateMoons(std::size_t PlanetIndex,
					   float FrostLineAu,
					   const Astro::Star* Star,
					   float PoyntingVector,
					   const std::pair<float, float>& HabitableZoneAu,
					   Astro::Orbit::ObjectDetails& ParentPlanet,
					   std::vector<std::unique_ptr<Astro::Orbit>>& Orbits,
					   std::vector<std::unique_ptr<Astro::Planet>>& Planets);

	void GenerateRings(std::size_t PlanetIndex,
					   float FrostLineAu,
					   const Astro::Star* Star,
					   Astro::Orbit::ObjectDetails& ParentPlanet,
					   std::vector<std::unique_ptr<Astro::Orbit>>& Orbits,
					   std::vector<std::unique_ptr<Astro::AsteroidCluster>>& AsteroidClusters);

	void GenerateTerra(const Astro::Star* Star,
					   float PoyntingVector,
					   const std::pair<float, float>& HabitableZoneAu,
					   const Astro::Orbit* Orbit,
					   Astro::Planet* Planet);

	void GenerateTrojan(const Astro::Star* Star,
						float FrostLineAu,
						Astro::Orbit* Orbit,
						Astro::Orbit::ObjectDetails& ParentPlanet,
						std::vector<std::unique_ptr<Astro::AsteroidCluster>>& AsteroidClusters);

	void GenerateCivilization(const Astro::Star* Star,
							  float PoyntingVector,
							  const std::pair<float, float>& HabitableZoneAu,
							  const Astro::Orbit* Orbit,
							  Astro::Planet* Planet);

	void CalculateOrbitalPeriods(std::vector<std::unique_ptr<Astro::Orbit>>& Orbits);

private:
	std::mt19937                                 _RandomEngine;
	std::array<Util::BernoulliDistribution<>, 2> _RingsProbabilities;
	Util::BernoulliDistribution<>                _AsteroidBeltProbability;
	Util::BernoulliDistribution<>                _MigrationProbability;
	Util::BernoulliDistribution<>                _ScatteringProbability;
	Util::BernoulliDistribution<>                _WalkInProbability;
	Util::NormalDistribution<>                   _BinaryPeriodDistribution;
	Util::UniformRealDistribution<>              _CommonGenerator;

	std::unique_ptr<CivilizationGenerator> _CivilizationGenerator;

	float _AsteroidUpperLimit;
	float _CoilTemperatureLimit;
	float _RingsParentLowerLimit;
	float _UniverseAge;
	bool  _bContainUltravioletHabitableZone;
};

_MODULE_END
_NPGS_END
