#include "CivilizationGenerator.h"

#include "Engine/Base/Civilization.h"

_NPGS_BEGIN
_MODULE_BEGIN

CivilizationGenerator::CivilizationGenerator(
	const std::seed_seq& SeedSequence,
	float LifeOccurrenceProbability,
	bool  bEnableAsiFilter,
	float DestroyedByDisasterProbability
)
	:
	_RandomEngine(SeedSequence),
	_AsiFiltedProbability(static_cast<double>(bEnableAsiFilter) * 0.2),
	_DestroyedByDisasterProbability(DestroyedByDisasterProbability),
	_LifeOccurrenceProbability(LifeOccurrenceProbability),
	_CommonGenerator(0.0f, 1.0f)
{}

void CivilizationGenerator::GenerateCivilization(double StarAge, float PoyntingVector, Astro::Planet* Planet) {
	if (StarAge < 2.4e9 || !_LifeOccurrenceProbability(_RandomEngine)) {
		return;
	}

	// 计算生命演化阶段
	float Random1 = 0.5f + _CommonGenerator(_RandomEngine) + 1.5f;
	auto LifePhase = static_cast<Civilization::LifePhase>(std::min(4, std::max(1, static_cast<int>(Random1 * StarAge / (5e8)))));

	// 处理生命成矿机制以及 ASI 大过滤器
	if (LifePhase == Civilization::LifePhase::kCenoziocEra) {
		float Random2 = 1.0f + _CommonGenerator(_RandomEngine) * 999.0f;
		if (_AsiFiltedProbability(_RandomEngine)) {
			LifePhase = Civilization::LifePhase::kSatTeeTouyButByAsi; // 被 ASI 去城市化了
			Planet->SetCrustMineralMass(Random2 * 1e16f + Planet->GetCrustMineralMassFloat());
		} else {
			if (_DestroyedByDisasterProbability(_RandomEngine)) {
				LifePhase = Civilization::LifePhase::kSatTeeTouyButByAsi; // 被小行星城市化或者自己玩死了
			} else {
				Planet->SetCrustMineralMass(Random2 * 1e15f + Planet->GetCrustMineralMassFloat());
			}
		}
	}

	const std::array<float, 7>* ProbabilityListPtr = nullptr;
	auto& CivilizationData = Planet->CivilizationData();
	CivilizationData = std::make_unique<Civilization>();

	int IntegerPart = 0;
	if (LifePhase != Civilization::LifePhase::kCenoziocEra &&
		LifePhase != Civilization::LifePhase::kSatTeeTouy  &&
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

const std::array<float, 7> CivilizationGenerator::_kProbabilityListForCenoziocEra{
	0.02f, 0.005f, 1e-4f, 1e-6f, 5e-7f, 4e-7f, 1e-6f
};

const std::array<float, 7> CivilizationGenerator::_kProbabilityListForSatTeeTouyButAsi{
	0.2f, 0.05f, 0.001f, 1e-5f, 1e-4f, 1e-4f, 1e-4f
};

_MODULE_END
_NPGS_END

