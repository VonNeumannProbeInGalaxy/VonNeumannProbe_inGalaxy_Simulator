#include "OrbitalGenerator.h"

#include <cmath>
#include <algorithm>
#include <array>
#include <iterator>
#include <print>
#include <utility>
#include <vector>

#include "Engine/Core/Constants.h"

#define DEBUG_OUTPUT

_NPGS_BEGIN
_MODULES_BEGIN

OrbitalGenerator::OrbitalGenerator(const std::seed_seq& SeedSeq, float AsteroidUpperLimit, float LifeProbability, bool bContainUltravioletChz, bool bEnableAsiFilter)
    :
    _RandomEngine(SeedSeq), _CommonGenerator(0.0f, 1.0f),
    _MigrationProbability(0.1), _WalkInProbability(0.8), _ScatteringProbability(0.15),
    _AsteroidBeltProbability(0.4), _LifeOccurrenceProbability(LifeProbability),
    _AsiFiltedProbability(static_cast<double>(bEnableAsiFilter) * 0.2),
    _AsteroidUpperLimit(AsteroidUpperLimit), _bContainUltravioletChz(bContainUltravioletChz)
{}

void OrbitalGenerator::GenerateOrbitals(StellarSystem& System) {}

void OrbitalGenerator::GeneratePlanets(StellarSystem& System) {
    // 目前只处理单星情况
    // 变量名未标注单位均为国际单位制
    PlanetaryDisk PlanetaryDiskTempData{};
    auto& Star = System.StarData().front();

    if (Star.GetFeH() < -2.0f) {
        return;
    }

    // 生成原行星盘数据
    float DiskBase = 1.0f + _CommonGenerator.Generate(_RandomEngine); // 基准随机数，1-2 之间
    float StarInitialMassSol = Star.GetInitialMass() / static_cast<float>(kSolarMass);
    auto StarType = Star.GetStellarClass().GetStarType();
    if (StarType != Modules::StellarClass::StarType::kNeutronStar && StarType != Modules::StellarClass::StarType::kBlackHole) {
        float DiskMassSol = DiskBase * StarInitialMassSol * std::pow(10.0f, -2.05f + 0.1214f * StarInitialMassSol - 0.02669f * std::pow(StarInitialMassSol, 2.0f) - 0.2274f * std::log(StarInitialMassSol));
        float DustMassSol = DiskMassSol * 0.0142f * 0.4f * std::pow(10.0f, Star.GetFeH());
        float OuterRadiusAu = StarInitialMassSol >= 1.0f ? 45.0f * StarInitialMassSol : 45.0f * std::pow(StarInitialMassSol, 2.0f);
        float InterRadiusAu = 0.0f;
        float Factor = StarInitialMassSol < 0.6f && StarInitialMassSol > 1.5f ? 1700.0f : 1400.0f;
        float CommonFactor = (std::pow(10.0f, 2.0f - StarInitialMassSol) + 1.0f) * (static_cast<float>(kSolarLuminosity) / (4.0f * kPi * kStefanBoltzmann * std::pow(Factor, 4.0f)));
        if (StarInitialMassSol >= 0.075f && StarInitialMassSol < 0.43f) {
            InterRadiusAu = CommonFactor * (0.23f * std::pow(StarInitialMassSol, 2.3f));
        } else if (StarInitialMassSol >= 0.43f && StarInitialMassSol < 2.0f) {
            InterRadiusAu = CommonFactor * std::pow(StarInitialMassSol, 4.0f);
        } else if (StarInitialMassSol >= 2.0f && StarInitialMassSol <= 12.0f) {
            InterRadiusAu = CommonFactor * (1.5f * std::pow(StarInitialMassSol, 3.5f));
        }
        InterRadiusAu = std::sqrt(InterRadiusAu) / kAuToMeter; // 转化为 AU

        PlanetaryDiskTempData.InterRadiusAu = InterRadiusAu;
        PlanetaryDiskTempData.OuterRadiusAu = OuterRadiusAu;
        PlanetaryDiskTempData.DiskMassSol = DiskMassSol;
        PlanetaryDiskTempData.DustMassSol = DustMassSol;
    } else if (Star.GetEvolutionEnding() == AstroObject::Star::Death::kWhiteDwarfMerge) {
        DiskBase = std::pow(10.0f, -1.0f) + _CommonGenerator.Generate(_RandomEngine) * (1.0f - std::pow(10.0f, -1.0f));
        float StarMassSol = static_cast<float>(Star.GetMass() / kSolarMass);
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

    // 生成行星个数
    std::size_t PlanetCount = 0;
    if (StarType != Modules::StellarClass::StarType::kNeutronStar && StarType != Modules::StellarClass::StarType::kBlackHole) {
        if (StarInitialMassSol < 0.6f) {
            PlanetCount = static_cast<std::size_t>(4.0f + _CommonGenerator.Generate(_RandomEngine) * 4.0f);
        } else if (StarInitialMassSol < 0.9f) {
            PlanetCount = static_cast<std::size_t>(5.0f + _CommonGenerator.Generate(_RandomEngine) * 5.0f);
        } else if (StarInitialMassSol < 3.0f) {
            PlanetCount = static_cast<std::size_t>(6.0f + _CommonGenerator.Generate(_RandomEngine) * 6.0f);
        } else {
            PlanetCount = static_cast<std::size_t>(4.0f + _CommonGenerator.Generate(_RandomEngine) * 4.0f);
        }
    } else if (Star.GetEvolutionEnding() == AstroObject::Star::Death::kWhiteDwarfMerge) {
        PlanetCount = static_cast<std::size_t>(2.0f + _CommonGenerator.Generate(_RandomEngine) * 2.0f);
    }

    std::vector<AstroObject::Planet> Planets(PlanetCount);

    // 生成行星初始核心质量
    std::vector<float> CoreBase(PlanetCount, 0);
    for (float& Num : CoreBase) {
        Num = static_cast<float>(_CommonGenerator.Generate(_RandomEngine) * 3.0f);
    }

    float CoreBaseSum = std::accumulate(CoreBase.begin(), CoreBase.end(), 0.0f, [](float Sum, float Num) -> float {
        return Sum + std::pow(10.0f, Num);
    });

    AstroObject::Planet::ComplexMass CoreMass;
    boost::multiprecision::int128_t InitialCoreMassSol;
    std::vector<float> CoreMassesSol(PlanetCount); // 初始核心质量，单位太阳
    for (std::size_t i = 0; i < PlanetCount; ++i) {
        CoreMassesSol[i] = PlanetaryDiskTempData.DustMassSol * std::pow(10.0f, CoreBase[i]) / CoreBaseSum;
        auto InitialCoreMass = boost::multiprecision::int128_t(kSolarMass * CoreMassesSol[i]);

        int VolatilesRate = 9000 + static_cast<int>(_CommonGenerator.Generate(_RandomEngine)) + 2000;
        int EnergeticNuclideRate = 4500000 + static_cast<int>(_CommonGenerator.Generate(_RandomEngine)) * 1000000;

        CoreMass.Volatiles = InitialCoreMass / VolatilesRate;
        CoreMass.EnergeticNuclide = InitialCoreMass / EnergeticNuclideRate;
        CoreMass.Z = InitialCoreMass - CoreMass.Volatiles - CoreMass.EnergeticNuclide;

        Planets[i].SetCoreMass(CoreMass);
#ifdef DEBUG_OUTPUT
        std::println("Generate initial core mass: planet {} initial core mass: {} earth", i + 1, CoreMassesSol[i] * kSolarMassToEarth);
#endif // DEBUG_OUTPUT
    }

#ifdef DEBUG_OUTPUT
    std::println("");
#endif // DEBUG_OUTPUT

    // 初始化轨道
    std::vector<std::pair<StellarSystem::OrbitalElements, std::vector<AstroObject::AstroObject*>>> Orbits(PlanetCount);
    for (auto& Orbit : Orbits) {
        Orbit.first.ParentBody = &System.StarData().front(); // 行星轨道上级天体，绑定为主恒星
    }

    // 生成初始轨道半长轴
    std::vector<float> DiskBoundariesAu(PlanetCount + 1);
    float CoreMassSum = std::accumulate(CoreMassesSol.begin(), CoreMassesSol.end(), 0.0f, [](float Sum, float Num) -> float {
        return Sum + std::pow(Num, 0.1f);
    });
    DiskBoundariesAu[0] = PlanetaryDiskTempData.InterRadiusAu;
    for (std::size_t i = 0; i != PlanetCount; ++i) {
        float PartCoreMassSum = std::accumulate(CoreMassesSol.begin(), CoreMassesSol.begin() + i + 1, 0.0f, [](float Sum, float Num) -> float {
            return Sum + std::pow(Num, 0.1f);
        });
        DiskBoundariesAu[i + 1] = PlanetaryDiskTempData.InterRadiusAu * std::pow(PlanetaryDiskTempData.OuterRadiusAu / PlanetaryDiskTempData.InterRadiusAu, PartCoreMassSum / CoreMassSum);
        float SemiMajorAxis = kAuToMeter * (DiskBoundariesAu[i] + DiskBoundariesAu[i + 1]) / 2.0f;
        Orbits[i].first.SemiMajorAxis = SemiMajorAxis;
        GenOrbitElements(Orbits[i].first); // 生成剩余的 5 个根数
#ifdef DEBUG_OUTPUT
        std::println("Generate initial semi-major axis: planet {} initial semi-major axis: {} AU", i + 1, Orbits[i].first.SemiMajorAxis / kAuToMeter);
#endif // DEBUG_OUTPUT
    }

#ifdef DEBUG_OUTPUT
    std::println("");
#endif // DEBUG_OUTPUT

    // 计算原行星盘年龄
    float DiskAge = 8.15e6f + 830000 * StarInitialMassSol - 33584 * std::pow(StarInitialMassSol, 2.0f) - 5.301e6f * std::log(StarInitialMassSol);
    if (static_cast<int>(Star.GetEvolutionPhase()) < 1 && DiskAge >= Star.GetAge()) {
        for (auto& Planet : Planets) {
            Planet.SetPlanetType(AstroObject::Planet::PlanetType::kRockyAsteroidCluster);
        }
    }

    std::vector<float> NewCoreMassesSol(PlanetCount); // 吸积核心质量，单位太阳
    float MigratedOriginSemiMajorAxisAu = 0.0f;       // 原有的半长轴，用于计算内迁行星

    // Lambda 封装
    auto TransformData = [&]() -> void {
        std::transform(std::make_move_iterator(Planets.begin()),
            std::make_move_iterator(Planets.end()),
            std::back_inserter(System.PlanetData()),
            [](AstroObject::Planet&& Planet) -> AstroObject::Planet {
                return std::move(Planet);
            }
        );

        std::transform(std::make_move_iterator(Orbits.begin()),
            std::make_move_iterator(Orbits.end()),
            std::back_inserter(System.OrbitData()),
            [](std::pair<StellarSystem::OrbitalElements, std::vector<AstroObject::AstroObject*>>&& Orbit) -> std::pair<StellarSystem::OrbitalElements, std::vector<AstroObject::AstroObject*>> {
                return std::move(Orbit);
            }
        );
    };

    auto ErasePlanets = [&](float Limit) -> void { // 抹掉位于临界线以内的行星
        for (std::size_t i = 0; i < PlanetCount; ++i) {
            if (Orbits[0].first.SemiMajorAxis < Limit) {
                Planets.erase(Planets.begin());
                Orbits.erase(Orbits.begin());
                NewCoreMassesSol.erase(NewCoreMassesSol.begin());
                CoreMassesSol.erase(CoreMassesSol.begin());
                --PlanetCount;
            }
        }
    };

    auto CalcIcePlanetMass = [&](std::size_t Index) -> float {
        float Random2 = 0.9f + _CommonGenerator.Generate(_RandomEngine) * 0.2f;
        float Random3 = 0.9f + _CommonGenerator.Generate(_RandomEngine) * 0.2f;

        float CoreMass = kSolarMass * CoreMassesSol[Index];

        float OceanMassVolatiles        = 0.0f;
        float OceanMassEnergeticNuclide = 0.0f;
        float OceanMassZ                = 0.0f;

        if (static_cast<int>(Star.GetEvolutionPhase()) > 1) {
            Planets[Index].SetPlanetType(AstroObject::Planet::PlanetType::kRocky);
        } else {
            OceanMassVolatiles = CoreMass * 0.15f;
            OceanMassEnergeticNuclide = 0.15f * 5e-5f * CoreMass;
            OceanMassZ = CoreMass * 1.35f - OceanMassVolatiles - OceanMassEnergeticNuclide;
        }

        float CoreMassVolatiles = CoreMass * 1e-4f * Random2;
        float CoreMassEnergeticNuclide = CoreMass * 5e-6f * Random3;
        float CoreMassZ = CoreMass - CoreMassVolatiles - CoreMassEnergeticNuclide;

        Planets[Index].SetOceanMass({ boost::multiprecision::int128_t(OceanMassZ), boost::multiprecision::int128_t(OceanMassVolatiles), boost::multiprecision::int128_t(OceanMassEnergeticNuclide) });
        Planets[Index].SetCoreMass({ boost::multiprecision::int128_t(CoreMassZ), boost::multiprecision::int128_t(CoreMassVolatiles), boost::multiprecision::int128_t(CoreMassEnergeticNuclide) });

        return (OceanMassVolatiles + OceanMassEnergeticNuclide + OceanMassZ + CoreMassVolatiles + CoreMassEnergeticNuclide + CoreMassZ) / kEarthMass;
    };

    auto CalcOceanicMass = [&](std::size_t Index) -> float {
        float Random1 = _CommonGenerator.Generate(_RandomEngine) * 1.35f;
        float Random2 = 0.9f + _CommonGenerator.Generate(_RandomEngine) * 0.2f;
        float Random3 = 0.9f + _CommonGenerator.Generate(_RandomEngine) * 0.2f;

        float CoreMass = kSolarMass * CoreMassesSol[Index];

        float OceanMassVolatiles        = 0.0f;
        float OceanMassEnergeticNuclide = 0.0f;
        float OceanMassZ                = 0.0f;

        if (static_cast<int>(Star.GetEvolutionPhase()) > 1) {
            Planets[Index].SetPlanetType(AstroObject::Planet::PlanetType::kRocky);
        } else {
            OceanMassVolatiles = (CoreMass * Random1) / 9.0f;
            OceanMassEnergeticNuclide = 5e-5f * OceanMassVolatiles;
            OceanMassZ = CoreMass * Random1 - OceanMassVolatiles - OceanMassEnergeticNuclide;
        }

        float CoreMassVolatiles = CoreMass * 1e-4f * Random2;
        float CoreMassEnergeticNuclide = CoreMass * 5e-6f * Random3;
        float CoreMassZ = CoreMass - CoreMassVolatiles - CoreMassEnergeticNuclide;

        Planets[Index].SetOceanMass({ boost::multiprecision::int128_t(OceanMassZ), boost::multiprecision::int128_t(OceanMassVolatiles), boost::multiprecision::int128_t(OceanMassEnergeticNuclide) });
        Planets[Index].SetCoreMass({ boost::multiprecision::int128_t(CoreMassZ), boost::multiprecision::int128_t(CoreMassVolatiles), boost::multiprecision::int128_t(CoreMassEnergeticNuclide) });

        return (OceanMassVolatiles + OceanMassEnergeticNuclide + OceanMassZ + CoreMassVolatiles + CoreMassEnergeticNuclide + CoreMassZ) / kEarthMass;
    };

    auto CalcIceGiantMass = [&](std::size_t Index) -> float {
        float Random1 = 2.0f + _CommonGenerator.Generate(_RandomEngine) * (std::log10(20.0f) - std::log10(2.0f));
        float Random2 = 0.9f + _CommonGenerator.Generate(_RandomEngine) * 0.2f;
        float Random3 = 0.9f + _CommonGenerator.Generate(_RandomEngine) * 0.2f;

        float CoreMass = kSolarMass * CoreMassesSol[Index];
        float NewCoreMass = kSolarMass * NewCoreMassesSol[Index];
        float SemiMajorAxisAu = Planets[Index].GetMigration() ? MigratedOriginSemiMajorAxisAu : Orbits[Index].first.SemiMajorAxis / kAuToMeter;

        float AtmosphereMassVolatiles = (NewCoreMass - CoreMass) / 9.0f + CoreMass * (0.5f + 0.5f * (SemiMajorAxisAu - PlanetaryDiskTempData.InterRadiusAu) / (PlanetaryDiskTempData.OuterRadiusAu - PlanetaryDiskTempData.InterRadiusAu)) * Random1 / 6.0f;
        float AtmosphereMassEnergeticNuclide = 5e-5f * AtmosphereMassVolatiles;
        float AtmosphereMassZ = CoreMass * (0.5f + 0.5f * (SemiMajorAxisAu - PlanetaryDiskTempData.InterRadiusAu) / (PlanetaryDiskTempData.OuterRadiusAu - PlanetaryDiskTempData.InterRadiusAu)) * Random1 + (NewCoreMass - CoreMass) - AtmosphereMassVolatiles - AtmosphereMassEnergeticNuclide;

        float CoreMassVolatiles = CoreMass * 1e-4f * Random2;
        float CoreMassEnergeticNuclide = CoreMass * 5e-6f * Random3;
        float CoreMassZ = CoreMass - CoreMassVolatiles - CoreMassEnergeticNuclide;

        Planets[Index].SetAtmosphereMass({ boost::multiprecision::int128_t(AtmosphereMassZ), boost::multiprecision::int128_t(AtmosphereMassVolatiles), boost::multiprecision::int128_t(AtmosphereMassEnergeticNuclide) });
        Planets[Index].SetCoreMass({ boost::multiprecision::int128_t(CoreMassZ), boost::multiprecision::int128_t(CoreMassVolatiles), boost::multiprecision::int128_t(CoreMassEnergeticNuclide) });
        Planets[Index].SetPlanetType(AstroObject::Planet::PlanetType::kIceGiant);

        return (AtmosphereMassVolatiles + AtmosphereMassEnergeticNuclide + AtmosphereMassZ + CoreMassVolatiles + CoreMassEnergeticNuclide + CoreMassZ) / kEarthMass;
    };

    auto CalcGasGiantMass = [&](std::size_t Index) -> float {
        float Random1 = 7.0f + _CommonGenerator.Generate(_RandomEngine) * (std::min(50.0f, 1.0f / 0.0142f * std::pow(10.0f, Star.GetFeH())) - 7.0f);
        float Random2 = 0.9f + _CommonGenerator.Generate(_RandomEngine) * 0.2f;
        float Random3 = 0.9f + _CommonGenerator.Generate(_RandomEngine) * 0.2f;

        float CoreMass = kSolarMass * CoreMassesSol[Index];
        float NewCoreMass = kSolarMass * NewCoreMassesSol[Index];
        float SemiMajorAxisAu = Planets[Index].GetMigration() ? MigratedOriginSemiMajorAxisAu : Orbits[Index].first.SemiMajorAxis / kAuToMeter;

        float AtmosphereMassZ = (0.0142f * std::pow(10.0f, Star.GetFeH())) * CoreMass * (0.5f + 0.5f * (SemiMajorAxisAu - PlanetaryDiskTempData.InterRadiusAu) / (PlanetaryDiskTempData.OuterRadiusAu - PlanetaryDiskTempData.InterRadiusAu)) * Random1 + (1.0f - (1.0f + 5e-5f) / 9.0f) * (NewCoreMass - CoreMass);
        float AtmosphereMassEnergeticNuclide = 5e-5f * (CoreMass * (0.5f + 0.5f * (SemiMajorAxisAu - PlanetaryDiskTempData.InterRadiusAu) / (PlanetaryDiskTempData.OuterRadiusAu - PlanetaryDiskTempData.InterRadiusAu)) * Random1 + (NewCoreMass - CoreMass) / 9.0f);
        float AtmosphereMassVolatiles = CoreMass * (0.5f + 0.5f * (SemiMajorAxisAu - PlanetaryDiskTempData.InterRadiusAu) / (PlanetaryDiskTempData.OuterRadiusAu - PlanetaryDiskTempData.InterRadiusAu)) * Random1 + (NewCoreMass - CoreMass) - AtmosphereMassZ - AtmosphereMassEnergeticNuclide;

        float CoreMassVolatiles = CoreMass * 1e-4f * Random2;
        float CoreMassEnergeticNuclide = CoreMass * 5e-6f * Random3;
        float CoreMassZ = CoreMass - CoreMassVolatiles - CoreMassEnergeticNuclide;

        Planets[Index].SetAtmosphereMass({ boost::multiprecision::int128_t(AtmosphereMassZ), boost::multiprecision::int128_t(AtmosphereMassVolatiles), boost::multiprecision::int128_t(AtmosphereMassEnergeticNuclide) });
        Planets[Index].SetCoreMass({ boost::multiprecision::int128_t(CoreMassZ), boost::multiprecision::int128_t(CoreMassVolatiles), boost::multiprecision::int128_t(CoreMassEnergeticNuclide) });
        Planets[Index].SetPlanetType(AstroObject::Planet::PlanetType::kGasGiant);

        return (AtmosphereMassVolatiles + AtmosphereMassEnergeticNuclide + AtmosphereMassZ + CoreMassVolatiles + CoreMassEnergeticNuclide + CoreMassZ) / kEarthMass;
    };

    auto CalcRockyAsteroidMass = [&](std::size_t Index) -> float {
        float Random2 = 0.9f + _CommonGenerator.Generate(_RandomEngine) * 0.2f;
        float Random3 = 0.9f + _CommonGenerator.Generate(_RandomEngine) * 0.2f;

        float CoreMass = kSolarMass * CoreMassesSol[Index];

        float CoreMassVolatiles = CoreMass * 1e-4f * Random2;
        float CoreMassEnergeticNuclide = CoreMass * 5e-6f * Random3;
        float CoreMassZ = CoreMass - CoreMassVolatiles - CoreMassEnergeticNuclide;

        Planets[Index].SetCoreMass({ boost::multiprecision::int128_t(CoreMassZ), boost::multiprecision::int128_t(CoreMassVolatiles), boost::multiprecision::int128_t(CoreMassEnergeticNuclide) });

        return (CoreMassVolatiles + CoreMassEnergeticNuclide + CoreMassZ) / kEarthMass;
    };

    auto CalcRockyIceAsteroidMass = [&](std::size_t Index) -> float {
        float Random2 = 0.9f + _CommonGenerator.Generate(_RandomEngine) * 0.2f;
        float Random3 = 0.9f + _CommonGenerator.Generate(_RandomEngine) * 0.2f;

        float CoreMass = kSolarMass * CoreMassesSol[Index];

        float OceanMassVolatiles = CoreMass * 0.15f;
        float OceanMassEnergeticNuclide = 0.15f * 5e-5f * CoreMass;
        float OceanMassZ = CoreMass * 1.35f - OceanMassVolatiles - OceanMassEnergeticNuclide;

        float CoreMassVolatiles = CoreMass * 1e-4f * Random2;
        float CoreMassEnergeticNuclide = CoreMass * 5e-6f * Random3;
        float CoreMassZ = CoreMass - CoreMassVolatiles - CoreMassEnergeticNuclide;

        CoreMassVolatiles += OceanMassVolatiles;
        CoreMassEnergeticNuclide += OceanMassEnergeticNuclide;
        CoreMassZ += OceanMassZ;

        Planets[Index].SetCoreMass({ boost::multiprecision::int128_t(CoreMassZ), boost::multiprecision::int128_t(CoreMassVolatiles), boost::multiprecision::int128_t(CoreMassEnergeticNuclide) });

        return (CoreMassVolatiles + CoreMassEnergeticNuclide + CoreMassZ) / kEarthMass;
    };

    StarType = Star.GetStellarClass().GetStarType();
    if (StarType != Modules::StellarClass::StarType::kNeutronStar && StarType != Modules::StellarClass::StarType::kBlackHole) {
        // 抹掉坠入原恒星或恒星膨胀过程中吞掉的行星
        float StarRadiusMaxSol = 0.0f; // 恒星膨胀过程中达到的最大半径
        if (static_cast<int>(Star.GetEvolutionPhase()) <= 1) {
            ErasePlanets(Star.GetRadius());
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

        // 宜居带半径，单位 AU
        float InterChzRadiusAu = 0.0f;
        float OuterChzRadiusAu = 0.0f;
        float StarLuminosity = static_cast<float>(Star.GetLuminosity());
        InterChzRadiusAu = sqrt(StarLuminosity / (4.0f * kPi * 3000)) / kAuToMeter;
        OuterChzRadiusAu = sqrt(StarLuminosity / (4.0f * kPi * 600))  / kAuToMeter;

#ifdef DEBUG_OUTPUT
        std::println("Habitability zone: {} - {} AU", InterChzRadiusAu, OuterChzRadiusAu);
        std::println("");
#endif // DEBUG_OUTPUT

        // 冻结线半径，单位 AU
        float FrostLineAu = 0.0f;
        float CommonFactor = (std::pow(10.0f, 2.0f - StarInitialMassSol) + 1.0f) * (static_cast<float>(kSolarLuminosity) / (4.0f * kPi * kStefanBoltzmann * std::pow(270.0f, 4.0f)));
        if (StarInitialMassSol >= 0.075f && StarInitialMassSol < 0.43f) {
            FrostLineAu = CommonFactor * (0.23f * std::pow(StarInitialMassSol, 2.3f));
        } else if (StarInitialMassSol >= 0.43f && StarInitialMassSol < 2.0f) {
            FrostLineAu = CommonFactor * std::pow(StarInitialMassSol, 4.0f);
        } else if (StarInitialMassSol >= 2.0f && StarInitialMassSol <= 12.0f) {
            FrostLineAu = CommonFactor * (1.5f * std::pow(StarInitialMassSol, 3.5f));
        }
        FrostLineAu = std::sqrt(FrostLineAu) / kAuToMeter;

#ifdef DEBUG_OUTPUT
        std::println("Frost line: {} AU", FrostLineAu);
        std::println("");
#endif // DEBUG_OUTPUT

        for (std::size_t i = 0; i < PlanetCount; ++i) {
            NewCoreMassesSol[i] = Orbits[i].first.SemiMajorAxis / kAuToMeter > FrostLineAu ? CoreMassesSol[i] * 2.35f : CoreMassesSol[i];

            float PrevMainSequenceBalanceTemp = 0.0f;
            CommonFactor = (std::pow(10.0f, 2.0f - StarInitialMassSol) + 1.0f) * (static_cast<float>(kSolarLuminosity) / (4.0f * kPi * kStefanBoltzmann * std::pow(Orbits[i].first.SemiMajorAxis, 2.0f)));
            if (StarInitialMassSol >= 0.075f && StarInitialMassSol < 0.43f) {
                PrevMainSequenceBalanceTemp = CommonFactor * (0.23f * std::pow(StarInitialMassSol, 2.3f));
            } else if (StarInitialMassSol >= 0.43f && StarInitialMassSol < 2.0f) {
                PrevMainSequenceBalanceTemp = CommonFactor * std::pow(StarInitialMassSol, 4.0f);
            } else if (StarInitialMassSol >= 2.0f && StarInitialMassSol <= 12.0f) {
                PrevMainSequenceBalanceTemp = CommonFactor * (1.5f * std::pow(StarInitialMassSol, 3.5f));
            }

            PrevMainSequenceBalanceTemp = std::pow(PrevMainSequenceBalanceTemp, 0.25f);
            CommonFactor = PrevMainSequenceBalanceTemp * 4.638759e16f;

            // 开除大籍
            if (NewCoreMassesSol[i] * kSolarMass < _AsteroidUpperLimit || Planets[i].GetPlanetType() == AstroObject::Planet::PlanetType::kRockyAsteroidCluster) {
                if (NewCoreMassesSol[i] * kSolarMass < 1e19f) {
                    Orbits.erase(Orbits.begin() + i);
                    Planets.erase(Planets.begin() + i);
                    NewCoreMassesSol.erase(NewCoreMassesSol.begin() + i);
                    CoreMassesSol.erase(CoreMassesSol.begin() + i);
                    --PlanetCount;
                    --i;
                    continue;
                }

                if (static_cast<int>(Star.GetEvolutionPhase()) <= 1 && Orbits[i].first.SemiMajorAxis / kAuToMeter > FrostLineAu) {
                    Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kRockyIceAsteroidCluster);
                } else {
                    Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kRockyAsteroidCluster);
                }
            } else {
                if (Planets[i].GetPlanetType() != AstroObject::Planet::PlanetType::kRockyAsteroidCluster &&
                    Planets[i].GetPlanetType() != AstroObject::Planet::PlanetType::kRockyIceAsteroidCluster &&
                    CoreMassesSol[i] * kSolarMassToEarth < 0.1f &&
                    _AsteroidBeltProbability.Generate(_RandomEngine)) {
                    if (static_cast<int>(Star.GetEvolutionPhase()) <= 1 && Orbits[i].first.SemiMajorAxis / kAuToMeter > FrostLineAu) {
                        Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kRockyIceAsteroidCluster);
                    } else {
                        Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kRockyAsteroidCluster);
                    }

                    float Exponent = -3.0f + _CommonGenerator.Generate(_RandomEngine) * 3.0f;
                    float Factor = std::pow(10.0f, Exponent);
                    CoreMassesSol[i] *= Factor; // 对核心质量打个折扣
                } else {
                    if (Orbits[i].first.SemiMajorAxis / kAuToMeter < FrostLineAu) {
                        Planets[i].SetRadius(kEarthRadius * 1.007f * std::pow(NewCoreMassesSol[i] * kSolarMassToEarth, 1.0f / 3.7f));
                    } else {
                        Planets[i].SetRadius(kEarthRadius * 1.3f * std::pow(NewCoreMassesSol[i] * kSolarMassToEarth, 1.0f / 3.905f));
                    }

                    if ((NewCoreMassesSol[i] * kSolarMass / Planets[i].GetRadius()) > (CommonFactor / 4.0f)) {
                        Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kGasGiant);
                    } else if ((NewCoreMassesSol[i] * kSolarMass / Planets[i].GetRadius()) > (CommonFactor / 8.0f)) {
                        Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kIceGiant);
                    } else if ((CoreMassesSol[i] * kSolarMass / Planets[i].GetRadius()) > (CommonFactor / 18.0f) && Orbits[i].first.SemiMajorAxis / kAuToMeter > InterChzRadiusAu && Orbits[i].first.SemiMajorAxis / kAuToMeter < FrostLineAu && static_cast<int>(Star.GetEvolutionPhase()) < 1) {
                        Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kOceanic);
                    } else {
                        if (Orbits[i].first.SemiMajorAxis / kAuToMeter > FrostLineAu) {
                            Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kIcePlanet);
                            Planets[i].SetRadius(kEarthRadius * 1.3f * std::pow(NewCoreMassesSol[i] * kSolarMassToEarth, 1.0f / 3.905f));
                        } else {
                            // 行星默认为 Rocky，不需要再另行更改
                            Planets[i].SetRadius(kEarthRadius * 1.007f * std::pow(CoreMassesSol[i] * kSolarMassToEarth, 1.0f / 3.7f));
                        }
                    }
                }
            }
        }

