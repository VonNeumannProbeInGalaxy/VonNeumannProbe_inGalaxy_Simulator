#include "OrbitalGenerator.h"

#include <cmath>
#include <algorithm>
#include <iterator>
#include <print>
#include <utility>

#include "Engine/Core/Constants.h"

#define DEBUG_OUTPUT

_NPGS_BEGIN
_MODULES_BEGIN

OrbitalGenerator::OrbitalGenerator(const std::seed_seq& SeedSeq)
    : _RandomEngine(SeedSeq), _CommonGenerator(0.0f, 1.0f)
{}

void OrbitalGenerator::GenerateOrbitals(StellarSystem& System) {}

void OrbitalGenerator::GeneratePlanets(StellarSystem& System) {
    // 目前只处理单星情况
    // 变量名未标注单位均为国际单位制
    PlanetaryDisk PlanetaryDiskTempData{};
    auto& Star = System.StarData().front();
    float DiskBase = 1.0f + _CommonGenerator.Generate(_RandomEngine); // 基准随机数，1-2 之间
    float StarInitialMassSol = Star.GetInitialMass() / static_cast<float>(kSolarMass);
    // 生成原行星盘数据
    if (Star.GetStellarClass().GetStarType() == StellarClass::StarType::kNormalStar) {
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
        float StarMassSol = static_cast<float>(Star.GetMass() / kSolarMass);
        float DiskMassSol = DiskBase * 1e-5f * StarMassSol;
        PlanetaryDiskTempData.InterRadiusAu = 0.01f;
        PlanetaryDiskTempData.OuterRadiusAu = 1.0f;
        PlanetaryDiskTempData.DiskMassSol = DiskMassSol;
        PlanetaryDiskTempData.DustMassSol = DiskMassSol;
    } else {
        return;
    }

    // 生成行星个数
    std::size_t PlanetCount = 0;
    if (Star.GetStellarClass().GetStarType() == StellarClass::StarType::kNormalStar) {
        if (StarInitialMassSol < 0.6f) {
            PlanetCount = static_cast<std::size_t>(4.0f + _CommonGenerator.Generate(_RandomEngine) * 4.0f);
        } else if (StarInitialMassSol < 0.9f) {
            PlanetCount = static_cast<std::size_t>(5.0f + _CommonGenerator.Generate(_RandomEngine) * 5.0f);
        } else if (StarInitialMassSol < 3.0f) {
            PlanetCount = static_cast<std::size_t>(6.0f + _CommonGenerator.Generate(_RandomEngine) * 6.0f);
        } else {
            PlanetCount = static_cast<std::size_t>(4.0f + _CommonGenerator.Generate(_RandomEngine) * 4.0f);
        }
        PlanetCount = static_cast<std::size_t>(6.0f + _CommonGenerator.Generate(_RandomEngine) * 6.0f);
    } else if (Star.GetEvolutionEnding() == AstroObject::Star::Death::kWhiteDwarfMerge) {
        PlanetCount = static_cast<std::size_t>(2.0f + _CommonGenerator.Generate(_RandomEngine) * 2.3f);
    } else {
        return;
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

    AstroObject::Planet::MassProperties CoreMass;
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
        std::println("Generate basic core mass: planet {} initial core mass: {} earth", i + 1, CoreMassesSol[i] * kSolarMassToEarth);
#endif // DEBUG_OUTPUT
    }

#ifdef DEBUG_OUTPUT
    std::println("");
#endif // DEBUG_OUTPUT

    // 初始化轨道
    std::vector<std::pair<StellarSystem::OrbitalElements, AstroObject::CelestialBody*>> Orbits(PlanetCount);
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
    if (DiskAge >= Star.GetAge()) {
        for (auto& Planet : Planets) {
            Planet.SetPlanetType(AstroObject::Planet::PlanetType::kRockyAsteroidCluster);
        }
    } else {
        for (auto& Planet : Planets) {
            Planet.SetPlanetType(AstroObject::Planet::PlanetType::kRocky);
        }
    }

    std::vector<float> NewCoreMassesSol(PlanetCount); // 吸积核心质量，单位太阳

    // 计算冻结线半径
    float FrostLineAu = 0.0f; // 冻结线半径，单位 AU
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
    std::println("PMS frost line: {} AU", FrostLineAu);
    std::println("");
#endif // DEBUG_OUTPUT

    // Lambda 封装
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

    auto CalcOceanicMass = [&](std::size_t Index) -> float {
        float Random1 = _CommonGenerator.Generate(_RandomEngine) * 1.35f;
        float Random2 = 0.9f + _CommonGenerator.Generate(_RandomEngine) * 0.2f;
        float Random3 = 0.9f + _CommonGenerator.Generate(_RandomEngine) * 0.2f;

        float OceanMassVolatiles = static_cast<float>((kSolarMass * CoreMassesSol[Index] * Random1) / 9.0f);
        float OceanMassEnergeticNuclide = 5e-5f * OceanMassVolatiles;
        float OceanMassZ = static_cast<float>(kSolarMass * CoreMassesSol[Index] * Random1 - OceanMassVolatiles - OceanMassEnergeticNuclide);

        float CoreMassVolatiles = static_cast<float>(kSolarMass * CoreMassesSol[Index] * 1e-4f * Random2);
        float CoreMassEnergeticNuclide = static_cast<float>(kSolarMass * CoreMassesSol[Index] * 5e-6f * Random3);
        float CoreMassZ = static_cast<float>(kSolarMass * CoreMassesSol[Index] - CoreMassVolatiles - CoreMassEnergeticNuclide);

        Planets[Index].SetOceanMass({ boost::multiprecision::int128_t(OceanMassZ), boost::multiprecision::int128_t(OceanMassVolatiles), boost::multiprecision::int128_t(OceanMassEnergeticNuclide) });
        Planets[Index].SetCoreMass({ boost::multiprecision::int128_t(CoreMassZ), boost::multiprecision::int128_t(CoreMassVolatiles), boost::multiprecision::int128_t(CoreMassEnergeticNuclide) });

        return (OceanMassVolatiles + OceanMassEnergeticNuclide + OceanMassZ + CoreMassVolatiles + CoreMassEnergeticNuclide + CoreMassZ) / kEarthMass;
    };

    auto CalcIceGiantMass = [&](std::size_t Index) -> float {
        float Random1 = std::log10(2.35f) + _CommonGenerator.Generate(_RandomEngine) * (std::min(std::log10(30.0f), std::log10(1.0f / 0.0142f * std::pow(10.0f, Star.GetFeH())) - std::log10(2.35f)));
        float Random2 = 0.9f + _CommonGenerator.Generate(_RandomEngine) * 0.2f;
        float Random3 = 0.9f + _CommonGenerator.Generate(_RandomEngine) * 0.2f;

        float AtmosphereMassVolatiles = static_cast<float>(kSolarMass * (NewCoreMassesSol[Index] - CoreMassesSol[Index]) / 9.0f + kSolarMass * CoreMassesSol[Index] * (std::pow(10.0f, Random1) - 2.35f) / 6.0f);
        float AtmosphereMassEnergeticNuclide = 5e-5f * AtmosphereMassVolatiles;
        float AtmosphereMassZ = static_cast<float>(kSolarMass * (std::pow(10.0f, Random1) - 1.0f) - AtmosphereMassVolatiles - AtmosphereMassEnergeticNuclide);

        float CoreMassVolatiles = static_cast<float>(kSolarMass * CoreMassesSol[Index] * 1e-4f * Random2);
        float CoreMassEnergeticNuclide = static_cast<float>(kSolarMass * CoreMassesSol[Index] * 5e-6f * Random3);
        float CoreMassZ = static_cast<float>(kSolarMass * CoreMassesSol[Index] - CoreMassVolatiles - CoreMassEnergeticNuclide);

        Planets[Index].SetAtmosphereMass({ boost::multiprecision::int128_t(AtmosphereMassZ), boost::multiprecision::int128_t(AtmosphereMassVolatiles), boost::multiprecision::int128_t(AtmosphereMassEnergeticNuclide) });
        Planets[Index].SetCoreMass({ boost::multiprecision::int128_t(CoreMassZ), boost::multiprecision::int128_t(CoreMassVolatiles), boost::multiprecision::int128_t(CoreMassEnergeticNuclide) });

        return (AtmosphereMassVolatiles + AtmosphereMassEnergeticNuclide + AtmosphereMassZ + CoreMassVolatiles + CoreMassEnergeticNuclide + CoreMassZ) / kEarthMass;
    };

    auto CalcGasGiantMass = [&](std::size_t Index) -> float {
        float Random1 = std::log10(4.0f) + _CommonGenerator.Generate(_RandomEngine) * (std::min(std::log10(30.0f), std::log10(1.0f / 0.0142f * std::pow(10.0f, Star.GetFeH())) - std::log10(4.0f)));
        float Random2 = 0.9f + _CommonGenerator.Generate(_RandomEngine) * 0.2f;
        float Random3 = 0.9f + _CommonGenerator.Generate(_RandomEngine) * 0.2f;

        float AtmosphereMassZ = static_cast<float>((0.0142f * std::pow(10.0f, Star.GetFeH())) * kSolarMass * CoreMassesSol[Index] * (std::pow(10.0f, Random1) - 2.35f) + (1.0f - ((1.0f + 5e-5f) / 9.0f)) * kSolarMass * (NewCoreMassesSol[Index] - CoreMassesSol[Index]));
        float AtmosphereMassEnergeticNuclide = static_cast<float>(5e-5f * kSolarMass * (CoreMassesSol[Index] * (std::pow(10.0f, Random1) - 2.35f) + (NewCoreMassesSol[Index] - CoreMassesSol[Index]) / 9.0f));
        float AtmosphereMassVolatiles = static_cast<float>(kSolarMass * (NewCoreMassesSol[Index] - CoreMassesSol[Index]) / 9.0f + kSolarMass * CoreMassesSol[Index] * (std::pow(10.0f, Random1) - 2.35f) / 6.0f);

        float CoreMassVolatiles = static_cast<float>(kSolarMass * CoreMassesSol[Index] * 1e-4f * Random2);
        float CoreMassEnergeticNuclide = static_cast<float>(kSolarMass * CoreMassesSol[Index] * 5e-6f * Random3);
        float CoreMassZ = static_cast<float>(kSolarMass * CoreMassesSol[Index] - CoreMassVolatiles - CoreMassEnergeticNuclide);

        Planets[Index].SetAtmosphereMass({ boost::multiprecision::int128_t(AtmosphereMassZ), boost::multiprecision::int128_t(AtmosphereMassVolatiles), boost::multiprecision::int128_t(AtmosphereMassEnergeticNuclide) });
        Planets[Index].SetCoreMass({ boost::multiprecision::int128_t(CoreMassZ), boost::multiprecision::int128_t(CoreMassVolatiles), boost::multiprecision::int128_t(CoreMassEnergeticNuclide) });

        return (AtmosphereMassVolatiles + AtmosphereMassEnergeticNuclide + AtmosphereMassZ + CoreMassVolatiles + CoreMassEnergeticNuclide + CoreMassZ) / kEarthMass;
    };

    if (Star.GetStellarClass().GetStarType() == Modules::StellarClass::StarType::kNormalStar) {
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

            if (NewCoreMassesSol[i] * static_cast<float>(kSolarMass) < 1e21f || Planets[i].GetPlanetType() == AstroObject::Planet::PlanetType::kRockyAsteroidCluster) {
                if (Orbits[i].first.SemiMajorAxis / kAuToMeter > FrostLineAu) {
                    Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kRockyIceAsteroidCluster);
                } else {
                    Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kRockyAsteroidCluster);
                }
            } else {
                if (Orbits[i].first.SemiMajorAxis / kAuToMeter < FrostLineAu) {
                    Planets[i].SetRadius(kEarthRadius * 1.007f * std::pow(NewCoreMassesSol[i] * kSolarMassToEarth, 1.0f / 3.7f));
                } else {
                    Planets[i].SetRadius(kEarthRadius * 1.3f * std::pow(NewCoreMassesSol[i] * kSolarMassToEarth, 1.0f / 3.905f));
                }

                if ((NewCoreMassesSol[i] * kSolarMass / Planets[i].GetRadius()) > (CommonFactor / 2.0f)) {
                    Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kGasGiant);
                } else if ((NewCoreMassesSol[i] * kSolarMass / Planets[i].GetRadius()) > (CommonFactor / 8.0f)) {
                    Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kIceGiant);
                } else if ((CoreMassesSol[i] * kSolarMass / Planets[i].GetRadius()) > (CommonFactor / 18.0f) && Orbits[i].first.SemiMajorAxis / kAuToMeter > InterChzRadiusAu && Orbits[i].first.SemiMajorAxis / kAuToMeter < FrostLineAu) {
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

#ifdef DEBUG_OUTPUT
        for (std::size_t i = 0; i < PlanetCount; ++i) {
            std::println("Before migration: planet {} semi-major axis: {} AU, initial core mass: {} earth, new core mass: {} earth, core radius: {} earth, type: {}", i + 1, Orbits[i].first.SemiMajorAxis / kAuToMeter, CoreMassesSol[i] * kSolarMassToEarth, NewCoreMassesSol[i] * kSolarMassToEarth, Planets[i].GetRadius() / kEarthRadius, static_cast<int>(Planets[i].GetPlanetType()));
        }

        std::println("");
#endif // DEBUG_OUTPUT

        // 巨行星内迁
        BernoulliDistribution MigrationProbability(0.1);
        for (std::size_t i = 1; i < PlanetCount; ++i) {
            if (Planets[i].GetPlanetType() == AstroObject::Planet::PlanetType::kIceGiant ||
                Planets[i].GetPlanetType() == AstroObject::Planet::PlanetType::kGasGiant) {
                if (MigrationProbability.Generate(_RandomEngine)) {
                    BernoulliDistribution WalkInProbability(0.8); // 0.8 夺舍概率
                    int MigrationIndex = 0;
                    if (WalkInProbability.Generate(_RandomEngine)) { // 如果夺舍，随机生成在该行星之前的位置
                        MigrationIndex = static_cast<int>(_CommonGenerator.Generate(_RandomEngine) * (i - 1));
                    } else { // 不夺舍，直接迁移到最近轨道
                        float Lower = std::log10(PlanetaryDiskTempData.InterRadiusAu / 3.0f);
                        float Upper = std::log10(PlanetaryDiskTempData.InterRadiusAu * 0.67f);
                        float Exponent = Lower + _CommonGenerator.Generate(_RandomEngine) * (Upper - Lower);
                        Orbits[0].first.SemiMajorAxis = std::pow(10.0f, Exponent) * kAuToMeter;
                    }

                    // 迁移到指定位置
                    Planets[MigrationIndex] = Planets[i];
                    NewCoreMassesSol[MigrationIndex] = NewCoreMassesSol[i];
                    CoreMassesSol[MigrationIndex] = CoreMassesSol[i];
                    // 抹掉内迁途中的经过的其他行星
                    Planets.erase(Planets.begin() + MigrationIndex + 1, Planets.begin() + i + 1);
                    Orbits.erase(Orbits.begin() + MigrationIndex + 1, Orbits.begin() + i + 1);
                    NewCoreMassesSol.erase(NewCoreMassesSol.begin() + MigrationIndex + 1, NewCoreMassesSol.begin() + i + 1);
                    CoreMassesSol.erase(CoreMassesSol.begin() + MigrationIndex + 1, CoreMassesSol.begin() + i + 1);

                    PlanetCount = Planets.size();
                    break;
                }
            }
        }

#ifdef DEBUG_OUTPUT
        for (std::size_t i = 0; i < PlanetCount; ++i) {
            std::println("After migration: planet {} semi-major axis: {} AU, initial core mass: {} earth, new core mass: {} earth, core radius: {} earth, type: {}", i + 1, Orbits[i].first.SemiMajorAxis / kAuToMeter, CoreMassesSol[i] * kSolarMassToEarth, NewCoreMassesSol[i] * kSolarMassToEarth, Planets[i].GetRadius() / kEarthRadius, static_cast<int>(Planets[i].GetPlanetType()));
        }

        std::println("");
#endif // DEBUG_OUTPUT

        // 抹掉坠入原恒星或恒星膨胀过程中吞掉的行星
        if (static_cast<int>(Star.GetEvolutionPhase()) <= 1) {
            ErasePlanets(Star.GetRadius());
        } else {
            float StarRadiusMaxSol = 0.0f; // 恒星膨胀过程中达到的最大半径
            if (StarInitialMassSol < 0.75f) {
                StarRadiusMaxSol = 104 * std::pow(2.0f * StarInitialMassSol, 3.0f) + 0.1f;
            } else {
                StarRadiusMaxSol = 400 * std::pow(StarInitialMassSol - 0.75f, 1.0f / 3.0f);
            }

            ErasePlanets(StarRadiusMaxSol * static_cast<float>(kSolarMass));
        }

#ifdef DEBUG_OUTPUT
        for (std::size_t i = 0; i < PlanetCount; ++i) {
            std::println("Final system: planet {} semi-major axis: {} AU, initial core mass: {} earth, new core mass: {} earth, core radius: {} earth, type: {}", i + 1, Orbits[i].first.SemiMajorAxis / kAuToMeter, CoreMassesSol[i] * kSolarMassToEarth, NewCoreMassesSol[i] * kSolarMassToEarth, Planets[i].GetRadius() / kEarthRadius, static_cast<int>(Planets[i].GetPlanetType()));
        }

        std::println("");
#endif // DEBUG_OUTPUT

        for (std::size_t i = 0; i != PlanetCount; ++i) {
            float PlanetMassEarth = 0.0f;
            auto PlanetType = Planets[i].GetPlanetType();
            switch (PlanetType) {
            case AstroObject::Planet::PlanetType::kOceanic:
                Planets[i].SetRadius(kEarthRadius * 1.3f * std::pow(CalcOceanicMass(i), 1.0f / 3.905f));
                break;
            case AstroObject::Planet::PlanetType::kIceGiant:
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
                PlanetMassEarth = CalcGasGiantMass(i);
                break;
            default:
                break;
            }

            if (PlanetType == AstroObject::Planet::PlanetType::kIceGiant ||
                PlanetType == AstroObject::Planet::PlanetType::kSubIceGiant ||
                PlanetType == AstroObject::Planet::PlanetType::kGasGiant) {
                if (PlanetMassEarth < 6.2f) {
                    Planets[i].SetRadius(kEarthRadius * 1.41f * std::pow(PlanetMassEarth, 1.0f / 3.905f));
                } else if (PlanetMassEarth < 15.0f) {
                    Planets[i].SetRadius(kEarthRadius * 0.6f * std::pow(PlanetMassEarth, 0.72f));
                } else {
                    float CommonFactor = PlanetMassEarth / 317.8f;
                    Planets[i].SetRadius(kEarthRadius * 11.0f * (0.96f + 0.21f * std::log10(CommonFactor) - 0.2f * std::pow(std::log10(CommonFactor), 2.0f) + 0.1f * std::pow(CommonFactor, 0.215f)));
                }
            }

            if (PlanetType == AstroObject::Planet::PlanetType::kHotIceGiant ||
                PlanetType == AstroObject::Planet::PlanetType::kHotSubIceGiant ||
                PlanetType == AstroObject::Planet::PlanetType::kHotGasGiant) {
                if (PlanetMassEarth < 6.2f) {
                    Planets[i].SetRadius(kEarthRadius * 2.82f * std::pow(PlanetMassEarth, 1.0f / 3.905f));
                } else if (PlanetMassEarth < 15.0f) {
                    Planets[i].SetRadius(kEarthRadius * 1.2f * std::pow(PlanetMassEarth, 0.72f));
                } else {
                    float CommonFactor = std::log10(PlanetMassEarth / 317.8f);
                    Planets[i].SetRadius(kEarthRadius * 22.0f * (0.96f + 0.21f * CommonFactor - 0.2f * std::pow(CommonFactor, 2.0f) + 0.1f * std::pow(CommonFactor, 0.215f)));
                }
            }
        }
    } else {
        for (std::size_t i = 0; i != PlanetCount; ++i) {
            Planets[i].SetRadius(kEarthRadius * 1.007f * std::pow(CoreMassesSol[i] * kSolarMassToEarth, 1.0f / 3.7f));
        }
    }

    if (Star.GetStellarClass().GetStarType() == Modules::StellarClass::StarType::kWhiteDwarf) {
        for (std::size_t i = 0; i != PlanetCount; ++i) {
            if (Planets[i].GetPlanetType() == AstroObject::Planet::PlanetType::kGasGiant ||
                Planets[i].GetPlanetType() == AstroObject::Planet::PlanetType::kIceGiant) {
                Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kChthonian);
                NewCoreMassesSol[i] = CoreMassesSol[i];
                Planets[i].SetRadius(1.007f * std::pow(CoreMassesSol[i] * kSolarMassToEarth, 1.0f / 3.7f));
            }
        }
    } else {
        for (std::size_t i = 0; i != PlanetCount; ++i) {
            if ((Star.GetLuminosity() / (4.0f * kPi * std::pow(Orbits[i].first.SemiMajorAxis, 2.0f))) >= 10000) {
                if (Planets[i].GetPlanetType() == AstroObject::Planet::PlanetType::kGasGiant) {
                    Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kHotGasGiant);
                } else if (Planets[i].GetPlanetType() == AstroObject::Planet::PlanetType::kIceGiant) {
                    Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kHotIceGiant);
                }
            }
        }
    }

    if (Star.GetStellarClass().GetStarType() == Modules::StellarClass::StarType::kWhiteDwarf) {
        for (std::size_t i = 0; i != PlanetCount; ++i) {
            if (Planets[i].GetPlanetType() == AstroObject::Planet::PlanetType::kRocky) {
                BernoulliDistribution ScatteringProbability(0.1);
                if (ScatteringProbability.Generate(_RandomEngine)) {
                    float StarRadiusSol = Star.GetRadius() / kSolarRadius;
                    float Random = 1.0f + _CommonGenerator.Generate(_RandomEngine) * 19.0f;
                    Orbits[i].first.SemiMajorAxis = StarRadiusSol * Random * kSolarRadius;
                    break;
                }
            }
        }
    }

    for (std::size_t i = 0; i != PlanetCount; ++i) {
        Planets[i].SetAge(Star.GetAge() - 1e6f);
        Orbits[i].second = &Planets[i];
    }

    std::transform(std::make_move_iterator(Planets.begin()),
                   std::make_move_iterator(Planets.end()),
                   std::back_inserter(System.PlanetData()),
                   [](AstroObject::Planet&& Planet) -> AstroObject::Planet {
                       return std::move(Planet);
                   });

    std::transform(std::make_move_iterator(Orbits.begin()),
                   std::make_move_iterator(Orbits.end()),
                   std::back_inserter(System.OrbitData()),
                   [](std::pair<StellarSystem::OrbitalElements, AstroObject::CelestialBody*>&& Orbit) -> std::pair<StellarSystem::OrbitalElements, AstroObject::CelestialBody*> {
                       return std::move(Orbit);
                   });
}

void OrbitalGenerator::GenOrbitElements(StellarSystem::OrbitalElements& Orbit) {
    Orbit.Eccentricity             = _CommonGenerator.Generate(_RandomEngine) * 0.05f;
    Orbit.Inclination              = _CommonGenerator.Generate(_RandomEngine) * 4.0f - 2.0f;
    Orbit.LongitudeOfAscendingNode = _CommonGenerator.Generate(_RandomEngine) * 360.0f;
    Orbit.ArgumentOfPeriapsis      = _CommonGenerator.Generate(_RandomEngine) * 360.0f;
    Orbit.TrueAnomaly              = _CommonGenerator.Generate(_RandomEngine) * 360.0f;
}

_MODULES_END
_NPGS_END
