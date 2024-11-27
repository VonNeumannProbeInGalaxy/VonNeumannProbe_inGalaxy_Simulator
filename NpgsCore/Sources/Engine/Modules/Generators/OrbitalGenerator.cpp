#include "OrbitalGenerator.h"

#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include <limits>
#include <print>
#include <utility>

#include <boost/multiprecision/cpp_int.hpp>
#include <glm/glm.hpp>

#include "Engine/Core/Assert.h"
#include "Engine/Core/Base.h"
#include "Engine/Core/Constants.h"
#include "Engine/Utilities/Utilities.h"

// #define DEBUG_OUTPUT

_NPGS_BEGIN
_MODULE_BEGIN

#define CalculatePlanetMassByIndex(Index)                                                                           \
    CalculatePlanetMass(                                                                                            \
        kSolarMass * CoreMassesSol[Index], kSolarMass * NewCoreMassesSol[Index],                                    \
        Planets[Index]->GetMigration() ? MigratedOriginSemiMajorAxisAu : Orbits[Index]->SemiMajorAxis / kAuToMeter, \
        PlanetaryDiskTempData, Star, Planets[Index].get()                                                           \
    )

// Tool functions
// --------------
static float CalculatePrevMainSequenceLuminosity(float StarInitialMassSol);
static std::unique_ptr<Astro::AsteroidCluster> PlanetToAsteroidCluster(const Astro::Planet* Planet);

// OrbitalGenerator implementations
// --------------------------------
OrbitalGenerator::OrbitalGenerator(
    const std::seed_seq& SeedSequence,
    float UniverseAge,
    float BinaryPeriodMean,
    float BinaryPeriodSigma,
    float AsteroidUpperLimit,
    float RingsParentLowerLimit,
    float LifeOccurrenceProbability,
    bool  bContainUltravioletHabitableZone,
    bool  bEnableAsiFilter
)   :
    _RandomEngine(SeedSequence),
    _RingsProbabilities{ Util::BernoulliDistribution<>(0.5), Util::BernoulliDistribution<>(0.2) },
    _AsteroidBeltProbability(0.4),
    _RingsParentLowerLimit(RingsParentLowerLimit),
    _MigrationProbability(0.1),
    _ScatteringProbability(0.15),
    _WalkInProbability(0.8),
    _BinaryPeriodDistribution(BinaryPeriodMean, BinaryPeriodSigma),
    _CommonGenerator(0.0f, 1.0f),

    _CivilizationGenerator(nullptr),

    _AsteroidUpperLimit(AsteroidUpperLimit),
    _UniverseAge(UniverseAge),
    _bContainUltravioletHabitableZone(bContainUltravioletHabitableZone)
{
    std::vector<std::uint32_t> Seeds(SeedSequence.size());
    SeedSequence.param(Seeds.begin());
    std::shuffle(Seeds.begin(), Seeds.end(), _RandomEngine);
    std::seed_seq ShuffledSeeds(Seeds.begin(), Seeds.end());
    
    _CivilizationGenerator =
        std::make_unique<CivilizationGenerator>(ShuffledSeeds, LifeOccurrenceProbability, bEnableAsiFilter);
}

void OrbitalGenerator::GenerateOrbitals(Astro::StellarSystem& System) {
    if (System.StarData().size() == 2) {
        GenerateBinaryOrbit(System);
        Astro::Star* Star1 = System.StarData()[0].get();
        Astro::Star* Star2 = System.StarData()[1].get();

        for (auto& Star : System.StarData()) {
            Astro::Star* Current  = Star.get();
            Astro::Star* TheOther = Star.get() == Star1 ? Star2 : Star1;

            if (Current->GetMass() > 12 * kSolarMass) {
                Current->SetHasPlanets(false);
            } else {
                if ((Current->GetStellarClass().GetStarType() == Util::StellarClass::StarType::kNeutronStar ||
                     Current->GetStellarClass().GetStarType() == Util::StellarClass::StarType::kBlackHole) &&
                     Current->GetStarFrom() != Astro::Star::StarFrom::kWhiteDwarfMerge) {
                     Current->SetHasPlanets(false);
                } else {
                    if (TheOther->GetStellarClass().GetStarType() == Util::StellarClass::StarType::kNeutronStar ||
                        TheOther->GetStellarClass().GetStarType() == Util::StellarClass::StarType::kBlackHole) {
                        if (TheOther->GetFeH() >= -2.0f) {
                            if (Current->GetAge() > TheOther->GetAge()) {
                                Current->SetHasPlanets(false);
                            }
                        } else {
                            if (TheOther->GetInitialMass() <= 40 * kSolarMass || TheOther->GetInitialMass() >= 140 * kSolarMass) {
                                if (Current->GetAge() > TheOther->GetAge()) {
                                    Current->SetHasPlanets(false);
                                }
                            }
                        }
                    }
                }
            }
        }
    } else {
        Astro::Star* Star = System.StarData()[0].get();
        Astro::StellarSystem::Orbit::OrbitalObject MainStar(Star, Astro::StellarSystem::Orbit::ObjectType::kStar);
        Astro::StellarSystem::Orbit PointOrbit;
        PointOrbit.Objects.emplace_back(MainStar);
        PointOrbit.Parent.BaryCenterPtr = System.GetBaryCenter();
        PointOrbit.ParentType = Astro::StellarSystem::Orbit::ObjectType::kBaryCenter;
        System.OrbitData().emplace_back(std::make_unique<Astro::StellarSystem::Orbit>(PointOrbit));

        if (Star->GetMass() > 12 * kSolarMass) {
            Star->SetHasPlanets(false);
        } else {
            if (Star->GetStellarClass().GetStarType() == Util::StellarClass::StarType::kNeutronStar ||
                Star->GetStellarClass().GetStarType() == Util::StellarClass::StarType::kBlackHole) {
                if (Star->GetStarFrom() != Astro::Star::StarFrom::kWhiteDwarfMerge) {
                    Star->SetHasPlanets(false);
                }
            }
        }
    }

    for (std::size_t i = 0; i != System.StarData().size(); ++i) {
        if (System.StarData()[i]->GetHasPlanets()) {
            GeneratePlanets(i, System.OrbitData()[i]->Objects.front(), System);
        }
    }
}

void OrbitalGenerator::GenerateBinaryOrbit(Astro::StellarSystem& System) {
    auto* SystemBaryCenter = System.GetBaryCenter();

    std::array<Astro::StellarSystem::Orbit, 2> Orbits;

    for (int i = 0; i != 2; ++i) {
        Orbits[i].Parent.BaryCenterPtr = SystemBaryCenter;
        Orbits[i].ParentType = Astro::StellarSystem::Orbit::ObjectType::kBaryCenter;
        GenerateOrbitElements(Orbits[i]);
    }
    
    float MassSol1 = static_cast<float>(System.StarData().front()->GetMass() / kSolarMass);
    float MassSol2 = static_cast<float>(System.StarData().back()->GetMass()  / kSolarMass);

    float LogPeriodDays       = 0.0f;
    float CommonCoefficient   = 365 * std::pow(MassSol1 + MassSol2, 0.3f);
    float LogPeriodLowerLimit = std::log10(50   * CommonCoefficient);
    float LogPeriodUpperLimit = std::log10(2500 * CommonCoefficient);

    do {
        LogPeriodDays = _BinaryPeriodDistribution(_RandomEngine);
    } while (LogPeriodDays > LogPeriodUpperLimit || LogPeriodDays < LogPeriodLowerLimit);

    float Period = std::pow(10.0f, LogPeriodDays) * kDayToSeconds;
    float BinarySemiMajorAxis = static_cast<float>(std::pow(
        (kGravityConstant * kSolarMass * (MassSol1 + MassSol2) * std::pow(Period, 2)) / (4 * std::pow(kPi, 2)),
        1.0 / 3.0
    ));

    float SemiMajorAxis1 = BinarySemiMajorAxis * MassSol2 / (MassSol1 + MassSol2);
    float SemiMajorAxis2 = BinarySemiMajorAxis - SemiMajorAxis1;

    Orbits[0].SemiMajorAxis = SemiMajorAxis1;
    Orbits[1].SemiMajorAxis = SemiMajorAxis2;

    Orbits[0].Period = Orbits[1].Period = Period;

    float Random = _CommonGenerator(_RandomEngine) * 1.2f;
    float Eccentricity = 0.0f;
    if (Period / kDayToSeconds < 10) {
        Eccentricity = Random * 0.01f;
    } else if (Period / kDayToSeconds < 1e6f) {
        Eccentricity = Random * (0.1975f * std::log10(Period / kDayToSeconds) - 0.385f);
    } else {
        Eccentricity = Random * 0.8f;
    }

    Orbits[0].Eccentricity = Orbits[1].Eccentricity = Eccentricity;
    Orbits[0].Normal = Orbits[1].Normal = System.GetBaryNormal();

    std::array<glm::vec2, 2> StarNormals{};

    for (int i = 0; i != 2; ++i) {
        StarNormals[i] = glm::vec2(Orbits[i].Normal + glm::vec2(
            -0.09f + _CommonGenerator(_RandomEngine) * 0.18f,
            -0.09f + _CommonGenerator(_RandomEngine) * 0.18f
        ));

        if (StarNormals[i].x > 2 * kPi) {
            StarNormals[i].x -= 2 * kPi;
        } else if (StarNormals[i].x < 0.0f) {
            StarNormals[i].x += 2 * kPi;
        }

        if (StarNormals[i].y > kPi) {
            StarNormals[i].y -= kPi;
        } else if (StarNormals[i].y < 0.0f) {
            StarNormals[i].y += kPi;
        }
    }

    System.StarData().front()->SetNormal(StarNormals[0]);
    System.StarData().back()->SetNormal(StarNormals[1]);

    Random = _CommonGenerator(_RandomEngine) * 2.0f * kPi;
    float ArgumentOfPeriapsis1 = Random;
    float ArgumentOfPeriapsis2 = 0.0f;
    if (ArgumentOfPeriapsis1 >= kPi) {
        ArgumentOfPeriapsis2 = ArgumentOfPeriapsis1 - kPi;
    } else {
        ArgumentOfPeriapsis2 = ArgumentOfPeriapsis1 + kPi;
    }

    Orbits[0].ArgumentOfPeriapsis = ArgumentOfPeriapsis1;
    Orbits[1].ArgumentOfPeriapsis = ArgumentOfPeriapsis2;

    Random = _CommonGenerator(_RandomEngine) * 2.0f * kPi;
    float InitialTrueAnomaly1 = Random;
    float InitialTrueAnomaly2 = 0.0f;
    if (InitialTrueAnomaly1 >= kPi) {
        InitialTrueAnomaly2 = InitialTrueAnomaly1 - kPi;
    } else {
        InitialTrueAnomaly2 = InitialTrueAnomaly1 + kPi;
    }

    Astro::StellarSystem::Orbit::OrbitalObject Star1(
        System.StarData().front().get(), Astro::StellarSystem::Orbit::ObjectType::kStar, InitialTrueAnomaly1);
    Astro::StellarSystem::Orbit::OrbitalObject Star2(
        System.StarData().back().get(),  Astro::StellarSystem::Orbit::ObjectType::kStar, InitialTrueAnomaly2);

    Orbits[0].Objects.emplace_back(Star1);
    Orbits[1].Objects.emplace_back(Star2);

    System.OrbitData().emplace_back(std::make_unique<Astro::StellarSystem::Orbit>(Orbits[0]));
    System.OrbitData().emplace_back(std::make_unique<Astro::StellarSystem::Orbit>(Orbits[1]));

#ifdef DEBUG_OUTPUT
    std::println("Semi-major axis of binary stars: {} AU",          BinarySemiMajorAxis     / kAuToMeter);
    std::println("Semi-major axis of binary first star: {} AU",     Orbits[0].SemiMajorAxis / kAuToMeter);
    std::println("Semi-major axis of binary second star: {} AU",    Orbits[1].SemiMajorAxis / kAuToMeter);
    std::println("Period of binary: {} days",                       Period / kDayToSeconds);
    std::println("Eccentricity of binary: {}",                      Eccentricity);
    std::println("Argument of periapsis of binary first star: {}",  ArgumentOfPeriapsis1);
    std::println("Argument of periapsis of binary second star: {}", ArgumentOfPeriapsis2);
    std::println("Initial true anomaly of binary first star: {}",   InitialTrueAnomaly1);
    std::println("Initial true anomaly of binary second star: {}",  InitialTrueAnomaly2);
    std::println("Normal of binary first star: ({}, {})",           StarNormals[0].x, StarNormals[1].y);
    std::println("Normal of binary second star: ({}, {})",          StarNormals[0].x, StarNormals[1].y);
    std::println("");
#endif // DEBUG_OUTPUT
}