#ifdef DEBUG_OUTPUT
        for (std::size_t i = 0; i < PlanetCount; ++i) {
            std::println("Before migration: planet {} semi-major axis: {} AU, initial core mass: {} earth, new core mass: {} earth, core radius: {} earth, type: {}", i + 1, Orbits[i].first.SemiMajorAxis / kAuToMeter, CoreMassesSol[i] * kSolarMassToEarth, NewCoreMassesSol[i] * kSolarMassToEarth, Planets[i].GetRadius() / kEarthRadius, static_cast<int>(Planets[i].GetPlanetType()));
        }

        std::println("");
#endif // DEBUG_OUTPUT

        // 巨行星内迁
        for (std::size_t i = 1; i < PlanetCount; ++i) {
            auto PlanetType = Planets[i].GetPlanetType();
            if (PlanetType == AstroObject::Planet::PlanetType::kIceGiant || PlanetType == AstroObject::Planet::PlanetType::kGasGiant) {
                if (_MigrationProbability.Generate(_RandomEngine)) {
                    int MigrationIndex = 0;
                    if (_WalkInProbability.Generate(_RandomEngine)) { // 夺舍，随机生成在该行星之前的位置
                        MigrationIndex = static_cast<int>(_CommonGenerator.Generate(_RandomEngine) * (i - 1));
                    } else { // 不夺舍，直接迁移到最近轨道
                        float Lower = std::log10(PlanetaryDiskTempData.InterRadiusAu / 3.0f);
                        float Upper = std::log10(PlanetaryDiskTempData.InterRadiusAu * 0.67f);
                        float Exponent = Lower + _CommonGenerator.Generate(_RandomEngine) * (Upper - Lower);
                        Orbits[0].first.SemiMajorAxis = std::pow(10.0f, Exponent) * kAuToMeter;
                    }

                    // 迁移到指定位置
                    Planets[i].SetMigration(true);
                    Planets[MigrationIndex] = Planets[i];
                    NewCoreMassesSol[MigrationIndex] = NewCoreMassesSol[i];
                    CoreMassesSol[MigrationIndex] = CoreMassesSol[i];
                    MigratedOriginSemiMajorAxisAu = Orbits[i].first.SemiMajorAxis / kAuToMeter;
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

#ifdef DEBUG_OUTPUT
        for (std::size_t i = 0; i < PlanetCount; ++i) {
            std::println("After migration: planet {} semi-major axis: {} AU, initial core mass: {} earth, new core mass: {} earth, core radius: {} earth, type: {}", i + 1, Orbits[i].first.SemiMajorAxis / kAuToMeter, CoreMassesSol[i] * kSolarMassToEarth, NewCoreMassesSol[i] * kSolarMassToEarth, Planets[i].GetRadius() / kEarthRadius, static_cast<int>(Planets[i].GetPlanetType()));
        }

        std::println("");

        for (std::size_t i = 0; i < PlanetCount; ++i) {
            std::println("Final orbits: planet {} semi-major axis: {} AU, initial core mass: {} earth, new core mass: {} earth, core radius: {} earth, type: {}", i + 1, Orbits[i].first.SemiMajorAxis / kAuToMeter, CoreMassesSol[i] * kSolarMassToEarth, NewCoreMassesSol[i] * kSolarMassToEarth, Planets[i].GetRadius() / kEarthRadius, static_cast<int>(Planets[i].GetPlanetType()));
        }

        std::println("");
#endif // DEBUG_OUTPUT
        // 计算最终质量
        for (std::size_t i = 0; i < PlanetCount; ++i) {
            float PlanetMassEarth = 0.0f;
            auto PlanetType = Planets[i].GetPlanetType();
            switch (PlanetType) {
            case AstroObject::Planet::PlanetType::kIcePlanet:
                Planets[i].SetRadius(kEarthRadius * 1.3f * std::pow(CalcIcePlanetMass(i), 1.0f / 3.905f));
                break;
            case AstroObject::Planet::PlanetType::kOceanic:
                Planets[i].SetRadius(kEarthRadius * 1.3f * std::pow(CalcOceanicMass(i), 1.0f / 3.905f));
                break;
            case AstroObject::Planet::PlanetType::kIceGiant:
                if (Star.GetStellarClass().GetStarType() == Modules::StellarClass::StarType::kWhiteDwarf && Orbits[i].first.SemiMajorAxis < 2.0f * StarRadiusMaxSol * kSolarRadius) {
                    Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kChthonian);
                    NewCoreMassesSol[i] = CoreMassesSol[i];
                    Planets[i].SetRadius(kEarthRadius * 1.007f * std::pow(CoreMassesSol[i] * kSolarMassToEarth, 1.0f / 3.7f));
                    PlanetMassEarth = CoreMassesSol[i] * kSolarMassToEarth;
                    break;
                }

                if ((PlanetMassEarth = CalcIceGiantMass(i)) < 10.0f) {
                    Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kSubIceGiant);
                }
                break;
            case AstroObject::Planet::PlanetType::kHotIceGiant:
                if ((PlanetMassEarth = CalcIceGiantMass(i)) < 10.0f) {
                    Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kHotSubIceGiant);
                }
                break;
            case AstroObject::Planet::PlanetType::kGasGiant:
                if (Star.GetStellarClass().GetStarType() == Modules::StellarClass::StarType::kWhiteDwarf && Orbits[i].first.SemiMajorAxis < 2.0f * StarRadiusMaxSol * kSolarRadius) {
                    Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kChthonian);
                    NewCoreMassesSol[i] = CoreMassesSol[i];
                    Planets[i].SetRadius(kEarthRadius * 1.007f * std::pow(CoreMassesSol[i] * kSolarMassToEarth, 1.0f / 3.7f));
                    PlanetMassEarth = CoreMassesSol[i] * kSolarMassToEarth;
                    break;
                }

                PlanetMassEarth = CalcGasGiantMass(i);
                break;
            case AstroObject::Planet::PlanetType::kRockyAsteroidCluster:
                PlanetMassEarth = CalcRockyAsteroidMass(i);
                break;
            case AstroObject::Planet::PlanetType::kRockyIceAsteroidCluster:
                PlanetMassEarth = CalcRockyIceAsteroidMass(i);
                break;
            default:
                break;
            }

            PlanetType = Planets[i].GetPlanetType();

            if (Star.GetStellarClass().GetStarType() == Modules::StellarClass::StarType::kWhiteDwarf && Star.GetAge() > 1e6) {
                // 处理白矮星引力散射
                if (Planets[i].GetPlanetType() == AstroObject::Planet::PlanetType::kRocky) {
                    if (_ScatteringProbability.Generate(_RandomEngine)) {
                        float Random = 4.0f + _CommonGenerator.Generate(_RandomEngine) * 16.0f; // 4.0 Rsun 高于洛希极限
                        Orbits[i].first.SemiMajorAxis = Random * kSolarRadius;
                    }
                }
            }

            // 计算最终半径
            if (PlanetType == AstroObject::Planet::PlanetType::kIceGiant ||
                PlanetType == AstroObject::Planet::PlanetType::kSubIceGiant ||
                PlanetType == AstroObject::Planet::PlanetType::kGasGiant) {
                if (PlanetMassEarth < 6.2f) {
                    Planets[i].SetRadius(kEarthRadius * 1.41f * std::pow(PlanetMassEarth, 1.0f / 3.905f));
                } else if (PlanetMassEarth < 15.0f) {
                    Planets[i].SetRadius(kEarthRadius * 0.6f * std::pow(PlanetMassEarth, 0.72f));
                } else {
                    float CommonFactor = PlanetMassEarth / (kJupiterMass / kEarthMass);
                    Planets[i].SetRadius(kEarthRadius * 11.0f * (0.96f + 0.21f * std::log10(CommonFactor) - 0.2f * std::pow(std::log10(CommonFactor), 2.0f) + 0.1f * std::pow(CommonFactor, 0.215f)));
                }
            }

            // 判断热木星
            float PoyntingVector = static_cast<float>(Star.GetLuminosity()) / (4.0f * kPi * std::pow(Orbits[i].first.SemiMajorAxis, 2.0f));
