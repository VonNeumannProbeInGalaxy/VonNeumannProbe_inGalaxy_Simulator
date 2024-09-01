#include "OrbitalGenerator.h"

#include <cmath>
#include <algorithm>

#include "Engine/Core/Constants.h"

_NPGS_BEGIN
_MODULES_BEGIN

OrbitalGenerator::OrbitalGenerator(const std::seed_seq& SeedSeq)
    : _RandomEngine(SeedSeq), _CommonGenerator(0.0f, 1.0f), _PlanetaryDiskTempData({})
{}

void OrbitalGenerator::GenerateOrbitals(StellarSystem& System) {}

void OrbitalGenerator::GeneratePlanetaryDisk(StellarSystem& System) {
    // Single star
    auto& Star = System.StarData().front();
    float Base = 1.0f + _CommonGenerator.Generate(_RandomEngine);
    if (Star.GetStellarClass().GetStarType() == StellarClass::StarType::kNormalStar) {
        float InitialMass   = Star.GetInitialMass();
        float LuminositySol = Star.GetLuminosity() / static_cast<float>(kSolarLuminosity);
        float DiskMass = Base * InitialMass * std::pow(10.0f, -2.05f + 0.1214f * InitialMass - 0.02669f * std::pow(InitialMass, 2.0f) - 0.2274f * std::log(InitialMass));
        float DustMass = DiskMass * 0.0142f * 0.4f * std::pow(10.0f, Star.GetFeH());
        float OuterRadius  = 30.0f * InitialMass;
        float InterRadius  = 0.0f;
        float CommonFactor = (std::pow(10.0f, 2.0f - InitialMass) + 1.0f) * (LuminositySol / (4.0f * kPi * kStefanBoltzmann * std::pow(1700.0f, 4.0f)));

        if (InitialMass >= 0.075f && InitialMass < 0.43f) {
            InterRadius = CommonFactor * (0.23f * std::pow(InitialMass, 2.3f));
        } else if (InitialMass >= 0.43f && InitialMass < 2.0f) {
            InterRadius = CommonFactor * std::pow(InitialMass, 4.0f);
        } else if (InitialMass >= 2.0f && InitialMass <= 12.0f) {
            InterRadius = CommonFactor * (1.5f * std::pow(InitialMass, 3.5f));
        }
        InterRadius = std::sqrt(InterRadius) / kAuInMeter;

        _PlanetaryDiskTempData.InterRadius = InterRadius;
        _PlanetaryDiskTempData.OuterRadius = OuterRadius;
        _PlanetaryDiskTempData.DiskMass = DiskMass;
        _PlanetaryDiskTempData.DustMass = DustMass;
    } else if (Star.GetEvolutionEnding() == AstroObject::Star::Death::kWhiteDwarfMerge) {
        float StarMass = Star.GetMass() / static_cast<float>(kSolarMass);
        float DiskMass = Base * 1e-5f * StarMass;
        _PlanetaryDiskTempData.InterRadius = 0.01f;
        _PlanetaryDiskTempData.OuterRadius = 1.0f;
        _PlanetaryDiskTempData.DiskMass = DiskMass;
        _PlanetaryDiskTempData.DustMass = DiskMass;
    } else {
        _PlanetaryDiskTempData = {};
    }
}

void OrbitalGenerator::GeneratePlanetaryCore(StellarSystem& System) {
    auto& Star = System.StarData().front();
    int PlanetCount = 0;
    if (Star.GetStellarClass().GetStarType() == StellarClass::StarType::kNormalStar) {
        PlanetCount = static_cast<int>(6.0f + _CommonGenerator.Generate(_RandomEngine) * 6.0f);
    } else if (Star.GetEvolutionEnding() == AstroObject::Star::Death::kWhiteDwarfMerge) {
        PlanetCount = static_cast<int>(2.0f + _CommonGenerator.Generate(_RandomEngine) * 2.0f);
    } else {
        return;
    }

    std::vector<int> Base(PlanetCount, 0);
    for (int& Num : Base) {
        Num = static_cast<float>(_CommonGenerator.Generate(_RandomEngine) * 2.0f);
    }

    int AccBase = std::accumulate(Base.begin(), Base.end(), 0, [](int Acc, int Num) -> int {
        return Acc + std::pow(10, Num);
    });

    std::vector<AstroObject::Planet> Planets(PlanetCount);
    for (int i = 0; i != PlanetCount; ++i) {

    }
}

_MODULES_END
_NPGS_END