void OrbitalGenerator::GeneratePlanets(
    std::size_t StarIndex,
    Astro::StellarSystem::Orbit::OrbitalObject& ParentStar,
    Astro::StellarSystem& System
) {
    // 变量名未标注单位均为国际单位制
    const Astro::Star* Star = System.StarData()[StarIndex].get();
    if (Star->GetFeH() < -2.0f) {
        return;
    }

    float BinarySemiMajorAxis = 0.0f;
    if (System.StarData().size() > 1) {
        BinarySemiMajorAxis = System.OrbitData()[0]->SemiMajorAxis + System.OrbitData()[1]->SemiMajorAxis;
    }

    // 生成原行星盘数据
    PlanetaryDisk PlanetaryDiskTempData;
    float DiskBase = 1.0f + _CommonGenerator(_RandomEngine); // 基准随机数，1-2 之间
    float StarInitialMassSol = Star->GetInitialMass() / kSolarMass;
    auto  StarType = Star->GetStellarClass().GetStarType();
    if (StarType != Util::StellarClass::StarType::kNeutronStar && StarType != Util::StellarClass::StarType::kBlackHole) {
        float DiskMassSol = DiskBase * StarInitialMassSol *
            std::pow(10.0f, -2.05f + 0.1214f * StarInitialMassSol - 0.02669f *
            std::pow(StarInitialMassSol, 2.0f) - 0.2274f * std::log(StarInitialMassSol));
        float DustMassSol = DiskMassSol * 0.0142f * 0.4f * std::pow(10.0f, Star->GetFeH());
        float OuterRadiusAu = StarInitialMassSol >= 1 ? 45.0f * StarInitialMassSol : 45.0f * std::pow(StarInitialMassSol, 2.0f);
        float InterRadiusAu = 0.0f;
        float DiskCoefficient = 0.0f;
        if (StarInitialMassSol < 0.6f) {
            DiskCoefficient = 2100;
        } else if (StarInitialMassSol < 1.5f) {
            DiskCoefficient = 1400;
        } else {
            DiskCoefficient = 1700;
        }

        float CommonCoefficient =
            (std::pow(10.0f, 2.0f - StarInitialMassSol) + 1.0f) * (static_cast<float>(kSolarLuminosity) /
            (4 * kPi * kStefanBoltzmann * std::pow(DiskCoefficient, 4.0f)));

        float InterRadiusAuSquared = 0.0f;
        if (StarInitialMassSol >= 0.075f && StarInitialMassSol < 0.43f) {
            InterRadiusAuSquared = CommonCoefficient * (0.23f * std::pow(StarInitialMassSol, 2.3f));
        } else if (StarInitialMassSol >= 0.43f && StarInitialMassSol < 2.0f) {
            InterRadiusAuSquared = CommonCoefficient * std::pow(StarInitialMassSol, 4.0f);
        } else if (StarInitialMassSol >= 2.0f && StarInitialMassSol <= 12.0f) {
            InterRadiusAuSquared = CommonCoefficient * (1.5f * std::pow(StarInitialMassSol, 3.5f));
        }

        InterRadiusAu = std::sqrt(InterRadiusAuSquared) / kAuToMeter; // 转化为 AU

        PlanetaryDiskTempData.InterRadiusAu = InterRadiusAu;
        PlanetaryDiskTempData.OuterRadiusAu = OuterRadiusAu;
        PlanetaryDiskTempData.DiskMassSol = DiskMassSol;
        PlanetaryDiskTempData.DustMassSol = DustMassSol;
    } else if (Star->GetStarFrom() == Astro::Star::StarFrom::kWhiteDwarfMerge) {
        DiskBase = std::pow(10.0f, -1.0f) + _CommonGenerator(_RandomEngine) * (1.0f - std::pow(10.0f, -1.0f));
        float StarMassSol = static_cast<float>(Star->GetMass() / kSolarMass);
        float DiskMassSol = DiskBase * 1e-5f * StarMassSol;
        PlanetaryDiskTempData.InterRadiusAu = 0.02f; // 高于洛希极限
        PlanetaryDiskTempData.OuterRadiusAu = 1.0f;
        PlanetaryDiskTempData.DiskMassSol = DiskMassSol;
        PlanetaryDiskTempData.DustMassSol = DiskMassSol;
    } else {
        return;
    }

#ifdef DEBUG_OUTPUT
    std::println("Planetary disk inter radius: {} AU", PlanetaryDiskTempData.InterRadiusAu);
    std::println("Planetary disk outer radius: {} AU", PlanetaryDiskTempData.OuterRadiusAu);
    std::println("Planetary disk mass: {} solar",      PlanetaryDiskTempData.DiskMassSol);
    std::println("Planetary disk dust mass: {} solar", PlanetaryDiskTempData.DustMassSol);
    std::println("");
#endif // DEUB_OUTPUT

    // 生成行星们
    std::size_t PlanetCount = 0;
    if (StarType != Util::StellarClass::StarType::kNeutronStar && StarType != Util::StellarClass::StarType::kBlackHole) {
        if (StarInitialMassSol < 0.6f) {
            PlanetCount = static_cast<std::size_t>(4.0f + _CommonGenerator(_RandomEngine) * 4.0f);
        } else if (StarInitialMassSol < 0.9f) {
            PlanetCount = static_cast<std::size_t>(5.0f + _CommonGenerator(_RandomEngine) * 5.0f);
        } else if (StarInitialMassSol < 3.0f) {
            PlanetCount = static_cast<std::size_t>(6.0f + _CommonGenerator(_RandomEngine) * 6.0f);
        } else {
            PlanetCount = static_cast<std::size_t>(4.0f + _CommonGenerator(_RandomEngine) * 4.0f);
        }
    } else if (Star->GetStarFrom() == Astro::Star::StarFrom::kWhiteDwarfMerge) {
        PlanetCount = static_cast<std::size_t>(2.0f + _CommonGenerator(_RandomEngine) * 2.0f);
    }

    std::vector<std::unique_ptr<Astro::Planet>> Planets;
    std::vector<std::unique_ptr<Astro::AsteroidCluster>> AsteroidClusters;

    Planets.reserve(PlanetCount);
    for (std::size_t i = 0; i < PlanetCount; ++i) {
        Planets.emplace_back(std::make_unique<Astro::Planet>());
    }

    // 生成行星初始核心质量
    std::vector<float> CoreBase(PlanetCount, 0.0f);
    for (float& Num : CoreBase) {
        Num = _CommonGenerator(_RandomEngine) * 3.0f;
    }

    float CoreBaseSum = 0.0f;
    for (float& Num : CoreBase) {
        CoreBaseSum += std::pow(10.0f, Num);
    }

    Astro::ComplexMass CoreMass;
    std::vector<float> CoreMassesSol(PlanetCount); // 初始核心质量，单位太阳
    for (std::size_t i = 0; i < PlanetCount; ++i) {
        CoreMassesSol[i] = PlanetaryDiskTempData.DustMassSol * std::pow(10.0f, CoreBase[i]) / CoreBaseSum;
        auto InitialCoreMass = boost::multiprecision::uint128_t(kSolarMass * CoreMassesSol[i]);

        int VolatilesRate = 9000 + static_cast<int>(_CommonGenerator(_RandomEngine)) + 2000;
        int EnergeticNuclideRate = 4500000 + static_cast<int>(_CommonGenerator(_RandomEngine)) * 1000000;

        CoreMass.Volatiles = InitialCoreMass / VolatilesRate;
        CoreMass.EnergeticNuclide = InitialCoreMass / EnergeticNuclideRate;
        CoreMass.Z = InitialCoreMass - CoreMass.Volatiles - CoreMass.EnergeticNuclide;

        Planets[i]->SetCoreMass(CoreMass);
#ifdef DEBUG_OUTPUT
        std::println("Generate initial core mass: planet {} initial core mass: {} earth",
                     i + 1, CoreMassesSol[i] * kSolarMassToEarth);
#endif // DEBUG_OUTPUT
    }

#ifdef DEBUG_OUTPUT
    std::println("");
#endif // DEBUG_OUTPUT

    // 初始化轨道
    std::vector<std::unique_ptr<Astro::StellarSystem::Orbit>> Orbits;
    for (std::size_t i = 0; i != PlanetCount; ++i) {
        Orbits.emplace_back(std::make_unique<Astro::StellarSystem::Orbit>());
    }

    for (auto& Orbit : Orbits) {
        Orbit->Parent.StarPtr = Star; // 轨道上级天体
        Orbit->ParentType = Astro::StellarSystem::Orbit::ObjectType::kStar;
        ParentStar.DirectOrbits.emplace_back(Orbit.get());
    }

    // 生成初始轨道半长轴
    std::vector<float> DiskBoundariesAu(PlanetCount + 1);
    DiskBoundariesAu[0] = PlanetaryDiskTempData.InterRadiusAu;

    float CoreMassSum = 0.0f;
    for (float Num : CoreMassesSol) {
        CoreMassSum += std::pow(Num, 0.1f);
    }

    std::vector<float> PartCoreMassSums(PlanetCount + 1, 0.0f);
    for (std::size_t i = 1; i <= PlanetCount; ++i) {
        PartCoreMassSums[i] = PartCoreMassSums[i - 1] + std::pow(CoreMassesSol[i - 1], 0.1f);
    }

    for (std::size_t i = 0; i < PlanetCount; ++i) {
        DiskBoundariesAu[i + 1] = PlanetaryDiskTempData.InterRadiusAu * std::pow(
            PlanetaryDiskTempData.OuterRadiusAu / PlanetaryDiskTempData.InterRadiusAu,
            PartCoreMassSums[i + 1] / CoreMassSum
        );
        float SemiMajorAxis = kAuToMeter * (DiskBoundariesAu[i] + DiskBoundariesAu[i + 1]) / 2.0f;
        Orbits[i]->SemiMajorAxis = SemiMajorAxis;
        GenerateOrbitElements(*Orbits[i].get()); // 生成剩余的根数
#ifdef DEBUG_OUTPUT
        std::println("Generate initial semi-major axis: planet {} initial semi-major axis: {} AU",
                     i + 1, Orbits[i]->SemiMajorAxis / kAuToMeter);
#endif // DEBUG_OUTPUT
    }

#ifdef DEBUG_OUTPUT
    std::println("");
#endif // DEBUG_OUTPUT

    // 计算原行星盘年龄
    float DiskAge = 8.15e6f + 8.3e5f * StarInitialMassSol - 33854 *
        std::pow(StarInitialMassSol, 2.0f) - 5.031e6f * std::log(StarInitialMassSol);

    if (std::to_underlying(Star->GetEvolutionPhase()) <= 9 && DiskAge >= Star->GetAge()) {
        for (auto& Planet : Planets) {
            Planet->SetPlanetType(Astro::Planet::PlanetType::kRockyAsteroidCluster);
        }
    }

    // 抹掉位于（双星）稳定区域以外的恒星
    if (System.StarData().size() > 1) {
        const Astro::Star* Current  = System.StarData()[StarIndex].get();
        const Astro::Star* TheOther = System.StarData()[1 - StarIndex].get();

        float Eccentricity = System.OrbitData()[0]->Eccentricity;
        float Mu = static_cast<float>(TheOther->GetMass() / (Current->GetMass() + TheOther->GetMass()));
        float StableBoundaryLimit = BinarySemiMajorAxis *
            (0.464f - 0.38f * Mu - 0.361f * Eccentricity + 0.586f * Mu * Eccentricity +
             0.15f * std::pow(Eccentricity, 2.0f) - 0.198f * Mu * std::pow(Eccentricity, 2.0f));
    
        for (std::size_t i = 0; i < PlanetCount; ++i) {
            if (Orbits[i]->SemiMajorAxis > StableBoundaryLimit) {
                Planets.erase(Planets.begin() + i, Planets.end());
                Orbits.erase(Orbits.begin() + i, Orbits.end());
                CoreMassesSol.erase(CoreMassesSol.begin() + i, CoreMassesSol.end());
                PlanetCount = Planets.size();
            }
        }
    }

    std::vector<float> NewCoreMassesSol(PlanetCount); // 吸积核心质量，单位太阳
    float MigratedOriginSemiMajorAxisAu = 0.0f;       // 原有的半长轴，用于计算内迁行星

    // Short Lambda functions
    // ----------------------
    auto ErasePlanets = [&](float Limit) -> void { // 抹掉位于临界线以内的行星
        for (std::size_t i = 0; i < PlanetCount; ++i) {
            if (Orbits[0]->SemiMajorAxis < Limit) {
                Planets.erase(Planets.begin());
                Orbits.erase(Orbits.begin());
                NewCoreMassesSol.erase(NewCoreMassesSol.begin());
                CoreMassesSol.erase(CoreMassesSol.begin());
                --PlanetCount;
            }
        }
    };

    StarType = Star->GetStellarClass().GetStarType();
    if (StarType != Util::StellarClass::StarType::kNeutronStar && StarType != Util::StellarClass::StarType::kBlackHole) {
        // 宜居带半径，单位 AU
        std::pair<float, float> HabitableZoneAu;

        if (System.StarData().size() > 1) {
            const Astro::Star* Current  = System.StarData()[StarIndex].get();
            const Astro::Star* TheOther = System.StarData()[1 - StarIndex].get();

            float CurrentLuminosity  = static_cast<float>(Current->GetLuminosity());
            float TheOtherLuminoisty = static_cast<float>(TheOther->GetLuminosity());
        
            HabitableZoneAu.first = std::sqrt(CurrentLuminosity /
                (4 * kPi * (3000 - TheOtherLuminoisty / (4 * kPi * std::pow(BinarySemiMajorAxis, 2.0f))))) / kAuToMeter;
            HabitableZoneAu.second = std::sqrt(CurrentLuminosity /
                (4 * kPi * (600  - TheOtherLuminoisty / (4 * kPi * std::pow(BinarySemiMajorAxis, 2.0f))))) / kAuToMeter;
        } else {
            float StarLuminosity   = static_cast<float>(Star->GetLuminosity());
            HabitableZoneAu.first  = std::sqrt(StarLuminosity / (4 * kPi * 3000)) / kAuToMeter;
            HabitableZoneAu.second = std::sqrt(StarLuminosity / (4 * kPi * 600))  / kAuToMeter;
        }

#ifdef DEBUG_OUTPUT
        std::println("Circumstellar habitable zone: {} - {} AU", HabitableZoneAu.first, HabitableZoneAu.second);
        std::println("");
#endif // DEBUG_OUTPUT

        // 冻结线半径，单位 AU
        float FrostLineAu        = 0.0f;
        float FrostLineAuSquared = 0.0f;
        if (System.StarData().size() > 1) {
            const Astro::Star* Current  = System.StarData()[StarIndex].get();
            const Astro::Star* TheOther = System.StarData()[1 - StarIndex].get();

            float CurrentPrevMainSequenceLuminosity  =
                CalculatePrevMainSequenceLuminosity(Current->GetInitialMass()  / kSolarMass);
            float TheOtherPrevMainSequenceLuminosity =
                CalculatePrevMainSequenceLuminosity(TheOther->GetInitialMass() / kSolarMass);

            FrostLineAuSquared = (CurrentPrevMainSequenceLuminosity / (4 * kPi * ((kStefanBoltzmann * std::pow(270.0f, 4.0f)) -
                TheOtherPrevMainSequenceLuminosity / (4 * kPi * std::pow(BinarySemiMajorAxis, 2.0f)))));
        } else {
            float PrevMainSequenceLuminosity = CalculatePrevMainSequenceLuminosity(StarInitialMassSol);
            FrostLineAuSquared = (PrevMainSequenceLuminosity / (4 * kPi * kStefanBoltzmann * std::pow(270.0f, 4.0f)));
        }

        FrostLineAu = std::sqrt(FrostLineAuSquared) / kAuToMeter;

#ifdef DEBUG_OUTPUT
        std::println("Frost line: {} AU", FrostLineAu);
        std::println("");
#endif // DEBUG_OUTPUT

        // 判断大行星
        PlanetCount = JudgeLargePlanets(StarIndex, System.StarData(), BinarySemiMajorAxis, HabitableZoneAu.first,
                                        FrostLineAu, CoreMassesSol, NewCoreMassesSol, Orbits, Planets);

#ifdef DEBUG_OUTPUT
        for (std::size_t i = 0; i < PlanetCount; ++i) {
            std::println("Before migration: planet {} semi-major axis: {} AU, initial core mass: {} earth, new core mass: {} earth, core radius: {} earth, type: {}",
                         i + 1, Orbits[i]->SemiMajorAxis / kAuToMeter, CoreMassesSol[i] * kSolarMassToEarth, NewCoreMassesSol[i] * kSolarMassToEarth, Planets[i]->GetRadius() / kEarthRadius, std::to_underlying(Planets[i]->GetPlanetType()));
        }

        std::println("");
#endif // DEBUG_OUTPUT

        // 巨行星内迁
        for (std::size_t i = 1; i < PlanetCount; ++i) {
            auto PlanetType = Planets[i]->GetPlanetType();
            if (PlanetType == Astro::Planet::PlanetType::kIceGiant || PlanetType == Astro::Planet::PlanetType::kGasGiant) {
                if (_MigrationProbability(_RandomEngine)) {
                    int MigrationIndex = 0;
                    if (_WalkInProbability(_RandomEngine)) { // 夺舍，随机生成在该行星之前的位置
                        MigrationIndex = static_cast<int>(_CommonGenerator(_RandomEngine) * (i - 1));
                    } else { // 不夺舍，直接迁移到最近轨道
                        float Coefficient = 0.0f;
                        float StarMassSol = static_cast<float>(Star->GetMass() / kSolarMass);
                        if (StarMassSol < 0.6) {
                            Coefficient = 2.0f;
                        } else if (StarMassSol < 1.2) {
                            Coefficient = 10.0f;
                        } else {
                            Coefficient = 7.0f;
                        }
                        float Lower = std::log10(PlanetaryDiskTempData.InterRadiusAu / Coefficient);
                        float Upper = std::log10(PlanetaryDiskTempData.InterRadiusAu * 0.67f);
                        float Exponent = Lower + _CommonGenerator(_RandomEngine) * (Upper - Lower);
                        Orbits[0]->SemiMajorAxis = std::pow(10.0f, Exponent) * kAuToMeter;
                    }

                    // 迁移到指定位置
                    Planets[i]->SetMigration(true);
                    Planets[MigrationIndex] = std::move(Planets[i]);
                    NewCoreMassesSol[MigrationIndex] = NewCoreMassesSol[i];
                    CoreMassesSol[MigrationIndex] = CoreMassesSol[i];
                    MigratedOriginSemiMajorAxisAu = Orbits[i]->SemiMajorAxis / kAuToMeter;
                    // 抹掉内迁途中的经过的其他行星
                    Planets.erase(Planets.begin() + MigrationIndex + 1, Planets.begin() + i + 1);
                    Orbits.erase(Orbits.begin() + MigrationIndex + 1, Orbits.begin() + i + 1);
                    NewCoreMassesSol.erase(NewCoreMassesSol.begin() + MigrationIndex + 1, NewCoreMassesSol.begin() + i + 1);
                    CoreMassesSol.erase(CoreMassesSol.begin() + MigrationIndex + 1, CoreMassesSol.begin() + i + 1);

                    PlanetCount = Planets.size();
                    break; // 只内迁一个行星
                } else {
                    break; // 给你机会你不中用
                }
            }
        }

        // 抹掉内迁坠入恒星或恒星膨胀过程中吞掉的行星
        float StarRadiusMaxSol = 0.0f; // 恒星膨胀过程中达到的最大半径
        if (std::to_underlying(Star->GetEvolutionPhase()) <= 1) {
            ErasePlanets(Star->GetRadius());
        } else {
            if (StarInitialMassSol < 0.75f) {
                StarRadiusMaxSol = 104 * std::pow(2.0f * StarInitialMassSol, 3.0f) + 0.1f;
            } else {
                StarRadiusMaxSol = 400 * std::pow(StarInitialMassSol - 0.75f, 1.0f / 3.0f);
            }

#ifdef DEBUG_OUTPUT
            std::println("Max star radius: {} solar", StarRadiusMaxSol);
            std::println("");
#endif // DEBUG_OUTPUT

            ErasePlanets(StarRadiusMaxSol * kSolarRadius);
        }

        // 判断冥府行星
        for (std::size_t i = 0; i != PlanetCount; ++i) {
            if ((Planets[i]->GetPlanetType() == Astro::Planet::PlanetType::kGasGiant ||
                 Planets[i]->GetPlanetType() == Astro::Planet::PlanetType::kIceGiant) &&
                Star->GetStellarClass().GetStarType() == Util::StellarClass::StarType::kWhiteDwarf &&
                Orbits[i]->SemiMajorAxis < 2.0f * StarRadiusMaxSol * kSolarRadius) {
                Planets[i]->SetPlanetType(Astro::Planet::PlanetType::kChthonian);
                NewCoreMassesSol[i] = CoreMassesSol[i];
                CalculatePlanetRadius(CoreMassesSol[i] * kSolarMassToEarth, Planets[i].get());
            }
        }

        // 处理白矮星引力散射
        for (std::size_t i = 0; i != PlanetCount; ++i) {
            if (Star->GetStellarClass().GetStarType() == Util::StellarClass::StarType::kWhiteDwarf && Star->GetAge() > 1e6) {
                if (Planets[i]->GetPlanetType() == Astro::Planet::PlanetType::kRocky) {
                    if (_ScatteringProbability(_RandomEngine)) {
                        float Random = 4.0f + _CommonGenerator(_RandomEngine) * 16.0f; // 4.0 Rsun 高于洛希极限
                        Orbits[i]->SemiMajorAxis = Random * kSolarRadius;
                        break;
                    }
                }
            }
        }

#ifdef DEBUG_OUTPUT
        for (std::size_t i = 0; i < PlanetCount; ++i) {
            std::println("Final orbits: planet {} semi-major axis: {} AU, initial core mass: {} earth, new core mass: {} earth, core radius: {} earth, type: {}",
                         i + 1, Orbits[i]->SemiMajorAxis / kAuToMeter, CoreMassesSol[i] * kSolarMassToEarth, NewCoreMassesSol[i] * kSolarMassToEarth, Planets[i]->GetRadius() / kEarthRadius, std::to_underlying(Planets[i]->GetPlanetType()));
        }

        std::println("");
#endif // DEBUG_OUTPUT

        for (std::size_t i = 0; i < PlanetCount; ++i) {
            Planets[i]->SetAge(DiskAge);
        }

        for (std::size_t i = 0; i < PlanetCount; ++i) {
            float PlanetMassEarth = 0.0f;
            auto PlanetType = Planets[i]->GetPlanetType();

            switch (PlanetType) {
            case Astro::Planet::PlanetType::kIcePlanet:
            case Astro::Planet::PlanetType::kOceanic:
            case Astro::Planet::PlanetType::kGasGiant:
            case Astro::Planet::PlanetType::kRockyAsteroidCluster:
            case Astro::Planet::PlanetType::kRockyIceAsteroidCluster:
                CalculatePlanetRadius(CalculatePlanetMassByIndex(i), Planets[i].get());
                break;
            case Astro::Planet::PlanetType::kIceGiant:
                if ((PlanetMassEarth = CalculatePlanetMassByIndex(i)) < 10.0f) {
                    Planets[i]->SetPlanetType(Astro::Planet::PlanetType::kSubIceGiant);
                }
                CalculatePlanetRadius(PlanetMassEarth, Planets[i].get());
                break;
            default:
                break;
            }

            PlanetType = Planets[i]->GetPlanetType();
            float PoyntingVector = 0.0f;

            if (System.StarData().size() > 1) {
                const Astro::Star* Current  = System.StarData()[StarIndex].get();
                const Astro::Star* TheOther = System.StarData()[1 - StarIndex].get();
                PoyntingVector =
                    static_cast<float>(Current->GetLuminosity())  / (4 * kPi * std::pow(Orbits[i]->SemiMajorAxis, 2.0f)) +
                    static_cast<float>(TheOther->GetLuminosity()) / (4 * kPi * std::pow(BinarySemiMajorAxis,     2.0f));
            } else {
                PoyntingVector = static_cast<float>(Star->GetLuminosity()) /
                    (4 * kPi * std::pow(Orbits[i]->SemiMajorAxis, 2.0f));
            }

#ifdef DEBUG_OUTPUT
            std::println("Planet {} poynting vector: {} W/m^2", i + 1, PoyntingVector);
#endif // DEBUG_OUTPUT
            // 判断热木星
            if (PoyntingVector >= 10000) {
                if (PlanetType == Astro::Planet::PlanetType::kGasGiant) {
                    Planets[i]->SetPlanetType(Astro::Planet::PlanetType::kHotGasGiant);
                } else if (PlanetType == Astro::Planet::PlanetType::kIceGiant) {
                    Planets[i]->SetPlanetType(Astro::Planet::PlanetType::kHotIceGiant);
                } else if (PlanetType == Astro::Planet::PlanetType::kSubIceGiant) {
                    Planets[i]->SetPlanetType(Astro::Planet::PlanetType::kHotSubIceGiant);
                }
            }

            PlanetType = Planets[i]->GetPlanetType();

            if (PlanetType == Astro::Planet::PlanetType::kHotIceGiant    ||
                PlanetType == Astro::Planet::PlanetType::kHotSubIceGiant ||
                PlanetType == Astro::Planet::PlanetType::kHotGasGiant) {
                Planets[i]->SetRadius(Planets[i]->GetRadius() * std::pow(PoyntingVector / 10000.0f, 0.094f));
            }

            if (PlanetType == Astro::Planet::PlanetType::kOceanic &&
                HabitableZoneAu.second <= Orbits[i]->SemiMajorAxis / kAuToMeter) {
                Planets[i]->SetPlanetType(Astro::Planet::PlanetType::kIcePlanet);
            }

            // 计算自转周期和扁率
            Astro::StellarSystem::StellarSystem::Orbit::OrbitalObject Parent(Star, Astro::StellarSystem::Orbit::ObjectType::kStar);
            GenerateSpin(Orbits[i]->SemiMajorAxis, Parent, Planets[i].get());

            // 计算类地行星、次生大气层和地壳矿脉
            if (Star->GetStellarClass().GetStarType() == Util::StellarClass::StarType::kNormalStar) {
                GenerateTerra(Star, PoyntingVector, HabitableZoneAu, Orbits[i].get(), Planets[i].get());
            }

            // 计算平衡温度
            CalculateTemperature(Astro::StellarSystem::Orbit::ObjectType::kStar, PoyntingVector, Planets[i].get());
            float BalanceTemperature = Planets[i]->GetBalanceTemperature();
            // 判断有没有被烧似
            if (((PlanetType != Astro::Planet::PlanetType::kRockyAsteroidCluster     &&
                  PlanetType != Astro::Planet::PlanetType::kRockyIceAsteroidCluster) && BalanceTemperature >= 2700) ||
                ((PlanetType == Astro::Planet::PlanetType::kRockyAsteroidCluster     ||
                  PlanetType == Astro::Planet::PlanetType::kRockyIceAsteroidCluster) && PoyntingVector > 1e6f)) {
                Planets.erase(Planets.begin() + i);
                CoreMassesSol.erase(CoreMassesSol.begin() + i);
                NewCoreMassesSol.erase(NewCoreMassesSol.begin() + i);
                Orbits.erase(Orbits.begin() + i);
                --PlanetCount;
                --i;
                continue;
            }

            // 生成卫星和行星环
            Astro::StellarSystem::Orbit::OrbitalObject MyPlanet(Planets[i].get(),
                                                                Astro::StellarSystem::Orbit::ObjectType::kPlanet);
            GenerateMoons(i, FrostLineAu, Star, PoyntingVector, HabitableZoneAu, MyPlanet, Orbits, Planets);

            if (PlanetType != Astro::Planet::PlanetType::kRockyAsteroidCluster    &&
                PlanetType != Astro::Planet::PlanetType::kRockyIceAsteroidCluster &&
                Planets[i]->GetMassFloat() > _RingsParentLowerLimit) {
                GenerateRings(i, std::numeric_limits<float>::infinity(), Star, MyPlanet, Orbits, AsteroidClusters);
            }

            // 生成生命和文明
            PlanetType = Planets[i]->GetPlanetType();
            if (PlanetType == Astro::Planet::PlanetType::kTerra) {
                GenerateCivilization(Star, PoyntingVector, HabitableZoneAu, Orbits[i].get(), Planets[i].get());
            }

            Orbits[i]->Objects.emplace_back(MyPlanet);

            // 生成特洛伊带
            GenerateTrojan(Star, FrostLineAu, Orbits[i].get(), MyPlanet, AsteroidClusters);
        }

        // 生成柯伊伯带
        AsteroidClusters.emplace_back(std::make_unique<Astro::AsteroidCluster>());
        float Exponent = 1.0f + _CommonGenerator(_RandomEngine);
        float KuiperBeltMass = PlanetaryDiskTempData.DustMassSol * std::pow(10.0f, Exponent) * 1e-4f * kSolarMass;
        float KuiperBeltRadiusAu = PlanetaryDiskTempData.OuterRadiusAu * (1.0f + _CommonGenerator(_RandomEngine) * 0.5f);
        float KuiperBeltMassVolatiles = 0.0f;
        float KuiperBeltMassEnergeticNuclide = 0.0f;
        float KuiperBeltMassZ = 0.0f;

        if (std::to_underlying(Star->GetEvolutionPhase()) < 1 && KuiperBeltRadiusAu > FrostLineAu) {
            AsteroidClusters.back()->SetAsteroidType(Astro::AsteroidCluster::AsteroidType::kRockyIce);
            KuiperBeltMassVolatiles = KuiperBeltMass * 0.064f;
            KuiperBeltMassEnergeticNuclide = KuiperBeltMass * 0.064f * 5e-6f;
            KuiperBeltMassZ = KuiperBeltMass - KuiperBeltMassVolatiles - KuiperBeltMassEnergeticNuclide;
        } else {
            AsteroidClusters.back()->SetAsteroidType(Astro::AsteroidCluster::AsteroidType::kRocky);
            KuiperBeltMassEnergeticNuclide = KuiperBeltMass * 5e-6f;
            KuiperBeltMassZ = KuiperBeltMass - KuiperBeltMassEnergeticNuclide;
        }

        Astro::StellarSystem::Orbit::OrbitalObject KuiperBelt(AsteroidClusters.back().get(),
                                                              Astro::StellarSystem::Orbit::ObjectType::kAsteroidCluster);

        Astro::StellarSystem::Orbit KuiperBeltOrbit;
        KuiperBeltOrbit.Objects.emplace_back(KuiperBelt);
        KuiperBeltOrbit.Parent.StarPtr = Star;
        KuiperBeltOrbit.ParentType = Astro::StellarSystem::Orbit::ObjectType::kStar;
        KuiperBeltOrbit.SemiMajorAxis = KuiperBeltRadiusAu * kAuToMeter;
        
        GenerateOrbitElements(KuiperBeltOrbit);

#ifdef DEBUG_OUTPUT
        std::println("");
        std::println("Kuiper belt details:");
        std::println("semi-major axis: {} AU, mass: {} moon, type: {}",
                     KuiperBeltOrbit.SemiMajorAxis / kAuToMeter, KuiperBeltMass / kMoonMass, std::to_underlying(AsteroidClusters.back()->GetAsteroidType()));
        std::println("mass z: {:.2E} kg, mass vol: {:.2E} kg, mass nuc: {:.2E} kg",
                     KuiperBeltMassZ, KuiperBeltMassVolatiles, KuiperBeltMassEnergeticNuclide);
        std::println("");
#endif // DEBUG_OUTPUT


        Orbits.emplace_back(std::make_unique<Astro::StellarSystem::Orbit>(KuiperBeltOrbit));
    } else {
        PlanetCount = JudgeLargePlanets(StarIndex, System.StarData(), BinarySemiMajorAxis,
                                        std::numeric_limits<float>::infinity(),
                                        std::numeric_limits<float>::infinity(),
                                        CoreMassesSol, NewCoreMassesSol, Orbits, Planets);

        for (std::size_t i = 0; i < PlanetCount; ++i) {
            Planets[i]->SetAge(Star->GetAge());
        }

        for (std::size_t i = 0; i < PlanetCount; ++i) {
#ifdef DEBUG_OUTPUT
            float PlanetMassEarth = Planets[i]->GetMassFloat() / kEarthMass;
            std::println("Final system: planet {} semi-major axis: {} AU, mass: {} earth, radius: {} earth, type: {}",
                         i + 1, Orbits[i]->SemiMajorAxis / kAuToMeter, PlanetMassEarth, Planets[i]->GetRadius() / kEarthRadius, std::to_underlying(Planets[i]->GetPlanetType()));
#endif // DEBUG_OUTPUT
            CalculatePlanetRadius(CoreMassesSol[i] * kSolarMassToEarth, Planets[i].get());

            Astro::StellarSystem::StellarSystem::Orbit::OrbitalObject Parent(Star, Astro::StellarSystem::Orbit::ObjectType::kStar);
            GenerateSpin(Orbits[i]->SemiMajorAxis, Parent, Planets[i].get());
            
            float PoyntingVector = static_cast<float>(Star->GetLuminosity()) / (4 * kPi * std::pow(Orbits[i]->SemiMajorAxis, 2.0f));
            CalculateTemperature(Astro::StellarSystem::Orbit::ObjectType::kStar, PoyntingVector, Planets[i].get());
            float BalanceTemperature = Planets[i]->GetBalanceTemperature();
            // 判断有没有被烧似
            auto PlanetType = Planets[i]->GetPlanetType();
            if (((PlanetType != Astro::Planet::PlanetType::kRockyAsteroidCluster     &&
                  PlanetType != Astro::Planet::PlanetType::kRockyIceAsteroidCluster) && BalanceTemperature >= 2700) ||
                ((PlanetType == Astro::Planet::PlanetType::kRockyAsteroidCluster     ||
                  PlanetType == Astro::Planet::PlanetType::kRockyIceAsteroidCluster) && PoyntingVector > 1e6f)) {
                Planets.erase(Planets.begin() + i);
                CoreMassesSol.erase(CoreMassesSol.begin() + i);
                NewCoreMassesSol.erase(NewCoreMassesSol.begin() + i);
                Orbits.erase(Orbits.begin() + i);
                --PlanetCount;
                --i;
                continue;
            }

            Astro::StellarSystem::Orbit::OrbitalObject MyPlanet(Planets[i].get(),
                                                                Astro::StellarSystem::Orbit::ObjectType::kPlanet);
            GenerateMoons(i, std::numeric_limits<float>::infinity(), Star, PoyntingVector, {}, MyPlanet, Orbits, Planets);

            if (PlanetType != Astro::Planet::PlanetType::kRockyAsteroidCluster    &&
                PlanetType != Astro::Planet::PlanetType::kRockyIceAsteroidCluster &&
                Planets[i]->GetMassFloat() > _RingsParentLowerLimit) {
                GenerateRings(i, std::numeric_limits<float>::infinity(), Star, MyPlanet, Orbits, AsteroidClusters);
            }

            Orbits[i]->Objects.emplace_back(MyPlanet);

            // 生成特洛伊带
            GenerateTrojan(Star, std::numeric_limits<float>::infinity(), Orbits[i].get(), MyPlanet, AsteroidClusters);
        }
    }

    for (auto& Orbit : Orbits) {
        if (Orbit->Objects.front().Type == Astro::StellarSystem::Orbit::ObjectType::kPlanet) {
            if (Orbit->Objects.front().Object.PlanetPtr->GetPlanetType() == Astro::Planet::PlanetType::kRockyAsteroidCluster ||
                Orbit->Objects.front().Object.PlanetPtr->GetPlanetType() == Astro::Planet::PlanetType::kRockyIceAsteroidCluster) {
                auto& Object = Orbit->Objects.front();
                auto AsteroidCluster = PlanetToAsteroidCluster(Orbit->Objects.front().Object.PlanetPtr);
                Object.Object.AsteroidClusterPtr = AsteroidCluster.get();
                Object.Type = Astro::StellarSystem::Orbit::ObjectType::kAsteroidCluster;
                AsteroidClusters.emplace_back(std::move(AsteroidCluster));
            }
        }
    }

#ifdef DEBUG_OUTPUT
    std::println("");

    for (std::size_t i = 0; i != PlanetCount; ++i) {
        auto& Planet                         = Planets[i];
        auto  PlanetType                     = Planet->GetPlanetType();
        float PlanetMass                     = Planet->GetMassFloat();
        float PlanetMassEarth                = PlanetMass / kEarthMass;
        float PlanetRadius                   = Planet->GetRadius();
        float PlanetRadiusEarth              = PlanetRadius / kEarthRadius;
        float AtmosphereMassZ                = Planet->GetAtmosphereMassZFloat();
        float AtmosphereMassVolatiles        = Planet->GetAtmosphereMassVolatilesFloat();
        float AtmosphereMassEnergeticNuclide = Planet->GetAtmosphereMassEnergeticNuclideFloat();
        float CoreMassZ                      = Planet->GetCoreMassZFloat();
        float CoreMassVolatiles              = Planet->GetCoreMassVolatilesFloat();
        float CoreMassEnergeticNuclide       = Planet->GetCoreMassEnergeticNuclideFloat();
        float OceanMassZ                     = Planet->GetOceanMassZFloat();;
        float OceanMassVolatiles             = Planet->GetOceanMassVolatilesFloat();
        float OceanMassEnergeticNuclide      = Planet->GetOceanMassEnergeticNuclideFloat();
        float CrustMineralMass               = Planet->GetCrustMineralMassFloat();
        float AtmospherePressure             = (kGravityConstant * PlanetMass * (AtmosphereMassZ + AtmosphereMassVolatiles + AtmosphereMassEnergeticNuclide)) / (4 * kPi * std::pow(Planets[i]->GetRadius(), 4.0f));
        float Oblateness                     = Planet->GetOblateness();
        float Spin                           = Planet->GetSpin();
        float BalanceTemperature             = Planet->GetBalanceTemperature();

        if (PlanetType != Astro::Planet::PlanetType::kRockyAsteroidCluster &&
            PlanetType != Astro::Planet::PlanetType::kRockyIceAsteroidCluster) {
            std::println("Planet {} details:", i + 1);
            std::println("semi-major axis: {} AU, mass: {} earth, radius: {} earth, type: {}",
                         Orbits[i]->SemiMajorAxis / kAuToMeter, PlanetMassEarth, PlanetRadiusEarth, std::to_underlying(PlanetType));
            std::println("rotation period: {} h, oblateness: {}, balance temperature: {} K",
                         Spin / 3600, Oblateness, BalanceTemperature);
            std::println("atmo  mass z: {:.2E} kg, atmo  mass vol: {:.2E} kg, atmo  mass nuc: {:.2E} kg",
                         AtmosphereMassZ, AtmosphereMassVolatiles, AtmosphereMassEnergeticNuclide);
            std::println("core  mass z: {:.2E} kg, core  mass vol: {:.2E} kg, core  mass nuc: {:.2E} kg",
                         CoreMassZ, CoreMassVolatiles, CoreMassEnergeticNuclide);
            std::println("ocean mass z: {:.2E} kg, ocean mass vol: {:.2E} kg, ocean mass nuc: {:.2E} kg",
                         OceanMassZ, OceanMassVolatiles, OceanMassEnergeticNuclide);
            std::println("crust mineral mass : {:.2E} kg, atmo pressure : {:.2f} atm",
                         CrustMineralMass, AtmospherePressure / kPascalToAtm);
        } else {
            std::println("Asteroid belt (origin planet {}) details:", i + 1);
            std::println("semi-major axis: {} AU, mass: {} moon, type: {}",
                         Orbits[i]->SemiMajorAxis / kAuToMeter, PlanetMass / kMoonMass, std::to_underlying(PlanetType));
            std::println("mass z: {:.2E} kg, mass vol: {:.2E} kg, mass nuc: {:.2E} kg",
                         CoreMassZ, CoreMassVolatiles, CoreMassEnergeticNuclide);
        }

        std::println("");
    }
#endif // DEBUG_OUTPUT

    for (std::size_t i = 0; i < PlanetCount; ++i) {
        if (Planets[i]->GetPlanetType() == Astro::Planet::PlanetType::kRockyAsteroidCluster ||
            Planets[i]->GetPlanetType() == Astro::Planet::PlanetType::kRockyIceAsteroidCluster) {
            Planets.erase(Planets.begin() + i);
            --i;
            --PlanetCount;
        }
    }

    System.PlanetData().reserve(System.PlanetData().size() + Planets.size());
    System.PlanetData().insert(System.PlanetData().end(),
                               std::make_move_iterator(Planets.begin()),
                               std::make_move_iterator(Planets.end()));

    System.OrbitData().reserve(System.OrbitData().size() + Orbits.size());
    System.OrbitData().insert(System.OrbitData().end(),
                               std::make_move_iterator(Orbits.begin()),
                               std::make_move_iterator(Orbits.end()));

    System.AsteroidClusterData().reserve(System.AsteroidClusterData().size() + AsteroidClusters.size());
    System.AsteroidClusterData().insert(System.AsteroidClusterData().end(),
                               std::make_move_iterator(AsteroidClusters.begin()),
                               std::make_move_iterator(AsteroidClusters.end()));
}