#ifdef DEBUG_OUTPUT
            std::println("Planet {} poynting vector: {} W/m^2", i + 1, PoyntingVector);
#endif // DEBUG_OUTPUT
            if (PoyntingVector >= 10000) {
                if (PlanetType == AstroObject::Planet::PlanetType::kGasGiant) {
                    Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kHotGasGiant);
                } else if (PlanetType == AstroObject::Planet::PlanetType::kIceGiant) {
                    Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kHotIceGiant);
                } else if (PlanetType == AstroObject::Planet::PlanetType::kSubIceGiant) {
                    Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kHotSubIceGiant);
                }
            }

            PlanetType = Planets[i].GetPlanetType();

            if (PlanetType == AstroObject::Planet::PlanetType::kHotIceGiant ||
                PlanetType == AstroObject::Planet::PlanetType::kHotSubIceGiant ||
                PlanetType == AstroObject::Planet::PlanetType::kHotGasGiant) {
                Planets[i].SetRadius(Planets[i].GetRadius() * std::pow(PoyntingVector / 10000.0f, 0.094f));
            }

            if (PlanetType == AstroObject::Planet::PlanetType::kOceanic && OuterChzRadiusAu <= Orbits[i].first.SemiMajorAxis / kAuToMeter) {
                Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kIcePlanet);
            }

            // 计算类地行星、大气层和生命
            if (Star.GetStellarClass().GetStarType() == Modules::StellarClass::StarType::kNormalStar) {
                PlanetType = Planets[i].GetPlanetType();
                PlanetMassEarth = Planets[i].GetMass() / kEarthMass;
                float PlanetMass = PlanetMassEarth * kEarthMass;
                float CoreMass = std::stof((Planets[i].GetCoreMass().EnergeticNuclide + Planets[i].GetCoreMass().Volatiles + Planets[i].GetCoreMass().Z).str());
                float Term1 = 1.6567e15f * static_cast<float>(std::pow(Star.GetLuminosity() / (4.0 * kPi * kStefanBoltzmann * std::pow(Orbits[i].first.SemiMajorAxis, 2.0f)), 0.25));
                float Term2 = PlanetMass / Planets[i].GetRadius();
                float MaxTerm = std::max(1.0f, Term1 / Term2);
                float EscapeFactor = std::pow(10.0f, 1.0f - MaxTerm);
                int   EvolutionPhase = static_cast<int>(Star.GetEvolutionPhase());
                if (PlanetType == AstroObject::Planet::PlanetType::kRocky &&
                    Orbits[i].first.SemiMajorAxis / kAuToMeter > InterChzRadiusAu &&
                    Orbits[i].first.SemiMajorAxis / kAuToMeter < OuterChzRadiusAu &&
                    EscapeFactor > 0.1f && EvolutionPhase < 1) { // 判断类地行星
                    Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kTerra);
                    // 计算新的海洋质量
                    float Exponent = -0.5f + _CommonGenerator.Generate(_RandomEngine) * 1.5f;
                    float Random1 = std::pow(10.0f, Exponent);
                    float NewOceanMass = CoreMass * Random1 * 1e-4f;
                    float NewOceanMassVolatiles = NewOceanMass / 9.0f;
                    float NewOceanMassEnergeticNuclide = NewOceanMass * 5e-5f / 9.0f;
                    float NewOceanMassZ = NewOceanMass - NewOceanMassVolatiles - NewOceanMassEnergeticNuclide;
                    Planets[i].SetOceanMass({ boost::multiprecision::int128_t(NewOceanMassZ), boost::multiprecision::int128_t(NewOceanMassVolatiles), boost::multiprecision::int128_t(NewOceanMassEnergeticNuclide) });
                    // 计算地壳矿脉
                    float Random2 = 0.0f;
                    if (PlanetType == AstroObject::Planet::PlanetType::kRocky) {
                        Random2 = 1.0f + _CommonGenerator.Generate(_RandomEngine) * 9.0f;
                    } else if (PlanetType == AstroObject::Planet::PlanetType::kTerra) {
                        Random2 = 0.1f + _CommonGenerator.Generate(_RandomEngine) * 0.9f;
                    }
                    float CrustMineralMass = Random2 * 1e-9f * std::pow(PlanetMass / kEarthMass, 2.0f) * kEarthMass;
                    Planets[i].SetCrustMineralMass(CrustMineralMass);
                }

                PlanetType = Planets[i].GetPlanetType();

                // 计算次生大气
                if (PlanetType == AstroObject::Planet::PlanetType::kRocky ||
                    PlanetType == AstroObject::Planet::PlanetType::kTerra ||
                    PlanetType == AstroObject::Planet::PlanetType::kOceanic ||
                    PlanetType == AstroObject::Planet::PlanetType::kIcePlanet) {
                    float Exponent = _CommonGenerator.Generate(_RandomEngine);
                    float Random1 = std::pow(10.0f, Exponent);
                    float NewAtmosphereMass = EscapeFactor * PlanetMass * Random1 * 1e-5f;
                    if (PlanetType == AstroObject::Planet::PlanetType::kTerra) {
                        NewAtmosphereMass *= 0.035f;
                    } else if (PlanetType == AstroObject::Planet::PlanetType::kIcePlanet) {
                        if (PoyntingVector > 2) { // 保证氮气不会液化
                            NewAtmosphereMass = std::pow(EscapeFactor, 2.0f) * PlanetMass * Random1 * 1e-5f;
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
                        Planets[i].SetAtmosphereMassZ(NewAtmosphereMassZ);
                        Planets[i].SetAtmosphereMassVolatiles(NewAtmosphereMassVolatiles);
                        Planets[i].SetAtmosphereMassEnergeticNuclide(NewAtmosphereMassEnergeticNuclide);
                    } else { // 只调整核心质量（核心就是整个星球）
                        float CoreMassVolatiles = std::stof(Planets[i].GetCoreMass().Volatiles.str());
                        float CoreMassEnergeticNuclide = std::stof(Planets[i].GetCoreMass().EnergeticNuclide.str());
                        CoreMassVolatiles += 33.1f * std::pow(Planets[i].GetRadius(), 2.0f);
                        CoreMassEnergeticNuclide += 3.31e-4f * std::pow(Planets[i].GetRadius(), 2.0f);
                        Planets[i].SetCoreMassVolatiles(CoreMassVolatiles);
                        Planets[i].SetCoreMassEnergeticNuclide(CoreMassEnergeticNuclide);
                    }
                }

                PlanetType = Planets[i].GetPlanetType();
                // 计算生命和文明
                if (PlanetType == AstroObject::Planet::PlanetType::kTerra) {
                    bool bCanHasLife = false;
                    if (Star.GetAge() > 5e8) {
                        if (_bContainUltravioletChz) {
                            if (Star.GetMass() / kSolarMass > 0.75f && Star.GetMass() / kSolarMass < 1.5f) {
                                bCanHasLife = true;
                            }
                        } else {
                            bCanHasLife = true;
                        }
                    }

                    if (bCanHasLife && _LifeOccurrenceProbability.Generate(_RandomEngine)) {
                        float Random1 = 0.5f + _CommonGenerator.Generate(_RandomEngine) + 1.5f;
                        auto LifePhase = static_cast<AstroObject::Planet::LifePhase>(std::min(4, std::max(1, static_cast<int>(Random1 * Star.GetAge() / (5 * std::pow(10, 8))))));

                        if (LifePhase == AstroObject::Planet::LifePhase::kCenoziocEra) {
                            float Random2 = 1.0f + _CommonGenerator.Generate(_RandomEngine) * 999.0f;
                            if (_AsiFiltedProbability.Generate(_RandomEngine)) {
                                LifePhase = AstroObject::Planet::LifePhase::kSatTeeTouyButAsi; // 被 ASI 去城市化了
                                Planets[i].SetCrustMineralMass(Random2 * 1e16f + Planets[i].GetCrustMineralMassFloat());
                            } else {
                                Planets[i].SetCrustMineralMass(Random2 * 1e15f + Planets[i].GetCrustMineralMassFloat());
                            }
                        }

                        const std::array<float, 7>* ProbabilityListPtr = nullptr;
                        int IntegerPart = 0;
                        if (LifePhase != AstroObject::Planet::LifePhase::kCenoziocEra &&
                            LifePhase != AstroObject::Planet::LifePhase::kSatTeeTouy  &&
                            LifePhase != AstroObject::Planet::LifePhase::kSatTeeTouyButAsi) {
                            Planets[i].SetCivilizationLevel(0.0f);
                        } else if (LifePhase == AstroObject::Planet::LifePhase::kCenoziocEra) {
                            ProbabilityListPtr = &_kProbabilityListForCenoziocEra;
                        } else if (LifePhase == AstroObject::Planet::LifePhase::kSatTeeTouyButAsi) {
                            ProbabilityListPtr = &_kProbabilityListForSatTeeTouyButAsi;
                        }

                        if (ProbabilityListPtr != nullptr) {
                            float Random = _CommonGenerator.Generate(_RandomEngine);
                            float CumulativeProbability = 0.0f;

                            for (int i = 0; i < 7; ++i) {
                                CumulativeProbability += (*ProbabilityListPtr)[i];
                                if (Random < CumulativeProbability) {
                                    IntegerPart = i + 1;
                                    break;
                                }
                            }

                            if (IntegerPart >= 7) {
                                if (LifePhase == AstroObject::Planet::LifePhase::kCenoziocEra) {
                                    LifePhase = AstroObject::Planet::LifePhase::kSatTeeTouy;
                                } else if (LifePhase == AstroObject::Planet::LifePhase::kSatTeeTouyButAsi) {
                                    LifePhase = AstroObject::Planet::LifePhase::kNewCivilization;
                                }
                            }

                            float FractionalPart = _CommonGenerator.Generate(_RandomEngine);
                            Planets[i].SetCivilizationLevel(static_cast<float>(IntegerPart) + FractionalPart);
                        }

                        Planets[i].SetLifePhase(LifePhase);
                    }
                }
#ifdef DEBUG_OUTPUT
                PlanetType = Planets[i].GetPlanetType();
                PlanetMassEarth = Planets[i].GetMass() / kEarthMass;
                float AtmosphereMassZ = std::stof(Planets[i].GetAtmosphereMass().Z.str());
                float AtmosphereMassVolatiles = std::stof(Planets[i].GetAtmosphereMass().Volatiles.str());
                float AtmosphereMassEnergeticNuclide = std::stof(Planets[i].GetAtmosphereMass().EnergeticNuclide.str());
                float OceanMassZ = std::stof(Planets[i].GetOceanMass().Z.str());
                float OceanMassVolatiles = std::stof(Planets[i].GetOceanMass().Volatiles.str());
                float OceanMassEnergeticNuclide = std::stof(Planets[i].GetOceanMass().EnergeticNuclide.str());
                float CoreMassZ = std::stof(Planets[i].GetCoreMass().Z.str());
                float CoreMassVolatiles = std::stof(Planets[i].GetCoreMass().Volatiles.str());
                float CoreMassEnergeticNuclide = std::stof(Planets[i].GetCoreMass().EnergeticNuclide.str());
                float CrustMineralMass = std::stof(Planets[i].GetCrustMineralMass().str());
                float AtmospherePressure = (kGravityConstant * PlanetMass * (AtmosphereMassZ + AtmosphereMassVolatiles + AtmosphereMassEnergeticNuclide)) / (4.0f * kPi * std::pow(Planets[i].GetRadius(), 4.0f));
                std::println("Planet detail:\natmo mass z: {:.2E} kg, atmo mass vol: {:.2E} kg, atmo mass nuc: {:.2E} kg\ncore mass z: {:.2E} kg, core mass vol: {:.2E} kg, core mass nuc: {:.2E} kg\nocean mass z: {:.2E} kg, ocean mass vol: {:.2E} kg, ocean mass nuc: {:.2E} kg\ncrust mineral mass: {:.2E} kg, atmo pressure: {:.2f} atm", AtmosphereMassZ, AtmosphereMassVolatiles, AtmosphereMassEnergeticNuclide, CoreMassZ, CoreMassVolatiles, CoreMassEnergeticNuclide, OceanMassZ, OceanMassVolatiles, OceanMassEnergeticNuclide, CrustMineralMass, AtmospherePressure / kPascalToAtm);
                std::println("semi-major axis: {} AU, mass: {} earth, radius: {} earth, type: {}", Orbits[i].first.SemiMajorAxis / kAuToMeter, PlanetMassEarth, Planets[i].GetRadius() / kEarthRadius, static_cast<int>(Planets[i].GetPlanetType()));
                if (PlanetType == AstroObject::Planet::PlanetType::kTerra) {
                    std::println("life: {}, civilization level: {:.2f}", static_cast<int>(Planets[i].GetLifePhase()), Planets[i].GetCivilizationLevel());
                }
                std::println("");
#endif // DEBUG_OUTPUT
            }
        }
    } else {
        for (std::size_t i = 0; i < PlanetCount; ++i) {
            if (CoreMassesSol[i] * kSolarMass < 1e21f && _AsteroidBeltProbability.Generate(_RandomEngine)) {
                Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kRockyAsteroidCluster);
            } else {
                // 行星默认为 Rocky
                Planets[i].SetRadius(kEarthRadius * 1.007f * std::pow(CoreMassesSol[i] * kSolarMassToEarth, 1.0f / 3.7f));
            }
#ifdef DEBUG_OUTPUT
            float PlanetMassEarth = Planets[i].GetMass() / kEarthMass;
            std::println("Final system: planet {} semi-major axis: {} AU, mass: {} earth, radius: {} earth, type: {}", i + 1, Orbits[i].first.SemiMajorAxis / kAuToMeter, PlanetMassEarth, Planets[i].GetRadius() / kEarthRadius, static_cast<int>(Planets[i].GetPlanetType()));
#endif // DEBUG_OUTPUT
        }
    }
