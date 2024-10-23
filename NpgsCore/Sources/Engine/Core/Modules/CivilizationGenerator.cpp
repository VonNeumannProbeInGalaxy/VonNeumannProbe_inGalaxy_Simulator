#include "CivilizationGenerator.h"

#include <memory>

#include "Engine/Base/NpgsObject/Civilization.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_MODULE_BEGIN

CivilizationGenerator::CivilizationGenerator(const std::seed_seq& SeedSequence, float LifeOccurrenceProbability, bool bEnableAsiFilter)
    :
    _RandomEngine(SeedSequence),
    _AsiFiltedProbability(static_cast<double>(bEnableAsiFilter) * 0.2),
    _LifeOccurrenceProbability(LifeOccurrenceProbability),
    _CommonGenerator(0.0f, 1.0f)
{}

void CivilizationGenerator::GenerateCivilization(double StarAge, float PoyntingVector, float PlanetRadius, float PlanetMass, std::unique_ptr<Astro::Planet>& Planet) {
    if (_LifeOccurrenceProbability(_RandomEngine)) {
        float Random1 = 0.5f + _CommonGenerator(_RandomEngine) + 1.5f;
        auto LifePhase = static_cast<Civilization::LifePhase>(std::min(4, std::max(1, static_cast<int>(Random1 * StarAge / (5 * std::pow(10, 8))))));

        if (LifePhase == Civilization::LifePhase::kCenoziocEra) {
            float Random2 = 1.0f + _CommonGenerator(_RandomEngine) * 999.0f;
            if (_AsiFiltedProbability(_RandomEngine)) {
                LifePhase = Civilization::LifePhase::kSatTeeTouyButByAsi; // 被 ASI 去城市化了
                Planet->SetCrustMineralMass(Random2 * 1e16f + Planet->GetCrustMineralMassFloat());
            } else {
                Planet->SetCrustMineralMass(Random2 * 1e15f + Planet->GetCrustMineralMassFloat());
            }
        }

        const std::array<float, 7>* ProbabilityListPtr = nullptr;
        auto& CivilizationData = Planet->CivilizationData();
        CivilizationData = std::make_unique<Civilization>();

        int IntegerPart = 0;
        if (LifePhase != Civilization::LifePhase::kCenoziocEra &&
            LifePhase != Civilization::LifePhase::kSatTeeTouy &&
            LifePhase != Civilization::LifePhase::kSatTeeTouyButByAsi) {
            CivilizationData->SetCivilizationProgress(0.0f);
        } else if (LifePhase == Civilization::LifePhase::kCenoziocEra) {
            ProbabilityListPtr = &_kProbabilityListForCenoziocEra;
        } else if (LifePhase == Civilization::LifePhase::kSatTeeTouyButByAsi) {
            ProbabilityListPtr = &_kProbabilityListForSatTeeTouyButAsi;
        }

        if (ProbabilityListPtr != nullptr) {
            float Random = _CommonGenerator(_RandomEngine);
            float CumulativeProbability = 0.0f;

            for (int i = 0; i < 7; ++i) {
                CumulativeProbability += (*ProbabilityListPtr)[i];
                if (Random < CumulativeProbability) {
                    IntegerPart = i + 1;
                    break;
                }
            }

            if (IntegerPart >= 7) {
                if (LifePhase == Civilization::LifePhase::kCenoziocEra) {
                    LifePhase = Civilization::LifePhase::kSatTeeTouy;
                } else if (LifePhase == Civilization::LifePhase::kSatTeeTouyButByAsi) {
                    LifePhase = Civilization::LifePhase::kNewCivilization;
                }
            }

            float FractionalPart = _CommonGenerator(_RandomEngine);
            CivilizationData->SetCivilizationProgress(static_cast<float>(IntegerPart) + FractionalPart);
        }

        CivilizationData->SetLifePhase(LifePhase);
    }
}

const std::array<float, 7> CivilizationGenerator::_kProbabilityListForCenoziocEra{ 0.02f, 0.005f, 1e-4f, 1e-6f, 5e-7f, 4e-7f, 1e-6f };
const std::array<float, 7> CivilizationGenerator::_kProbabilityListForSatTeeTouyButAsi{ 0.2f, 0.05f, 0.001f, 1e-5f, 1e-4f, 1e-4f, 1e-4f };

_MODULE_END
_NPGS_END