void OrbitalGenerator::GenerateOrbitElements(Astro::StellarSystem::Orbit& Orbit) {
    if (!Orbit.Eccentricity) {
        Orbit.Eccentricity = _CommonGenerator(_RandomEngine) * 0.05f;
    }

    if (!Orbit.Inclination) {
        Orbit.Inclination = _CommonGenerator(_RandomEngine) * 4.0f - 2.0f;
    }

    if (!Orbit.LongitudeOfAscendingNode) {
        Orbit.LongitudeOfAscendingNode = _CommonGenerator(_RandomEngine) * 2 * kPi;
    }

    if (!Orbit.ArgumentOfPeriapsis) {
        Orbit.ArgumentOfPeriapsis = _CommonGenerator(_RandomEngine) * 2 * kPi;
    }

    if (!Orbit.TrueAnomaly) {
        Orbit.TrueAnomaly = _CommonGenerator(_RandomEngine) * 2 * kPi;
    }
}

std::size_t OrbitalGenerator::JudgeLargePlanets(
    std::size_t StarIndex,
    const std::vector<std::unique_ptr<Astro::Star>>& StarData,
    float BinarySemiMajorAxis,
    float InterHabitableZoneRadiusAu,
    float FrostLineAu,
    std::vector<float>& CoreMassesSol,
    std::vector<float>& NewCoreMassesSol,
    std::vector<std::unique_ptr<Astro::StellarSystem::Orbit>>& Orbits,
    std::vector<std::unique_ptr<Astro::Planet>>& Planets
) {
    const Astro::Star* Star = StarData[StarIndex].get();
    auto StarType = Star->GetStellarClass().GetStarType();
    std::size_t PlanetCount = CoreMassesSol.size();

    for (std::size_t i = 0; i < PlanetCount; ++i) {
        NewCoreMassesSol[i] = Orbits[i]->SemiMajorAxis / kAuToMeter > FrostLineAu ? CoreMassesSol[i] * 2.35f : CoreMassesSol[i];

        // 计算前主序平衡温度
        float PlanetBalanceTemperatureWhenStarAtPrevMainSequence = 0.0f;
        float PlanetBalanceTemperatureWhenStarAtPrevMainSequenceQuadraticed = 0.0f;

        if (StarData.size() == 1) {
            float PrevMainSequenceLuminosity = CalculatePrevMainSequenceLuminosity(Star->GetInitialMass() / kSolarMass);
            PlanetBalanceTemperatureWhenStarAtPrevMainSequenceQuadraticed =
                PrevMainSequenceLuminosity / (4 * kPi * std::pow(Orbits[i]->SemiMajorAxis, 2.0f)) / kStefanBoltzmann;
        } else {
            const Astro::Star* Current  = StarData[StarIndex].get();
            const Astro::Star* TheOther = StarData[1 - StarIndex].get();

            float CurrentPrevMainSequenceLuminosity  =
                CalculatePrevMainSequenceLuminosity(Current->GetInitialMass()  / kSolarMass);
            float TheOtherPrevMainSequenceLuminosity =
                CalculatePrevMainSequenceLuminosity(TheOther->GetInitialMass() / kSolarMass);

            PlanetBalanceTemperatureWhenStarAtPrevMainSequenceQuadraticed =
              (CurrentPrevMainSequenceLuminosity  / (4 * kPi * std::pow(Orbits[i]->SemiMajorAxis, 2.0f)) +
               TheOtherPrevMainSequenceLuminosity / (4 * kPi * std::pow(BinarySemiMajorAxis,     2.0f))) / kStefanBoltzmann;
        }

        PlanetBalanceTemperatureWhenStarAtPrevMainSequence =
            std::pow(PlanetBalanceTemperatureWhenStarAtPrevMainSequenceQuadraticed, 0.25f);

        // 开除大行星
        if (NewCoreMassesSol[i] * kSolarMass < _AsteroidUpperLimit ||
            Planets[i]->GetPlanetType() == Astro::Planet::PlanetType::kRockyAsteroidCluster) {
            if (NewCoreMassesSol[i] * kSolarMass < 1e19f) {
                Orbits.erase(Orbits.begin() + i);
                Planets.erase(Planets.begin() + i);
                NewCoreMassesSol.erase(NewCoreMassesSol.begin() + i);
                CoreMassesSol.erase(CoreMassesSol.begin() + i);
                --i;
                --PlanetCount;
                continue;
            }

            if (std::to_underlying(Star->GetEvolutionPhase()) < 1 && Orbits[i]->SemiMajorAxis / kAuToMeter > FrostLineAu) {
                Planets[i]->SetPlanetType(Astro::Planet::PlanetType::kRockyIceAsteroidCluster);
            } else {
                Planets[i]->SetPlanetType(Astro::Planet::PlanetType::kRockyAsteroidCluster);
            }
        } else {
            if (Planets[i]->GetPlanetType() != Astro::Planet::PlanetType::kRockyAsteroidCluster &&
                Planets[i]->GetPlanetType() != Astro::Planet::PlanetType::kRockyIceAsteroidCluster &&
                CoreMassesSol[i] * kSolarMassToEarth < 0.1f && _AsteroidBeltProbability(_RandomEngine)) {
                if (std::to_underlying(Star->GetEvolutionPhase()) < 1 && Orbits[i]->SemiMajorAxis / kAuToMeter > FrostLineAu) {
                    Planets[i]->SetPlanetType(Astro::Planet::PlanetType::kRockyIceAsteroidCluster);
                } else {
                    Planets[i]->SetPlanetType(Astro::Planet::PlanetType::kRockyAsteroidCluster);
                }

                float Exponent = -3.0f + _CommonGenerator(_RandomEngine) * 3.0f;
                float DiscountCoefficient = std::pow(10.0f, Exponent);
                CoreMassesSol[i] *= DiscountCoefficient; // 对核心质量打个折扣
            } else {
                // 计算初始核心半径
                if (Orbits[i]->SemiMajorAxis / kAuToMeter < FrostLineAu) {
                    Planets[i]->SetPlanetType(Astro::Planet::PlanetType::kRocky);
                } else {
                    Planets[i]->SetPlanetType(Astro::Planet::PlanetType::kIcePlanet);
                }

                CalculatePlanetRadius(NewCoreMassesSol[i] * kSolarMassToEarth, Planets[i].get());
                if (Star->GetStellarClass().GetStarType() == Util::StellarClass::StarType::kBlackHole ||
                    Star->GetStellarClass().GetStarType() == Util::StellarClass::StarType::kNeutronStar) {
                    continue;
                }

                float CommonCoefficient = PlanetBalanceTemperatureWhenStarAtPrevMainSequence * 4.638759e16f;

                if ((NewCoreMassesSol[i] * kSolarMass / Planets[i]->GetRadius()) > (CommonCoefficient / 4.0f)) {
                    Planets[i]->SetPlanetType(Astro::Planet::PlanetType::kGasGiant);
                } else if ((NewCoreMassesSol[i] * kSolarMass / Planets[i]->GetRadius()) > (CommonCoefficient / 8.0f)) {
                    Planets[i]->SetPlanetType(Astro::Planet::PlanetType::kIceGiant);
                } else if ((CoreMassesSol[i] * kSolarMass / Planets[i]->GetRadius()) > (CommonCoefficient / 18.0f) &&
                    Orbits[i]->SemiMajorAxis / kAuToMeter > InterHabitableZoneRadiusAu &&
                    Orbits[i]->SemiMajorAxis / kAuToMeter < FrostLineAu && std::to_underlying(Star->GetEvolutionPhase()) < 1) {
                    Planets[i]->SetPlanetType(Astro::Planet::PlanetType::kOceanic);
                } else {
                    if (Orbits[i]->SemiMajorAxis / kAuToMeter > FrostLineAu) {
                        Planets[i]->SetPlanetType(Astro::Planet::PlanetType::kIcePlanet);
                    } else {
                        Planets[i]->SetPlanetType(Astro::Planet::PlanetType::kRocky);
                    }

                    CalculatePlanetRadius(NewCoreMassesSol[i] * kSolarMassToEarth, Planets[i].get());
                }
            }
        }
    }

    return PlanetCount;
}