#ifdef DEBUG_OUTPUT
    std::println("");
#endif // DEBUG_OUTPUT

    for (std::size_t i = 0; i < PlanetCount; ++i) {
        Planets[i].SetAge(Star.GetAge() - 1e6f);
        Orbits[i].second.emplace_back(&Planets[i]);
    }

    TransformData();
}

void OrbitalGenerator::GenOrbitElements(StellarSystem::OrbitalElements& Orbit) {
    Orbit.Eccentricity             = _CommonGenerator.Generate(_RandomEngine) * 0.05f;
    Orbit.Inclination              = _CommonGenerator.Generate(_RandomEngine) * 4.0f - 2.0f;
    Orbit.LongitudeOfAscendingNode = _CommonGenerator.Generate(_RandomEngine) * 360.0f;
    Orbit.ArgumentOfPeriapsis      = _CommonGenerator.Generate(_RandomEngine) * 360.0f;
    Orbit.TrueAnomaly              = _CommonGenerator.Generate(_RandomEngine) * 360.0f;
}

const std::array<float, 7> OrbitalGenerator::_kProbabilityListForCenoziocEra{ 0.02f, 0.005f, 1e-4f, 1e-6f, 5e-7f, 4e-7f, 1e-6f };
const std::array<float, 7> OrbitalGenerator::_kProbabilityListForSatTeeTouyButAsi{ 0.2f, 0.05f, 0.001f, 1e-5f, 1e-4f, 1e-4f, 1e-4f };

_MODULES_END
_NPGS_END
