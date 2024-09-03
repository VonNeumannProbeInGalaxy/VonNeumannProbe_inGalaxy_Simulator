#include "OrbitalGenerator.h"

#include <cmath>
#include <algorithm>
#include <iterator>
#include <utility>

#include "Engine/Core/Constants.h"

_NPGS_BEGIN
_MODULES_BEGIN

OrbitalGenerator::OrbitalGenerator(const std::seed_seq& SeedSeq)
    : _RandomEngine(SeedSeq), _CommonGenerator(0.0f, 1.0f)
{}

void OrbitalGenerator::GenerateOrbitals(StellarSystem& System) {}

void OrbitalGenerator::GeneratePlanets(StellarSystem& System) {
    // Single star
    PlanetaryDisk PlanetaryDiskTempData{};
    auto& Star = System.StarData().front();
    float DiskBase = 1.0f + _CommonGenerator.Generate(_RandomEngine);
    float StarInitialMassSol = Star.GetInitialMass() / static_cast<float>(kSolarMass);
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
        InterRadiusAu = std::sqrt(InterRadiusAu) / kAuInMeter;

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

    std::vector<float> CoreBase(PlanetCount, 0);
    for (float& Num : CoreBase) {
        Num = static_cast<float>(_CommonGenerator.Generate(_RandomEngine) * 3.0f);
    }

    float CoreBaseSum = std::accumulate(CoreBase.begin(), CoreBase.end(), 0.0f, [](float Sum, float Num) -> float {
        return Sum + std::pow(10.0f, Num);
    });

    AstroObject::Planet::MassProperties CoreMassKg;
    boost::multiprecision::int128_t InitialCoreMassSol;
    std::vector<AstroObject::Planet> Planets(PlanetCount);
    std::vector<float> CoreMassesSol(PlanetCount);
    for (std::size_t i = 0; i != PlanetCount; ++i) {
        CoreMassesSol[i] = PlanetaryDiskTempData.DustMassSol * std::pow(10.0f, CoreBase[i]) / CoreBaseSum;
        auto InitialCoreMassKg = boost::multiprecision::int128_t(kSolarMass * CoreMassesSol[i]);
        int VolatilesRate = 9000 + static_cast<int>(_CommonGenerator.Generate(_RandomEngine)) + 2000;
        int EnergeticNuclideRate = 4500000 + static_cast<int>(_CommonGenerator.Generate(_RandomEngine)) * 1000000;
        CoreMassKg.Volatiles = InitialCoreMassKg / VolatilesRate;
        CoreMassKg.EnergeticNuclide = InitialCoreMassKg / EnergeticNuclideRate;
        CoreMassKg.Z = InitialCoreMassKg - CoreMassKg.Volatiles - CoreMassKg.EnergeticNuclide;
        Planets[i].SetCoreMass(CoreMassKg);
        std::println("Initial CoreMass: {} Earth", CoreMassesSol[i] * kSolarMassToEarth);
    }

    std::vector<std::pair<StellarSystem::OrbitalElements, AstroObject::CelestialBody*>> Orbits(PlanetCount);
    for (auto& Orbit : Orbits) {
        Orbit.first.ParentBody = &System.StarData().front();
    }

    std::vector<float> DiskBoundaries(PlanetCount + 1);
    float CoreMassSum = std::accumulate(CoreMassesSol.begin(), CoreMassesSol.end(), 0.0f, [](float Sum, float Num) -> float {
        return Sum + std::pow(Num, 0.1f);
    });
    DiskBoundaries[0] = PlanetaryDiskTempData.InterRadiusAu;
    for (std::size_t i = 0; i != PlanetCount; ++i) {
        float PartCoreMassSum = std::accumulate(CoreMassesSol.begin(), CoreMassesSol.begin() + i + 1, 0.0f, [](float Sum, float Num) -> float {
            return Sum + std::pow(Num, 0.1f);
        });
        DiskBoundaries[i + 1] = PlanetaryDiskTempData.InterRadiusAu * std::pow(PlanetaryDiskTempData.OuterRadiusAu / PlanetaryDiskTempData.InterRadiusAu, PartCoreMassSum / CoreMassSum);
        float SemiMajorAxis = (DiskBoundaries[i] + DiskBoundaries[i + 1]) / 2.0f;
        Orbits[i].first.SemiMajorAxis = SemiMajorAxis;
        GenOrbitElements(Orbits[i].first);
    }

    for (auto& Orbit : Orbits) {
        std::println("Initial SemiMajorAxis: {} AU", Orbit.first.SemiMajorAxis);
    }

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

    std::vector<float> NewCoreMassesSol(PlanetCount);

    auto ErasePlanets = [&](float Limit) -> void {
        for (std::size_t i = 0; i < PlanetCount; ++i) {
            if (kAuInMeter * Orbits[0].first.SemiMajorAxis < Limit) {
                Planets.erase(Planets.begin());
                Orbits.erase(Orbits.begin());
                NewCoreMassesSol.erase(NewCoreMassesSol.begin());
                CoreMassesSol.erase(CoreMassesSol.begin());
                --PlanetCount;
            }
        }
    };

    auto GenerateOceanic = [&](std::size_t Index) -> void {
        float Random = _CommonGenerator.Generate(_RandomEngine) * 2.35f;
        float PlanetMassKg = static_cast<float>(CoreMassesSol[Index] * kSolarMass * Random);
    };

    auto GenerateIceGiant = [&](std::size_t Index) -> void {
        float Random = std::log10(2.35f) + _CommonGenerator.Generate(_RandomEngine) * (std::min(std::log10(30.0f), std::log10(1.0f / 0.0142f * std::pow(10.0f, Star.GetFeH())) - std::log10(2.35f)));
        float PlanetMassKg = static_cast<float>(CoreMassesSol[Index] * kSolarMass * Random);
    };

    if (Star.GetStellarClass().GetStarType() == Modules::StellarClass::StarType::kNormalStar) {
        float FrostLineAu = 0.0f;
        float CommonFactor = (std::pow(10.0f, 2.0f - StarInitialMassSol) + 1.0f) * (static_cast<float>(kSolarLuminosity) / (4.0f * kPi * kStefanBoltzmann * std::pow(270.0f, 4.0f)));
        if (StarInitialMassSol >= 0.075f && StarInitialMassSol < 0.43f) {
            FrostLineAu = CommonFactor * (0.23f * std::pow(StarInitialMassSol, 2.3f));
        } else if (StarInitialMassSol >= 0.43f && StarInitialMassSol < 2.0f) {
            FrostLineAu = CommonFactor * std::pow(StarInitialMassSol, 4.0f);
        } else if (StarInitialMassSol >= 2.0f && StarInitialMassSol <= 12.0f) {
            FrostLineAu = CommonFactor * (1.5f * std::pow(StarInitialMassSol, 3.5f));
        }
        FrostLineAu = std::sqrt(FrostLineAu) / kAuInMeter;

        std::println("FrostLine: {} AU", FrostLineAu);

        float InterChzRadiusAu = 0.0f;
        float OuterChzRadiusAu = 0.0f;
        float StarLuminosity = static_cast<float>(Star.GetLuminosity());
        InterChzRadiusAu = sqrt(StarLuminosity / (4.0f * kPi * 3000)) / kAuInMeter;
        OuterChzRadiusAu = sqrt(StarLuminosity / (4.0f * kPi * 600))  / kAuInMeter;

        for (std::size_t i = 0; i != PlanetCount; ++i) {
            NewCoreMassesSol[i] = CoreMassesSol[i] * 2.35f;
            
            float PrevMainSequenceBalanceTemp = 0.0f;
            CommonFactor = (std::pow(10.0f, 2.0f - StarInitialMassSol) + 1.0f) * (static_cast<float>(kSolarLuminosity) / (4.0f * kPi * kStefanBoltzmann * std::pow(Orbits[i].first.SemiMajorAxis * kAuInMeter, 2.0f)));
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
                if (Orbits[i].first.SemiMajorAxis > FrostLineAu) {
                    Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kRockyIceAsteroidCluster);
                } else {
                    Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kRockyAsteroidCluster);
                }
            } else {
                if (Orbits[i].first.SemiMajorAxis > FrostLineAu) {
                    Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kIcePlanet);
                    Planets[i].SetRadius(kEarthRadius * 1.3f * std::pow(NewCoreMassesSol[i] * kSolarMassToEarth, 1.0f / 3.905f));

                    if ((NewCoreMassesSol[i] * kSolarMass / Planets[i].GetRadius()) > (CommonFactor / 2.0f)) {
                        Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kGasGiant);
                    } else if ((NewCoreMassesSol[i] * kSolarMass / Planets[i].GetRadius()) > (CommonFactor / 4.0f)) {
                        Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kIceGiant);
                    }
                } else {
                    Planets[i].SetRadius(kEarthRadius * 1.007f * std::pow(CoreMassesSol[i] * kSolarMassToEarth, 1.0f / 3.7f));

                    if ((CoreMassesSol[i] * kSolarMass / Planets[i].GetRadius()) > (CommonFactor / 12.0f)) {
                        if (Orbits[i].first.SemiMajorAxis > InterChzRadiusAu && Orbits[i].first.SemiMajorAxis < OuterChzRadiusAu) {
                            Planets[i].SetPlanetType(AstroObject::Planet::PlanetType::kOceanic);
                        }
                    }
                }
            }
        }

        std::println("Habitability zone: {} - {} AU", InterChzRadiusAu, OuterChzRadiusAu);

        for (auto& NewCoreMass : NewCoreMassesSol) {
            std::println("NewCoreMass: {} Earth", NewCoreMass * kSolarMassToEarth);
        }

        for (auto& Planet : Planets) {
            std::println("Planet radius: {}", Planet.GetRadius());
            std::println("Planet type: {}", static_cast<int>(Planet.GetPlanetType()));
        }

        BernoulliDistribution MigrationProbability(0.1);
        for (std::size_t i = 1; i != PlanetCount; ++i) {
            if (Planets[i].GetPlanetType() == AstroObject::Planet::PlanetType::kIceGiant ||
                Planets[i].GetPlanetType() == AstroObject::Planet::PlanetType::kGasGiant) {
                if (MigrationProbability.Generate(_RandomEngine)) {
                    BernoulliDistribution WalkInProbability(0.8);
                    int MigrationIndex = 0;
                    if (WalkInProbability.Generate(_RandomEngine)) {
                        MigrationIndex = static_cast<int>(_CommonGenerator.Generate(_RandomEngine) * (i - 1));
                    } else {
                        float Lower = std::log10(PlanetaryDiskTempData.InterRadiusAu / 3.0f);
                        float Upper = std::log10(PlanetaryDiskTempData.InterRadiusAu * 0.67f);
                        float Exponent = Lower + _CommonGenerator.Generate(_RandomEngine) * (Upper - Lower);
                        Orbits[0].first.SemiMajorAxis = std::pow(10.0f, Exponent);
                    }

                    Planets[MigrationIndex] = Planets[i];
                    Planets.erase(Planets.begin() + MigrationIndex + 1, Planets.begin() + i + 1);
                    Orbits.erase(Orbits.begin() + MigrationIndex + 1, Orbits.begin() + i + 1);
                    NewCoreMassesSol.erase(NewCoreMassesSol.begin() + MigrationIndex + 1, NewCoreMassesSol.begin() + i + 1);
                    CoreMassesSol.erase(CoreMassesSol.begin() + MigrationIndex + 1, CoreMassesSol.begin() + i + 1);

                    PlanetCount = Planets.size();
                    break;
                }
            }
        }

        for (auto& Orbit : Orbits) {
            std::println("After migration SemiMajorAxis: {} AU", Orbit.first.SemiMajorAxis);
        }

        if (static_cast<int>(Star.GetEvolutionPhase()) <= 1) {
            ErasePlanets(Star.GetRadius());
        } else {
            float StarRadiusMaxSol = 0.0f;
            if (StarInitialMassSol < 0.75f) {
                StarRadiusMaxSol = 104 * std::pow(2.0f * StarInitialMassSol, 3.0f) + 0.1f;
            } else {
                StarRadiusMaxSol = 400 * std::pow(StarInitialMassSol - 0.75f, 1.0f / 3.0f);
            }

            StarRadiusMaxSol *= kSolarRadius;
            ErasePlanets(StarRadiusMaxSol);
        }

        for (auto& Orbit : Orbits) {
            std::println("After erase SemiMajorAxis: {} AU", Orbit.first.SemiMajorAxis);
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
                if ((Star.GetLuminosity() / (4.0f * kPi * std::pow(Orbits[i].first.SemiMajorAxis * kAuInMeter, 2.0f))) >= 10000) {
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
                        Orbits[i].first.SemiMajorAxis = StarRadiusSol * Random * kSolarRadius / kAuInMeter;
                        break;
                    }
                }
            }
        }

        for (auto& Planet : Planets) {
            std::println("After erase and migration: {}", static_cast<int>(Planet.GetPlanetType()));
        }
    } else {
        for (std::size_t i = 0; i != PlanetCount; ++i) {
            Planets[i].SetRadius(kEarthRadius * 1.007f * std::pow(CoreMassesSol[i] * kSolarMassToEarth, 1.0f / 3.7f));
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