float OrbitalGenerator::CalculatePlanetMass(
    float CoreMass,
    float NewCoreMass,
    float SemiMajorAxisAu,
    const PlanetaryDisk& PlanetaryDiskTempData,
    const Astro::Star* Star,
    Astro::Planet* Planet
) {
    float Random1 = 0.0f;
    float Random2 = 0.0f;
    float Random3 = 0.0f;

    float AtmosphereMassVolatiles        = 0.0f;
    float AtmosphereMassEnergeticNuclide = 0.0f;
    float AtmosphereMassZ                = 0.0f;
    float CoreMassVolatiles              = 0.0f;
    float CoreMassEnergeticNuclide       = 0.0f;
    float CoreMassZ                      = 0.0f;
    float OceanMassVolatiles             = 0.0f;
    float OceanMassEnergeticNuclide      = 0.0f;
    float OceanMassZ                     = 0.0f;

    auto CalculateIcePlanetMass = [&]() -> float {
        Random2 = 0.9f + _CommonGenerator(_RandomEngine) * 0.2f;
        Random3 = 0.9f + _CommonGenerator(_RandomEngine) * 0.2f;

        if (std::to_underlying(Star->GetEvolutionPhase()) < 1) {
            OceanMassVolatiles = CoreMass * 0.15f;
            OceanMassEnergeticNuclide = 0.15f * 5e-5f * CoreMass;
            OceanMassZ = CoreMass * 1.35f - OceanMassVolatiles - OceanMassEnergeticNuclide;
        } else {
            Planet->SetPlanetType(Astro::Planet::PlanetType::kRocky);
        }

        CoreMassVolatiles = CoreMass * 1e-4f * Random2;
        CoreMassEnergeticNuclide = CoreMass * 5e-6f * Random3;
        CoreMassZ = CoreMass - CoreMassVolatiles - CoreMassEnergeticNuclide;

        Planet->SetOceanMass({
            boost::multiprecision::uint128_t(OceanMassZ),
            boost::multiprecision::uint128_t(OceanMassVolatiles),
            boost::multiprecision::uint128_t(OceanMassEnergeticNuclide)
        });

        Planet->SetCoreMass({
            boost::multiprecision::uint128_t(CoreMassZ),
            boost::multiprecision::uint128_t(CoreMassVolatiles),
            boost::multiprecision::uint128_t(CoreMassEnergeticNuclide)
        });

        return (OceanMassVolatiles + OceanMassEnergeticNuclide + OceanMassZ +
                CoreMassVolatiles  + CoreMassEnergeticNuclide  + CoreMassZ) / kEarthMass;
    };

    auto CalculateOceanicMass = [&]() -> float {
        Random1 = _CommonGenerator(_RandomEngine) * 1.35f;
        Random2 = 0.9f + _CommonGenerator(_RandomEngine) * 0.2f;
        Random3 = 0.9f + _CommonGenerator(_RandomEngine) * 0.2f;

        if (std::to_underlying(Star->GetEvolutionPhase()) < 1) {
            OceanMassVolatiles = (CoreMass * Random1) / 9.0f;
            OceanMassEnergeticNuclide = 5e-5f * OceanMassVolatiles;
            OceanMassZ = CoreMass * Random1 - OceanMassVolatiles - OceanMassEnergeticNuclide;
        } else {
            Planet->SetPlanetType(Astro::Planet::PlanetType::kRocky);
        }

        CoreMassVolatiles = CoreMass * 1e-4f * Random2;
        CoreMassEnergeticNuclide = CoreMass * 5e-6f * Random3;
        CoreMassZ = CoreMass - CoreMassVolatiles - CoreMassEnergeticNuclide;

        Planet->SetOceanMass({
            boost::multiprecision::uint128_t(OceanMassZ),
            boost::multiprecision::uint128_t(OceanMassVolatiles),
            boost::multiprecision::uint128_t(OceanMassEnergeticNuclide)
        });

        Planet->SetCoreMass({
            boost::multiprecision::uint128_t(CoreMassZ),
            boost::multiprecision::uint128_t(CoreMassVolatiles),
            boost::multiprecision::uint128_t(CoreMassEnergeticNuclide)
        });

        return (OceanMassVolatiles + OceanMassEnergeticNuclide + OceanMassZ +
                CoreMassVolatiles  + CoreMassEnergeticNuclide  + CoreMassZ) / kEarthMass;
    };

    auto CalculateIceGiantMass = [&]() -> float {
        Random1 = 2.0f + _CommonGenerator(_RandomEngine) * (std::log10(20.0f) - std::log10(2.0f));
        Random2 = 0.9f + _CommonGenerator(_RandomEngine) * 0.2f;
        Random3 = 0.9f + _CommonGenerator(_RandomEngine) * 0.2f;

        float CommonCoefficient = (0.5f + 0.5f * (SemiMajorAxisAu - PlanetaryDiskTempData.InterRadiusAu) /
            (PlanetaryDiskTempData.OuterRadiusAu - PlanetaryDiskTempData.InterRadiusAu)) * Random1;

        AtmosphereMassVolatiles = (NewCoreMass - CoreMass) / 9.0f + CoreMass * CommonCoefficient / 6.0f;
        AtmosphereMassEnergeticNuclide = 5e-5f * AtmosphereMassVolatiles;
        AtmosphereMassZ = CoreMass * CommonCoefficient + (NewCoreMass - CoreMass) -
            AtmosphereMassVolatiles - AtmosphereMassEnergeticNuclide;

        CoreMassVolatiles = CoreMass * 1e-4f * Random2;
        CoreMassEnergeticNuclide = CoreMass * 5e-6f * Random3;
        CoreMassZ = CoreMass - CoreMassVolatiles - CoreMassEnergeticNuclide;

        Planet->SetAtmosphereMass({
            boost::multiprecision::uint128_t(AtmosphereMassZ),
            boost::multiprecision::uint128_t(AtmosphereMassVolatiles),
            boost::multiprecision::uint128_t(AtmosphereMassEnergeticNuclide)
        });

        Planet->SetCoreMass({
            boost::multiprecision::uint128_t(CoreMassZ),
            boost::multiprecision::uint128_t(CoreMassVolatiles),
            boost::multiprecision::uint128_t(CoreMassEnergeticNuclide)
        });

        Planet->SetPlanetType(Astro::Planet::PlanetType::kIceGiant);

        return (AtmosphereMassVolatiles + AtmosphereMassEnergeticNuclide + AtmosphereMassZ +
                CoreMassVolatiles       + CoreMassEnergeticNuclide       + CoreMassZ) / kEarthMass;
    };

    auto CalculateGasGiantMass = [&]() -> float {
        Random1 = 7.0f + _CommonGenerator(_RandomEngine) * (std::min(50.0f, 1.0f / 0.0142f * std::pow(10.0f, Star->GetFeH())) - 7.0f);
        Random2 = 0.9f + _CommonGenerator(_RandomEngine) * 0.2f;
        Random3 = 0.9f + _CommonGenerator(_RandomEngine) * 0.2f;

        float CommonCoefficient = (0.5f + 0.5f * (SemiMajorAxisAu - PlanetaryDiskTempData.InterRadiusAu) /
            (PlanetaryDiskTempData.OuterRadiusAu - PlanetaryDiskTempData.InterRadiusAu)) * Random1;

        AtmosphereMassZ = (0.0142f * std::pow(10.0f, Star->GetFeH())) * CoreMass *
            CommonCoefficient + (1.0f - (1.0f + 5e-5f) / 9.0f) * (NewCoreMass - CoreMass);
        AtmosphereMassEnergeticNuclide = 5e-5f * (CoreMass * CommonCoefficient + (NewCoreMass - CoreMass) / 9.0f);
        AtmosphereMassVolatiles = CoreMass * CommonCoefficient + (NewCoreMass - CoreMass) -
            AtmosphereMassZ - AtmosphereMassEnergeticNuclide;

        CoreMassVolatiles = CoreMass * 1e-4f * Random2;
        CoreMassEnergeticNuclide = CoreMass * 5e-6f * Random3;
        CoreMassZ = CoreMass - CoreMassVolatiles - CoreMassEnergeticNuclide;

        Planet->SetAtmosphereMass({
            boost::multiprecision::uint128_t(AtmosphereMassZ),
            boost::multiprecision::uint128_t(AtmosphereMassVolatiles),
            boost::multiprecision::uint128_t(AtmosphereMassEnergeticNuclide)
        });

        Planet->SetCoreMass({
            boost::multiprecision::uint128_t(CoreMassZ),
            boost::multiprecision::uint128_t(CoreMassVolatiles),
            boost::multiprecision::uint128_t(CoreMassEnergeticNuclide)
        });

        Planet->SetPlanetType(Astro::Planet::PlanetType::kGasGiant);

        return (AtmosphereMassVolatiles + AtmosphereMassEnergeticNuclide + AtmosphereMassZ +
                CoreMassVolatiles       + CoreMassEnergeticNuclide       + CoreMassZ) / kEarthMass;
    };

    auto CalculateRockyAsteroidMass = [&]() -> float {
        Random2 = 0.9f + _CommonGenerator(_RandomEngine) * 0.2f;
        Random3 = 0.9f + _CommonGenerator(_RandomEngine) * 0.2f;

        CoreMassVolatiles = CoreMass * 1e-4f * Random2;
        CoreMassEnergeticNuclide = CoreMass * 5e-6f * Random3;
        CoreMassZ = CoreMass - CoreMassVolatiles - CoreMassEnergeticNuclide;

        Planet->SetCoreMass({
            boost::multiprecision::uint128_t(CoreMassZ),
            boost::multiprecision::uint128_t(CoreMassVolatiles),
            boost::multiprecision::uint128_t(CoreMassEnergeticNuclide)
        });

        return (CoreMassVolatiles + CoreMassEnergeticNuclide + CoreMassZ) / kEarthMass;
    };

    auto CalculateRockyIceAsteroidMass = [&]() -> float {
        Random2 = 0.9f + _CommonGenerator(_RandomEngine) * 0.2f;
        Random3 = 0.9f + _CommonGenerator(_RandomEngine) * 0.2f;

        OceanMassVolatiles = CoreMass * 0.15f;
        OceanMassEnergeticNuclide = 0.15f * 5e-5f * CoreMass;
        OceanMassZ = CoreMass * 1.35f - OceanMassVolatiles - OceanMassEnergeticNuclide;

        CoreMassVolatiles = CoreMass * 1e-4f * Random2;
        CoreMassEnergeticNuclide = CoreMass * 5e-6f * Random3;
        CoreMassZ = CoreMass - CoreMassVolatiles - CoreMassEnergeticNuclide;

        CoreMassVolatiles += OceanMassVolatiles;
        CoreMassEnergeticNuclide += OceanMassEnergeticNuclide;
        CoreMassZ += OceanMassZ;

        Planet->SetCoreMass({
            boost::multiprecision::uint128_t(CoreMassZ),
            boost::multiprecision::uint128_t(CoreMassVolatiles),
            boost::multiprecision::uint128_t(CoreMassEnergeticNuclide)
        });

        return (CoreMassVolatiles + CoreMassEnergeticNuclide + CoreMassZ) / kEarthMass;
    };

    auto PlanetType = Planet->GetPlanetType();
    switch (PlanetType) {
    case Astro::Planet::PlanetType::kIcePlanet:
        return CalculateIcePlanetMass();
    case Astro::Planet::PlanetType::kOceanic:
        return CalculateOceanicMass();
    case Astro::Planet::PlanetType::kIceGiant:
        return CalculateIceGiantMass();
    case Astro::Planet::PlanetType::kGasGiant:
        return CalculateGasGiantMass();
    case Astro::Planet::PlanetType::kRockyAsteroidCluster:
        return CalculateRockyAsteroidMass();
    case Astro::Planet::PlanetType::kRockyIceAsteroidCluster:
        return CalculateRockyIceAsteroidMass();
    default:
        return 0.0f;
    }
}

void OrbitalGenerator::CalculatePlanetRadius(float MassEarth, Astro::Planet* Planet) {
    float RadiusEarth = 0.0f;
    auto PlanetType = Planet->GetPlanetType();

    switch (PlanetType) {
    case Astro::Planet::PlanetType::kRocky:
    case Astro::Planet::PlanetType::kTerra:
    case Astro::Planet::PlanetType::kChthonian:
        if (MassEarth < 1.0f) {
            RadiusEarth = 1.94935f * std::pow(10.0f, (std::log10(MassEarth) / 3 - 0.0804f * std::pow(MassEarth, 0.394f) - 0.20949f));
        } else {
            RadiusEarth = std::pow(MassEarth, 1.0f / 3.7f);
        }
        break;
    case Astro::Planet::PlanetType::kIcePlanet:
    case Astro::Planet::PlanetType::kOceanic:
        if (MassEarth < 1.0f) {
            RadiusEarth = 2.53536f * std::pow(10.0f, (std::log10(MassEarth) / 3 - 0.0807f * std::pow(MassEarth, 0.375f) - 0.209396f));
        } else {
            RadiusEarth = 1.3f * std::pow(MassEarth, 1.0f / 3.905f);
        }
        break;
    case Astro::Planet::PlanetType::kIceGiant:
    case Astro::Planet::PlanetType::kSubIceGiant:
    case Astro::Planet::PlanetType::kGasGiant:
        if (MassEarth < 6.2f) {
            RadiusEarth = 1.41f * std::pow(MassEarth, 1.0f / 3.905f);
        } else if (MassEarth < 15.0f) {
            RadiusEarth = 0.6f * std::pow(MassEarth, 0.72f);
        } else {
            float CommonCoefficient = MassEarth / (kJupiterMass / kEarthMass);
            RadiusEarth = 11.0f * (0.96f + 0.21f * std::log10(CommonCoefficient) -
                0.2f * std::pow(std::log10(CommonCoefficient), 2.0f) + 0.1f * std::pow(CommonCoefficient, 0.215f));
        }
        break;
    default:
        break;
    }

    float Radius = RadiusEarth * kEarthRadius;
    Planet->SetRadius(Radius);
}

void OrbitalGenerator::GenerateSpin(float SemiMajorAxis, const Astro::StellarSystem::Orbit::OrbitalObject Parent, Astro::Planet* Planet) {
    auto  PlanetType = Planet->GetPlanetType();
    float PlanetMass = Planet->GetMassFloat();
    auto  ParentType = Parent.Type;
    auto  ParentBody = Parent.Object;

    float TimeToTidalLock = 0.0f;
    float ViscosityCoefficient = 1e12f;
    if (PlanetType == Astro::Planet::PlanetType::kIcePlanet || PlanetType == Astro::Planet::PlanetType::kOceanic) {
        ViscosityCoefficient = 4e9f;
    } else if (PlanetType == Astro::Planet::PlanetType::kRocky || PlanetType == Astro::Planet::PlanetType::kTerra || PlanetType == Astro::Planet::PlanetType::kChthonian) {
        ViscosityCoefficient = 3e10f;
    }

    float ParentAge    = 0.0f;
    float ParentMass   = 0.0f;
    float PlanetRadius = Planet->GetRadius();
    
    switch (ParentType) {
    case Astro::StellarSystem::Orbit::ObjectType::kStar:
        ParentAge  = static_cast<float>(ParentBody.StarPtr->GetAge());
        ParentMass = static_cast<float>(ParentBody.StarPtr->GetMass());
        break;
    case Astro::StellarSystem::Orbit::ObjectType::kPlanet:
        ParentAge  = static_cast<float>(ParentBody.PlanetPtr->GetAge());
        ParentMass = static_cast<float>(ParentBody.PlanetPtr->GetMass());
        break;
    }

    // 计算潮汐锁定时标
    double Term1 = 0.61435 * PlanetMass * std::pow(SemiMajorAxis, 6);
    double Term2 = 1 + (5.963361e11 * ViscosityCoefficient * std::pow(PlanetRadius, 4)) / std::pow(PlanetMass, 2);
    double Term3 = std::pow(ParentMass, 2) * std::pow(PlanetRadius, 3);
    TimeToTidalLock = static_cast<float>((Term1 * Term2) / Term3);
    float Spin = 0.0f;

    if (TimeToTidalLock < ParentAge) {
        Spin = -1.0f; // 使用 -1.0 来标记潮汐锁定
    }

    // 计算自转
    if (Spin != -1.0f) {
        float OrbitalPeriod = 2.0f * kPi * std::sqrt(std::pow(SemiMajorAxis, 3.0f) / (kGravityConstant * ParentMass));
        float InitialSpin = 0.0f;
        if (PlanetType == Astro::Planet::PlanetType::kGasGiant ||
            PlanetType == Astro::Planet::PlanetType::kHotGasGiant) {
            InitialSpin = 21600.0f + _CommonGenerator(_RandomEngine) * (43200.0f - 21600.0f);
        } else {
            InitialSpin = 28800.0f + _CommonGenerator(_RandomEngine) * (86400.0f - 28800.0f);
        }
        Spin = InitialSpin + (OrbitalPeriod - InitialSpin) * static_cast<float>(std::pow(ParentAge / TimeToTidalLock, 2.35));

        float Oblateness = 4.0f * std::pow(kPi, 2.0f) * std::pow(PlanetRadius, 3.0f);
        Oblateness /= (std::pow(Spin, 2.0f) * kGravityConstant * PlanetMass);
        Planet->SetOblateness(Oblateness);
    }

    Planet->SetSpin(Spin);
}

void OrbitalGenerator::CalculateTemperature(
    const Astro::StellarSystem::Orbit::ObjectType ParentType,
    float PoyntingVector,
    Astro::Planet* Planet
) {
    auto  PlanetType = Planet->GetPlanetType();
    float Albedo     = 0.0f;
    float Emissivity = 0.0f;
    float PlanetMass = Planet->GetMassFloat();
    float Spin       = Planet->GetSpin();

    if (PlanetType == Astro::Planet::PlanetType::kSubIceGiant    ||
        PlanetType == Astro::Planet::PlanetType::kIceGiant       ||
        PlanetType == Astro::Planet::PlanetType::kGasGiant       ||
        PlanetType == Astro::Planet::PlanetType::kHotSubIceGiant ||
        PlanetType == Astro::Planet::PlanetType::kHotIceGiant    ||
        PlanetType == Astro::Planet::PlanetType::kHotGasGiant) {
        if (PoyntingVector <= 170) {
            Albedo = 0.34f;
        } else if (PoyntingVector <= 200) {
            Albedo = 0.0156667f * PoyntingVector - 2.32333f;
        } else if (PoyntingVector <= 3470) {
            Albedo = 0.75f;
        } else if (PoyntingVector <= 3790) {
            Albedo = 7.58156f - 0.00196875f * PoyntingVector;
        } else if (PoyntingVector <= 103500) {
            Albedo = 0.12f;
        } else if (PoyntingVector <= 150000) {
            Albedo = 0.320323f - 1.93548e-6f * PoyntingVector;
        } else if (PoyntingVector <= 654000) {
            Albedo = 0.03f;
        } else if (PoyntingVector <= 1897000) {
            Albedo = 4.18343e-7f * PoyntingVector - 0.243596f;
        } else {
            Albedo = 0.55f;
        }

        Emissivity = 0.98f;
    } else if (!Util::Equal(Planet->GetAtmosphereMassFloat(), 0.0f)) {
        float AtmospherePressureAtm = (kGravityConstant * PlanetMass * Planet->GetAtmosphereMassFloat()) / (4 * kPi * std::pow(Planet->GetRadius(), 4.0f)) / kPascalToAtm;
        float Random = 0.9f + _CommonGenerator(_RandomEngine) * 0.2f;
        float TidalLockCoefficient = 0.0f;
        if (ParentType == Astro::StellarSystem::Orbit::ObjectType::kStar) {
            TidalLockCoefficient = Util::Equal(Spin, -1.0f) ? 2.0f : 1.0f;
        } else {
            TidalLockCoefficient = 1.0f;
        }

        if (PlanetType == Astro::Planet::PlanetType::kRocky || PlanetType == Astro::Planet::PlanetType::kChthonian) {
            Albedo = Random * std::min(0.7f, 0.12f + 0.2f * std::sqrt(TidalLockCoefficient * AtmospherePressureAtm));
            Emissivity = std::max(0.012f, 0.95f - 0.35f * std::pow(AtmospherePressureAtm, 0.25f));
        } else if (PlanetType == Astro::Planet::PlanetType::kOceanic || PlanetType == Astro::Planet::PlanetType::kTerra) {
            Albedo = Random * std::min(0.7f, 0.07f + 0.2f * std::sqrt(TidalLockCoefficient * AtmospherePressureAtm));
            Emissivity = std::max(0.1f, 0.98f - 0.35f * std::pow(AtmospherePressureAtm, 0.25f));
        } else if (PlanetType == Astro::Planet::PlanetType::kIcePlanet) {
            Albedo = Random * std::max(0.2f, 0.4f - 0.1f * std::sqrt(AtmospherePressureAtm));
            Emissivity = std::max(0.1f, 0.98f - 0.35f * std::pow(AtmospherePressureAtm, 0.25f));
        }
    } else if (Util::Equal(Planet->GetAtmosphereMassFloat(), 0.0f)) {
        if (PlanetType == Astro::Planet::PlanetType::kRocky || PlanetType == Astro::Planet::PlanetType::kChthonian) {
            Albedo = 0.12f * (0.9f + _CommonGenerator(_RandomEngine) * 0.2f);
            Emissivity = 0.95f;
        } else if (PlanetType == Astro::Planet::PlanetType::kIcePlanet) {
            Albedo = 0.4f + _CommonGenerator(_RandomEngine) * (0.98f - 0.4f);
            Emissivity = 0.98f;
        }
    }

    // 计算平衡温度
    float BalanceTemperature = std::pow((PoyntingVector * (1.0f - Albedo)) / (4.0f * kStefanBoltzmann * Emissivity), 0.25f);
    float CosmosMicrowaveBackground = (3.76119e10f) / _UniverseAge;
    if (BalanceTemperature < CosmosMicrowaveBackground) {
        BalanceTemperature = CosmosMicrowaveBackground;
    }

    Planet->SetBalanceTemperature(BalanceTemperature);
}

void OrbitalGenerator::GenerateMoons(
    std::size_t PlanetIndex,
    float FrostLineAu,
    const Astro::Star* Star,
    float PoyntingVector,
    const std::pair<float, float>& HabitableZoneAu,
    Astro::StellarSystem::Orbit::OrbitalObject& ParentPlanet,
    std::vector<std::unique_ptr<Astro::StellarSystem::Orbit>>& Orbits,
    std::vector<std::unique_ptr<Astro::Planet>>& Planets
) {
    auto* Planet            = ParentPlanet.Object.PlanetPtr;
    auto  PlanetType        = Planet->GetPlanetType();
    float PlanetMass        = Planet->GetMassFloat();
    float PlanetMassEarth   = PlanetMass / kEarthMass;
    float LiquidRocheRadius = 2.02373e7f * std::pow(PlanetMassEarth, 1.0f / 3.0f);
    float HillSphereRadius  = Orbits[PlanetIndex]->SemiMajorAxis * std::pow(3 * PlanetMass / static_cast<float>(Star->GetMass()), 1.0f / 3.0f);

    std::size_t MoonCount = 0;
    if (std::to_underlying(Star->GetEvolutionPhase()) < 1) {
        if (Planet->GetMassFloat() > 10 * kEarthMass && HillSphereRadius / 3 - 2 * LiquidRocheRadius > 1e9) {
            MoonCount = static_cast<std::size_t>(_CommonGenerator(_RandomEngine) * 3.0f);
        } else {
            if (Planet->GetMassFloat() > 100 * _AsteroidUpperLimit &&
                HillSphereRadius / 3 - 2 * LiquidRocheRadius > 3e8f) {
                Util::BernoulliDistribution MoonProbability(
                    std::min(0.5f, 0.1f * (HillSphereRadius / 3 - 2 * LiquidRocheRadius) / 3e8f));
                if (MoonProbability(_RandomEngine)) {
                    MoonCount = 1;
                }
            }
        }
    }

    std::vector<std::unique_ptr<Astro::StellarSystem::Orbit>> MoonOrbits;

    if (MoonCount == 0) {
        return;
    } else if (MoonCount == 1) {
        Astro::StellarSystem::Orbit MoonOrbit;
        MoonOrbit.Parent.PlanetPtr = Planet;
        MoonOrbit.ParentType = Astro::StellarSystem::Orbit::ObjectType::kPlanet;
        MoonOrbit.SemiMajorAxis = 2 * LiquidRocheRadius + _CommonGenerator(_RandomEngine) *
            (std::min(1e9f, HillSphereRadius / 3 - 1e8f) - 2 * LiquidRocheRadius);
        GenerateOrbitElements(MoonOrbit);

        glm::vec2 MoonNormal(Planet->GetNormal() + glm::vec2(
            -0.09f + _CommonGenerator(_RandomEngine) * 0.18f,
            -0.09f + _CommonGenerator(_RandomEngine) * 0.18f
        ));

        if (MoonNormal.x > 2 * kPi) {
            MoonNormal.x -= 2 * kPi;
        } else if (MoonNormal.x < 0.0f) {
            MoonNormal.x += 2 * kPi;
        }

        if (MoonNormal.y > kPi) {
            MoonNormal.y -= kPi;
        } else if (MoonNormal.y < 0.0f) {
            MoonNormal.y += kPi;
        }

        MoonOrbit.Normal = MoonNormal;

        MoonOrbits.emplace_back(std::make_unique<Astro::StellarSystem::Orbit>(MoonOrbit));
    } else if (MoonCount == 2) {
        Astro::StellarSystem::Orbit MoonOrbit1, MoonOrbit2;
        MoonOrbit1.SemiMajorAxis = 2 * LiquidRocheRadius + _CommonGenerator(_RandomEngine) *
            (7e8f - 2 * LiquidRocheRadius);
        GenerateOrbitElements(MoonOrbit1);

        float Probability = _CommonGenerator(_RandomEngine);
        if (Probability >= 0.0f && Probability < 0.1f) {
            MoonOrbit2.SemiMajorAxis = 1.587401f * MoonOrbit1.SemiMajorAxis;
        } else if (Probability >= 0.1f && Probability < 0.2f) {
            MoonOrbit2.SemiMajorAxis = 2.080084f * MoonOrbit1.SemiMajorAxis;
        } else {
            MoonOrbit2.SemiMajorAxis = MoonOrbit1.SemiMajorAxis + 2e8f + _CommonGenerator(_RandomEngine) *
                (std::min(2e9f, HillSphereRadius / 3 - 1e8f) - (MoonOrbit1.SemiMajorAxis + 2e8f));
        }

        GenerateOrbitElements(MoonOrbit2);

        std::array<glm::vec2, 2> MoonNormals{};

        for (int i = 0; i != 2; ++i) {
            MoonNormals[i] = glm::vec2(Planet->GetNormal() + glm::vec2(
                -0.09f + _CommonGenerator(_RandomEngine) * 0.18f,
                -0.09f + _CommonGenerator(_RandomEngine) * 0.18f
            ));

            if (MoonNormals[i].x > 2.0f * kPi) {
                MoonNormals[i].x -= 2.0f * kPi;
            } else if (MoonNormals[i].x < 0.0f) {
                MoonNormals[i].x += 2.0f * kPi;
            }

            if (MoonNormals[i].y > kPi) {
                MoonNormals[i].y -= kPi;
            } else if (MoonNormals[i].y < 0.0f) {
                MoonNormals[i].y += kPi;
            }
        }

        MoonOrbit1.Normal = MoonNormals[0];
        MoonOrbit2.Normal = MoonNormals[1];

        MoonOrbits.emplace_back(std::make_unique<Astro::StellarSystem::Orbit>(MoonOrbit1));
        MoonOrbits.emplace_back(std::make_unique<Astro::StellarSystem::Orbit>(MoonOrbit2));
    }

    for (std::size_t i = 0; i != MoonCount; ++i) {
        ParentPlanet.DirectOrbits.emplace_back(MoonOrbits[i].get());
    }

    float ParentCoreMass = Planet->GetCoreMassZFloat();
    float LogCoreMassLowerLimit = std::log10(std::max(_AsteroidUpperLimit, ParentCoreMass / 600));
    float LogCoreMassUpperLimit = std::log10(ParentCoreMass / 30.0f);

    std::vector<std::unique_ptr<Astro::Planet>> Moons;
    Moons.reserve(MoonCount);

    for (std::size_t i = 0; i != MoonCount; ++i) {
        Moons.emplace_back(std::make_unique<Astro::Planet>());

        float Exponent = LogCoreMassLowerLimit + _CommonGenerator(_RandomEngine) * (LogCoreMassUpperLimit - LogCoreMassLowerLimit);
        boost::multiprecision::uint128_t InitialCoreMass(std::pow(10.0f, Exponent));

        int VolatilesRate = 9000 + static_cast<int>(_CommonGenerator(_RandomEngine)) + 2000;
        int EnergeticNuclideRate = 4500000 + static_cast<int>(_CommonGenerator(_RandomEngine)) * 1000000;

        Astro::ComplexMass CoreMass;

        CoreMass.Volatiles = InitialCoreMass / VolatilesRate;
        CoreMass.EnergeticNuclide = InitialCoreMass / EnergeticNuclideRate;
        CoreMass.Z = InitialCoreMass - CoreMass.Volatiles - CoreMass.EnergeticNuclide;

        Moons[i]->SetCoreMass(CoreMass);

        if (MoonOrbits[i]->SemiMajorAxis > 5 * LiquidRocheRadius) {
            if (Orbits[PlanetIndex]->SemiMajorAxis / kAuToMeter > FrostLineAu) {
                Moons[i]->SetPlanetType(Astro::Planet::PlanetType::kIcePlanet);
                CalculatePlanetMass(Moons[i]->GetCoreMassFloat(), 0, 0, {}, Star, Moons[i].get()); // 0, 0, {}: 这些参数只在计算气态行星时有用，此处省略
            } else {
                Moons[i]->SetPlanetType(Astro::Planet::PlanetType::kRocky);
            }
        } else {
            Moons[i]->SetPlanetType(Astro::Planet::PlanetType::kRocky);
        }

        CalculatePlanetRadius(Moons[i]->GetCoreMassFloat() / kEarthMass, Moons[i].get());

        Astro::StellarSystem::StellarSystem::Orbit::OrbitalObject Parent(Planet, Astro::StellarSystem::Orbit::ObjectType::kPlanet);
        GenerateSpin(MoonOrbits[i]->SemiMajorAxis, Parent, Moons[i].get());

        CalculateTemperature(Astro::StellarSystem::Orbit::ObjectType::kPlanet, PoyntingVector, Moons[i].get());

        if (Star->GetStellarClass().GetStarType() == Util::StellarClass::StarType::kNormalStar) {
            GenerateTerra(Star, PoyntingVector, HabitableZoneAu, Orbits[i].get(), Moons[i].get());
        }

        if (Moons[i]->GetPlanetType() == Astro::Planet::PlanetType::kTerra) {
            GenerateCivilization(Star, PoyntingVector, HabitableZoneAu, Orbits[i].get(), Moons[i].get());
        }
    }

    #ifdef DEBUG_OUTPUT
    std::println("");

    for (std::size_t i = 0; i != MoonCount; ++i) {
        auto& Moon                           = Moons[i];
        auto  MoonType                       = Moon->GetPlanetType();
        float MoonMass                       = Moon->GetMassFloat();
        float MoonMassEarth                  = MoonMass / kEarthMass;
        float MoonMassMoon                   = MoonMass / kMoonMass;
        float MoonRadius                     = Moons[i]->GetRadius();
        float MoonRadiusEarth                = MoonRadius / kEarthRadius;
        float MoonRadiusMoon                 = MoonRadius / kMoonRadius;
        float AtmosphereMassZ                = Moon->GetAtmosphereMassZFloat();
        float AtmosphereMassVolatiles        = Moon->GetAtmosphereMassVolatilesFloat();
        float AtmosphereMassEnergeticNuclide = Moon->GetAtmosphereMassEnergeticNuclideFloat();
        float CoreMassZ                      = Moon->GetCoreMassZFloat();
        float CoreMassVolatiles              = Moon->GetCoreMassVolatilesFloat();
        float CoreMassEnergeticNuclide       = Moon->GetCoreMassEnergeticNuclideFloat();
        float OceanMassZ                     = Moon->GetOceanMassZFloat();;
        float OceanMassVolatiles             = Moon->GetOceanMassVolatilesFloat();
        float OceanMassEnergeticNuclide      = Moon->GetOceanMassEnergeticNuclideFloat();
        float CrustMineralMass               = Moon->GetCrustMineralMassFloat();
        float AtmospherePressure             = (kGravityConstant * MoonMass * (AtmosphereMassZ + AtmosphereMassVolatiles + AtmosphereMassEnergeticNuclide)) / (4 * kPi * std::pow(Moons[i]->GetRadius(), 4.0f));
        float Oblateness                     = Moon->GetOblateness();
        float Spin                           = Moon->GetSpin();
        float BalanceTemperature             = Moon->GetBalanceTemperature();
        std::println("Moon generated, details:");
        std::println("parent planet: {}", PlanetIndex + 1);
        std::println("semi-major axis: {} km, mass: {} earth ({} moon), radius: {} earth ({} moon), type: {}",
                     MoonOrbits[i]->SemiMajorAxis / 1000, MoonMassEarth, MoonMassMoon, MoonRadiusEarth, MoonRadiusMoon, std::to_underlying(MoonType));
        std::println("rotation period: {} h, oblateness: {}, balance temperature: {} K",
                     Spin / 3600, Oblateness, BalanceTemperature);
        std::println("atmo  mass z: {:.2E} kg, atmo  mass vol: {:.2E} kg, atmo  mass nuc: {:.2E} kg",
                     AtmosphereMassZ, AtmosphereMassVolatiles, AtmosphereMassEnergeticNuclide);
        std::println("core  mass z: {:.2E} kg, core  mass vol: {:.2E} kg, core  mass nuc: {:.2E} kg",
                     CoreMassZ, CoreMassVolatiles, CoreMassEnergeticNuclide);
        std::println("ocean mass z: {:.2E} kg, ocean mass vol: {:.2E} kg, ocean mass nuc: {:.2E} kg",
                     OceanMassZ, OceanMassVolatiles, OceanMassEnergeticNuclide);
        std::println("crust mineral mass : {:.2E} kg, atmo pressure : {:.2f} atm",
                     CrustMineralMass, AtmospherePressure / kPascalToAtm);
        std::println("");
    }
#endif // DEBUG_OUTPUT

    for (std::size_t i = 0; i != MoonCount; ++i) {
        Astro::StellarSystem::Orbit::OrbitalObject Moon;
        Moon.Object.PlanetPtr = Moons[i].get();
        Moon.Type = Astro::StellarSystem::Orbit::ObjectType::kPlanet;
        Moon.InitialTrueAnomaly = 0.0f;
        MoonOrbits[i]->Objects.emplace_back(Moon);
    }

    Orbits.reserve(Orbits.size() + MoonOrbits.size());
    Orbits.insert(Orbits.end(),
                  std::make_move_iterator(MoonOrbits.begin()),
                  std::make_move_iterator(MoonOrbits.end()));

    Planets.reserve(Planets.size() + Moons.size());
    Planets.insert(Planets.end(),
                   std::make_move_iterator(Moons.begin()),
                   std::make_move_iterator(Moons.end()));
}

void OrbitalGenerator::GenerateRings(
    std::size_t PlanetIndex,
    float FrostLineAu,
    const Astro::Star* Star,
    Astro::StellarSystem::Orbit::OrbitalObject& ParentPlanet,
    std::vector<std::unique_ptr<Astro::StellarSystem::Orbit>>& Orbits,
    std::vector<std::unique_ptr<Astro::AsteroidCluster>>& AsteroidClusters
) {
    auto* Planet            = ParentPlanet.Object.PlanetPtr;
    auto  PlanetType        = Planet->GetPlanetType();
    float PlanetMass        = Planet->GetMassFloat();
    float PlanetMassEarth   = PlanetMass / kEarthMass;
    float LiquidRocheRadius = 2.02373e7f * std::pow(PlanetMassEarth, 1.0f / 3.0f);
    float HillSphereRadius  = Orbits[PlanetIndex]->SemiMajorAxis * std::pow(3.0f * PlanetMass / static_cast<float>(Star->GetMass()), 1.0f / 3.0f);

    Util::Distribution<double>* RingsProbability = nullptr;
    if (LiquidRocheRadius < HillSphereRadius / 3.0f && LiquidRocheRadius > Planet->GetRadius()) {
        if (PlanetType == Astro::Planet::PlanetType::kGasGiant ||
            PlanetType == Astro::Planet::PlanetType::kIceGiant) {
            RingsProbability = &_RingsProbabilities[0];
        } else {
            RingsProbability = &_RingsProbabilities[1];
        }
    }

    if (RingsProbability != nullptr) {
        if ((*RingsProbability)(_RandomEngine)) {
            Astro::AsteroidCluster::AsteroidType AsteroidType{};
            float Exponent = -4.0f + _CommonGenerator(_RandomEngine) * 4.0f;
            float Random = std::pow(10.0f, Exponent);
            float RingsMass = Random * 1e20f * std::pow(LiquidRocheRadius / 1e8f, 2.0f);
            float RingsMassVolatiles = 0.0f;
            float RingsMassEnergeticNuclide = 0.0f;
            float RingsMassZ = 0.0f;
            if (Orbits[PlanetIndex]->SemiMajorAxis / kAuToMeter >= FrostLineAu && std::to_underlying(Star->GetEvolutionPhase()) < 1) {
                RingsMassEnergeticNuclide = RingsMass * 5e-6f * 0.064f;
                RingsMassVolatiles = RingsMass * 0.064f;
                RingsMassZ = RingsMass - RingsMassVolatiles - RingsMassEnergeticNuclide;
                AsteroidType = Astro::AsteroidCluster::AsteroidType::kRockyIce;
            } else {
                RingsMassEnergeticNuclide = RingsMass * 5e-6f;
                RingsMassZ = RingsMass - RingsMassEnergeticNuclide;
                AsteroidType = Astro::AsteroidCluster::AsteroidType::kRocky;
            }

            Astro::StellarSystem::Orbit RingsOrbit;
            float Inaccuracy = -0.1f + _CommonGenerator(_RandomEngine) * 0.2f;
            RingsOrbit.SemiMajorAxis = 0.6f * LiquidRocheRadius * (1.0f + Inaccuracy);
            GenerateOrbitElements(RingsOrbit);

            Astro::AsteroidCluster* RingsPtr = AsteroidClusters.emplace_back(std::make_unique<Astro::AsteroidCluster>()).get();
            RingsPtr->SetMassEnergeticNuclide(RingsMassEnergeticNuclide);
            RingsPtr->SetMassVolatiles(RingsMassVolatiles);
            RingsPtr->SetMassZ(RingsMassZ);

            RingsOrbit.Parent.PlanetPtr = Planet;
            RingsOrbit.ParentType = Astro::StellarSystem::Orbit::ObjectType::kPlanet;
            Astro::StellarSystem::Orbit::OrbitalObject Rings;
            Rings.Object.AsteroidClusterPtr = RingsPtr;
            Rings.Type = Astro::StellarSystem::Orbit::ObjectType::kAsteroidCluster;
            Rings.InitialTrueAnomaly = 0.0f;

            RingsOrbit.Objects.emplace_back(Rings);
            Orbits.emplace_back(std::make_unique<Astro::StellarSystem::Orbit>(RingsOrbit));
            ParentPlanet.DirectOrbits.emplace_back(Orbits.back().get());

#ifdef DEBUG_OUTPUT
            std::println("");
            std::println("Rings generated, details:");
            std::println("parent planet: {}", PlanetIndex + 1);
            std::println("semi-major axis: {} km, mass: {} kg, type: {}",
                         RingsOrbit.SemiMajorAxis / 1000, RingsMass, std::to_underlying(RingsPtr->GetAsteroidType()));
            std::println("mass z: {:.2E} kg, mass vol: {:.2E} kg, mass nuc: {:.2E} kg",
                         RingsMassZ, RingsMassVolatiles, RingsMassEnergeticNuclide);
            std::println("");
#endif // DEBUG_OUTPUT
        }
    }
}

void OrbitalGenerator::GenerateTerra(
    const Astro::Star* Star,
    float PoyntingVector,
    const std::pair<float, float>& HabitableZoneAu,
    const Astro::StellarSystem::Orbit* Orbit,
    Astro::Planet* Planet
) {
    auto  PlanetType = Planet->GetPlanetType();
    float PlanetMass = Planet->GetMassFloat();
    float PlanetMassEarth = Planet->GetMassFloat() / kEarthMass;
    float CoreMass = Planet->GetCoreMassFloat();
    float Term1 = 1.6567e15f * static_cast<float>(std::pow(Star->GetLuminosity() / (4.0 * kPi * kStefanBoltzmann * std::pow(Orbit->SemiMajorAxis, 2.0f)), 0.25));
    float Term2 = PlanetMass / Planet->GetRadius();
    float MaxTerm = std::max(1.0f, Term1 / Term2);
    float EscapeCoefficient = std::pow(10.0f, 1.0f - MaxTerm);
    if (PlanetType == Astro::Planet::PlanetType::kRocky &&
        Orbit->SemiMajorAxis / kAuToMeter > HabitableZoneAu.first  &&
        Orbit->SemiMajorAxis / kAuToMeter < HabitableZoneAu.second &&
        EscapeCoefficient > 0.1f && std::to_underlying(Star->GetEvolutionPhase()) < 1) { // 判断类地行星
        Planet->SetPlanetType(Astro::Planet::PlanetType::kTerra);
        // 计算新的海洋质量
        float Exponent = -0.5f + _CommonGenerator(_RandomEngine) * 1.5f;
        float Random = std::pow(10.0f, Exponent);
        float NewOceanMass = CoreMass * Random * 1e-4f;
        float NewOceanMassVolatiles = NewOceanMass / 9.0f;
        float NewOceanMassEnergeticNuclide = NewOceanMass * 5e-5f / 9.0f;
        float NewOceanMassZ = NewOceanMass - NewOceanMassVolatiles - NewOceanMassEnergeticNuclide;
        Planet->SetOceanMass({
            boost::multiprecision::uint128_t(NewOceanMassZ),
            boost::multiprecision::uint128_t(NewOceanMassVolatiles),
            boost::multiprecision::uint128_t(NewOceanMassEnergeticNuclide)
        });
    }

    PlanetType = Planet->GetPlanetType();

    // 计算地壳矿脉
    float Random = 0.0f;
    if (PlanetType == Astro::Planet::PlanetType::kRocky) {
        Random = 0.1f + _CommonGenerator(_RandomEngine) * 0.9f;
    } else if (PlanetType == Astro::Planet::PlanetType::kTerra) {
        Random = 1.0f + _CommonGenerator(_RandomEngine) * 9.0f;
    }
    float CrustMineralMass = Random * 1e-9f * std::pow(PlanetMass / kEarthMass, 2.0f) * kEarthMass;
    Planet->SetCrustMineralMass(CrustMineralMass);

    // 计算次生大气
    if (std::to_underlying(Star->GetEvolutionPhase()) < 1) {
        if (PlanetType == Astro::Planet::PlanetType::kRocky ||
            PlanetType == Astro::Planet::PlanetType::kTerra ||
            PlanetType == Astro::Planet::PlanetType::kOceanic ||
            PlanetType == Astro::Planet::PlanetType::kIcePlanet) {
            float Exponent = _CommonGenerator(_RandomEngine);
            Random = std::pow(10.0f, Exponent);
            float NewAtmosphereMass = EscapeCoefficient * PlanetMass * Random * 1e-5f;
            if (PlanetType == Astro::Planet::PlanetType::kTerra) {
                NewAtmosphereMass *= 0.035f;
            } else if (PlanetType == Astro::Planet::PlanetType::kIcePlanet) {
                if (PoyntingVector > 8) { // 保证氮气不会液化
                    NewAtmosphereMass = std::pow(EscapeCoefficient, 2.0f) * PlanetMass * Random * 1e-5f;
                } else {
                    NewAtmosphereMass = 0.0f;
                }
            }

            float NewAtmosphereMassVolatiles = 0.0f;
            float NewAtmosphereMassEnergeticNuclide = 0.0f;
            float NewAtmosphereMassZ = 0.0f;
            if (NewAtmosphereMass > 1e16f) {
                NewAtmosphereMassVolatiles = NewAtmosphereMass * 1e-2f;
                NewAtmosphereMassEnergeticNuclide = 0.0f;
                NewAtmosphereMassZ = NewAtmosphereMass - NewAtmosphereMassVolatiles - NewAtmosphereMassEnergeticNuclide;
                Planet->SetAtmosphereMassZ(NewAtmosphereMassZ);
                Planet->SetAtmosphereMassVolatiles(NewAtmosphereMassVolatiles);
                Planet->SetAtmosphereMassEnergeticNuclide(NewAtmosphereMassEnergeticNuclide);
            } else { // 只调整核心质量（核心就是整个星球）
                float CoreMassVolatiles = Planet->GetCoreMassVolatilesFloat();
                float CoreMassEnergeticNuclide = Planet->GetCoreMassEnergeticNuclideFloat();
                CoreMassVolatiles += 33.1f * std::pow(Planet->GetRadius(), 2.0f);
                CoreMassEnergeticNuclide += 3.31e-4f * std::pow(Planet->GetRadius(), 2.0f);
                Planet->SetCoreMassVolatiles(CoreMassVolatiles);
                Planet->SetCoreMassEnergeticNuclide(CoreMassEnergeticNuclide);
            }
        }
    }
}

void OrbitalGenerator::GenerateTrojan(
    const Astro::Star* Star,
    float FrostLineAu,
    Astro::StellarSystem::Orbit* Orbit,
    Astro::StellarSystem::Orbit::OrbitalObject& ParentPlanet,
    std::vector<std::unique_ptr<Astro::AsteroidCluster>>& AsteroidClusters
) {
    auto* Planet            = ParentPlanet.Object.PlanetPtr;
    auto  PlanetType        = Planet->GetPlanetType();
    float PlanetMass        = Planet->GetMassFloat();
    float PlanetMassEarth   = PlanetMass / kEarthMass;
    float LiquidRocheRadius = 2.02373e7f * std::pow(PlanetMassEarth, 1.0f / 3.0f);
    float HillSphereRadius  = Orbit->SemiMajorAxis * std::pow(3.0f * PlanetMass / static_cast<float>(Star->GetMass()), 1.0f / 3.0f);

    float Random     = 1.0f + _CommonGenerator(_RandomEngine);
    float Term1      = 1e-9f * PlanetMassEarth * (HillSphereRadius / 3.11e9f);
    float Term2      = PlanetMassEarth * 1e-3f / 2.0f;
    float TrojanMass = Random * std::max(Term1, Term1) * kEarthMass;

    if (TrojanMass < 1e14f) {
        return;
    }

    bool bGenerated = false;
    auto TrojanBelt = std::make_unique<Astro::AsteroidCluster>();

    for (auto* NextOrbit : ParentPlanet.DirectOrbits) {
        if (NextOrbit->Objects.front().Type == Astro::StellarSystem::Orbit::ObjectType::kAsteroidCluster) {
            auto* Rings = NextOrbit->Objects.front().Object.AsteroidClusterPtr;
            float TotalMass = Rings->GetMassFloat();
            float RingsVolatiles = Rings->GetMassVolatilesFloat();
            float RingsEnergeticNuclide = Rings->GetMassEnergeticNuclideFloat();
            float RingsZ = Rings->GetMassZFloat();

            TrojanBelt->SetMassVolatiles(RingsVolatiles / TotalMass * TrojanMass);
            TrojanBelt->SetMassEnergeticNuclide(RingsEnergeticNuclide / TotalMass * TrojanMass);
            TrojanBelt->SetMassZ(RingsZ / TotalMass * TrojanMass);
            TrojanBelt->SetAsteroidType(NextOrbit->Objects.front().Object.AsteroidClusterPtr->GetAsteroidType());
            bGenerated = true;
        }
    }

    if (!bGenerated) {
        float TrojanMassEnergeticNuclide = 0.0f;
        float TrojanMassVolatiles = 0.0f;
        float TrojanMassZ = 0.0f;
        Astro::AsteroidCluster::AsteroidType AsteroidType;

        if (Orbit->SemiMajorAxis / kAuToMeter >= FrostLineAu && std::to_underlying(Star->GetEvolutionPhase()) < 1) {
            TrojanMassEnergeticNuclide = TrojanMass * 5e-6f * 0.064f;
            TrojanMassVolatiles = TrojanMass * 0.064f;
            TrojanMassZ = TrojanMass - TrojanMassVolatiles - TrojanMassEnergeticNuclide;
            AsteroidType = Astro::AsteroidCluster::AsteroidType::kRockyIce;
        } else {
            TrojanMassEnergeticNuclide = TrojanMass * 5e-6f;
            TrojanMassZ = TrojanMass - TrojanMassEnergeticNuclide;
            AsteroidType = Astro::AsteroidCluster::AsteroidType::kRocky;
        }

        TrojanBelt->SetAsteroidType(AsteroidType);
        TrojanBelt->SetMassEnergeticNuclide(TrojanMassEnergeticNuclide);
        TrojanBelt->SetMassVolatiles(TrojanMassVolatiles);
        TrojanBelt->SetMassZ(TrojanMassZ);
    }

#ifdef DEBUG_OUTPUT
    std::println("");
    std::println("Trojan belt details:");
    std::println("semi-major axis: {} AU, mass: {} moon, type: {}",
                 Orbit->SemiMajorAxis / kAuToMeter, TrojanMass / kMoonMass, std::to_underlying(TrojanBelt->GetAsteroidType()));
    std::println("mass z: {:.2E} kg, mass vol: {:.2E} kg, mass nuc: {:.2E} kg",
                 TrojanBelt->GetMassZFloat(), TrojanBelt->GetMassVolatilesFloat(), TrojanBelt->GetMassEnergeticNuclideFloat());
    std::println("");
#endif // DEBUG_OUTPUT

    Astro::StellarSystem::Orbit::OrbitalObject MyBelt(TrojanBelt.get(), Astro::StellarSystem::Orbit::ObjectType::kAsteroidCluster);
    Orbit->Objects.emplace_back(MyBelt);

    AsteroidClusters.emplace_back(std::move(TrojanBelt));
}

void OrbitalGenerator::GenerateCivilization(
    const Astro::Star* Star,
    float PoyntingVector,
    const std::pair<float, float>& HabitableZoneAu,
    const Astro::StellarSystem::Orbit* Orbit,
    Astro::Planet* Planet
) {
    bool bHasLife = false;
    if (Star->GetAge() > 5e8) {
        if (Orbit->SemiMajorAxis / kAuToMeter > HabitableZoneAu.first &&
            Orbit->SemiMajorAxis / kAuToMeter < HabitableZoneAu.second) {
            if (_bContainUltravioletHabitableZone) {
                double StarMassSol = Star->GetMass() / kSolarMass;
                if (StarMassSol > 0.75 && StarMassSol < 1.5) {
                    bHasLife = true;
                }
            } else {
                bHasLife = true;
            }
        }
    }

    if (bHasLife) {
        _CivilizationGenerator->GenerateCivilization(Star->GetAge(), PoyntingVector, Planet);
    }
}

// Tool function implementations
// -----------------------------
float CalculatePrevMainSequenceLuminosity(float StarInitialMassSol) {
    float CommonCoefficient = (std::pow(10.0f, 2.0f - StarInitialMassSol) + 1.0f) * static_cast<float>(kSolarLuminosity);
    float Luminosity = 0.0f;

    if (StarInitialMassSol >= 0.075f && StarInitialMassSol < 0.43f) {
        Luminosity = CommonCoefficient * (0.23f * std::pow(StarInitialMassSol, 2.3f));
    } else if (StarInitialMassSol >= 0.43f && StarInitialMassSol < 2.0f) {
        Luminosity = CommonCoefficient * std::pow(StarInitialMassSol, 4.0f);
    } else if (StarInitialMassSol >= 2.0f && StarInitialMassSol <= 12.0f) {
        Luminosity = CommonCoefficient * (1.5f * std::pow(StarInitialMassSol, 3.5f));
    }

    return Luminosity;
}

std::unique_ptr<Astro::AsteroidCluster> PlanetToAsteroidCluster(const Astro::Planet* Planet) {
    Astro::AsteroidCluster AsteroidCluster;

    if (Planet->GetPlanetType() == Astro::Planet::PlanetType::kRockyAsteroidCluster) {
        AsteroidCluster.SetAsteroidType(Astro::AsteroidCluster::AsteroidType::kRocky);
    } else {
        AsteroidCluster.SetAsteroidType(Astro::AsteroidCluster::AsteroidType::kRockyIce);
    }

    AsteroidCluster.SetMass(Planet->GetCoreMassStruct());
    AsteroidCluster.SetMassZ(Planet->GetCoreMassZ());
    AsteroidCluster.SetMassVolatiles(Planet->GetCoreMassVolatiles());
    AsteroidCluster.SetMassEnergeticNuclide(Planet->GetCoreMassEnergeticNuclide());

    return std::make_unique<Astro::AsteroidCluster>(AsteroidCluster);
}

_MODULE_END
_NPGS_END
