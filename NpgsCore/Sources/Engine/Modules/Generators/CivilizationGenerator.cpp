#include "CivilizationGenerator.h"

#include "Engine/Base/Civilization.h"
#include "Engine/Core/Constants.h"

_NPGS_BEGIN
_MODULE_BEGIN

CivilizationGenerator::CivilizationGenerator(const std::seed_seq& SeedSequence,
											 float LifeOccurrenceProbability,
											 bool  bEnableAsiFilter,
											 float DestroyedByDisasterProbability)
	:
	_RandomEngine(SeedSequence),
	_AsiFiltedProbability(static_cast<double>(bEnableAsiFilter) * 0.2),
	_DestroyedByDisasterProbability(DestroyedByDisasterProbability),
	_LifeOccurrenceProbability(LifeOccurrenceProbability),
	_CommonGenerator(0.0f, 1.0f)
{
}

void CivilizationGenerator::GenerateCivilization(const Astro::Star* Star, float PoyntingVector, Astro::Planet* Planet)
{
	if (Star->GetAge() < 2.4e9 || !_LifeOccurrenceProbability(_RandomEngine))
	{
		return;
	}

	Planet->CivilizationData() = std::make_unique<Civilization>();

	GenerateLife(Star->GetAge(), PoyntingVector, Planet);
	GenerateCivilizationDetails(Star, PoyntingVector, Planet);
}

void CivilizationGenerator::GenerateLife(double StarAge, float PoyntingVector, Astro::Planet* Planet)
{
	// 计算生命演化阶段
	float Random = 0.5f + _CommonGenerator(_RandomEngine) + 1.5f;
	auto  LifePhase = static_cast<Civilization::LifePhase>(std::min(4, std::max(1, static_cast<int>(Random * StarAge / (5e8)))));
	auto* CivilizationData = Planet->CivilizationData().get();

	// 处理生命成矿机制以及 ASI 大过滤器
	if (LifePhase == Civilization::LifePhase::kCenoziocEra)
	{
		Random = 1.0f + _CommonGenerator(_RandomEngine) * 999.0f;
		if (_AsiFiltedProbability(_RandomEngine))
		{
			LifePhase = Civilization::LifePhase::kSatTeeTouyButByAsi; // 被 ASI 去城市化了
			Planet->SetCrustMineralMass(Random * 1e16f + Planet->GetCrustMineralMassDigital<float>());
		}
		else
		{
			if (_DestroyedByDisasterProbability(_RandomEngine))
			{
				LifePhase = Civilization::LifePhase::kSatTeeTouyButByAsi; // 被天灾去城市化或者自己玩死了
			}
			else
			{
				Planet->SetCrustMineralMass(Random * 1e15f + Planet->GetCrustMineralMassDigital<float>());
			}
		}
	}

	CivilizationData->SetLifePhase(LifePhase);

	// 计算生物量和总功率
	double Scale             = (PoyntingVector / kSolarConstantOfEarth) * (Planet->GetRadius() / kEarthRadius);
	float  Exponent          = 0.0f;
	float  CommonRandom      = 0.01f + _CommonGenerator(_RandomEngine) * 0.04f;
	double OrganismBiomass   = 0.0;
	double OrganismUsedPower = 0.0;

	switch (LifePhase)
	{
	case Civilization::LifePhase::kLuca:
		Random            = _CommonGenerator(_RandomEngine);
		OrganismBiomass   = Random * 1e11 * Scale;
		OrganismUsedPower = CommonRandom * 0.1 * OrganismBiomass;
		break;
	case Civilization::LifePhase::kGreatOxygenationEvent:
		Exponent          = -1.0f + _CommonGenerator(_RandomEngine) * 2.0f;
		Random            = std::pow(10.0f, Exponent);
		OrganismBiomass   = Random * 1e12 * Scale;
		OrganismUsedPower = CommonRandom * 0.1 * OrganismBiomass;
		break;
	case Civilization::LifePhase::kMultiCellularLife:
		Exponent          = _CommonGenerator(_RandomEngine) * 2.0f;
		Random            = std::pow(10.0f, Exponent);
		OrganismBiomass   = Random * 1e13 * Scale;
		OrganismUsedPower = CommonRandom * 0.01 * OrganismBiomass;
		break;
	case Civilization::LifePhase::kCenoziocEra:
	case Civilization::LifePhase::kSatTeeTouyButByAsi:
		Exponent          = -1.0f + _CommonGenerator(_RandomEngine) * 2.0f;
		Random            = std::pow(10.0f, Exponent);
		OrganismBiomass   = Random * 1e16 * Scale;
		OrganismUsedPower = CommonRandom * 0.001 * OrganismBiomass;
		break;
	case Civilization::LifePhase::kSatTeeTouy:
	case Civilization::LifePhase::kNewCivilization:
		Random            = _CommonGenerator(_RandomEngine);
		OrganismBiomass   = Random * 1e15 * Scale;
		OrganismUsedPower = CommonRandom * 0.001 * OrganismBiomass;
		break;
	default:
		break;
	}

	if (Planet->GetSpin() > 10 * kDayToSecond) {
		CommonRandom = 0.01f + _CommonGenerator(_RandomEngine) * 0.04f;
		OrganismBiomass   *= CommonRandom;
		OrganismUsedPower *= CommonRandom;
	}

	CivilizationData->SetOrganismBiomass(boost::multiprecision::uint128_t(OrganismBiomass));
	CivilizationData->SetOrganismUsedPower(static_cast<float>(OrganismUsedPower));
}

void CivilizationGenerator::GenerateCivilizationDetails(const Astro::Star* Star, float PoyntingVector, Astro::Planet* Planet)
{
    const std::array<float, 7>* ProbabilityListPtr = nullptr;
    auto& CivilizationData = Planet->CivilizationData();
    CivilizationData = std::make_unique<Civilization>();

    auto LifePhase = Planet->CivilizationData()->GetLifePhase();
    int IntegerPart = 0;
    float FractionalPart = 0.0f;
    if (LifePhase != Civilization::LifePhase::kCenoziocEra &&
        LifePhase != Civilization::LifePhase::kSatTeeTouy &&
        LifePhase != Civilization::LifePhase::kSatTeeTouyButByAsi)
    {
        CivilizationData->SetCivilizationProgress(0.0f);
    }
    else if (LifePhase == Civilization::LifePhase::kCenoziocEra)
    {
        ProbabilityListPtr = &_kProbabilityListForCenoziocEra;
    }
    else if (LifePhase == Civilization::LifePhase::kSatTeeTouyButByAsi)
    {
        ProbabilityListPtr = &_kProbabilityListForSatTeeTouyButAsi;
    }

    if (ProbabilityListPtr != nullptr)
    {
        float Random = _CommonGenerator(_RandomEngine);
        float CumulativeProbability = 0.0f;

        for (int i = 0; i < 7; ++i)
        {
            CumulativeProbability += (*ProbabilityListPtr)[i];
            if (Random < CumulativeProbability)
            {
                IntegerPart = i + 1;
                break;
            }
        }

        if (IntegerPart >= 7)
        {
            if (LifePhase == Civilization::LifePhase::kCenoziocEra)
            {
                LifePhase = Civilization::LifePhase::kSatTeeTouy;
            }
            else if (LifePhase == Civilization::LifePhase::kSatTeeTouyButByAsi)
            {
                LifePhase = Civilization::LifePhase::kNewCivilization;
            }
        }

        FractionalPart = _CommonGenerator(_RandomEngine);
        CivilizationData->SetCivilizationProgress(static_cast<float>(IntegerPart) + FractionalPart);
    }

    CivilizationData->SetLifePhase(LifePhase);
    CivilizationData->SetIsIndependentIndividual(true);

    auto GenerateRandom1 = [this]() -> float
    {
        float Exponent = -1.0f + _CommonGenerator(_RandomEngine) * 2.0f;
        float Random1  = std::pow(10.0f, Exponent);
        return Random1;
    };

	auto GenerateRandom2 = [this]() -> float
	{
		float Random2 = 0.9f + _CommonGenerator(_RandomEngine) * 0.2f;
		return Random2;
	};

    float Random1 = GenerateRandom1();
    float Random2 = GenerateRandom2();

    // 文明生物的总生物量（CitizenBiomass）
    boost::multiprecision::uint128_t CitizenBiomass;
    if (IntegerPart >= 7.0f && IntegerPart <= 8.0f)
    {
        double Base        = Random1 * 4e11;
        double Coefficient = std::pow(100.0, FractionalPart);
        double Result      = Base * Coefficient;
        float  Random      = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        Result            *= Random;
        CitizenBiomass     = boost::multiprecision::uint128_t(Result);
    }
    else if (IntegerPart >= 6.0f && IntegerPart < 7.0f)
    {
        double Base        = Random1 * 1.15e11;
        double Coefficient = std::pow(3.47826, FractionalPart);
        double Result      = Base * Coefficient;
        float  Random      = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        Result            *= Random;
        CitizenBiomass     = boost::multiprecision::uint128_t(Result);
    }
    else if (IntegerPart >= 5.0f && IntegerPart < 6.0f)
    {
        double Base        = Random1 * 5e10;
        double Coefficient = std::pow(2.3, FractionalPart);
        double Result      = Base * Coefficient;
        float  Random      = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        Result            *= Random;
        CitizenBiomass     = boost::multiprecision::uint128_t(Result);
    }
    else if (IntegerPart >= 4.0f && IntegerPart < 5.0f)
    {
        double Base        = Random1 * 3e10;
        double Coefficient = std::pow(1.66666, FractionalPart);
        double Result      = Base * Coefficient;
        float  Random      = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        Result            *= Random;
        CitizenBiomass     = boost::multiprecision::uint128_t(Result);
    }
    else if (IntegerPart >= 3.0f && IntegerPart < 4.0f)
    {
        double Base        = Random1 * 3e8;
        double Coefficient = std::pow(100.0, FractionalPart);
        double Result      = Base * Coefficient;
        float  Random      = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        Result            *= Random;
        CitizenBiomass     = boost::multiprecision::uint128_t(Result);
    }
    else if (IntegerPart >= 2.0f && IntegerPart < 3.0f)
    {
        double Base        = Random1 * 5e7;
        double Coefficient = std::pow(6.0, FractionalPart);
        double Result      = Base * Coefficient;
        float  Random      = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        Result            *= Random;
        CitizenBiomass     = boost::multiprecision::uint128_t(Result);
    }
    else if (IntegerPart >= 1.0f && IntegerPart < 2.0f)
    {
        double Base        = Random1 * 5e6;
        double Coefficient = std::pow(10.0, FractionalPart);
        double Result      = Base * Coefficient;
        float  Random      = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        Result            *= Random;
        CitizenBiomass     = boost::multiprecision::uint128_t(Result);
    }
    else
    {
        CitizenBiomass     = 0;
    }
    CivilizationData->SetCitizenBiomass(CitizenBiomass);

    // 文明造物总质量（AtrificalStructureMass）
    boost::multiprecision::uint128_t AtrificalStructureMass;
    if (IntegerPart >= 7.0f && IntegerPart <= 8.0f)
    {
        double Base            = Random1 * 1e15;
        double Coefficient     = std::pow(1000.0, FractionalPart);
        double Result          = Base * Coefficient;
        float  Random          = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        Result                *= Random;
        AtrificalStructureMass = boost::multiprecision::uint128_t(Result);
    }
    else if (IntegerPart >= 6.0f && IntegerPart < 7.0f)
    {
        double Base            = Random1 * 6.25e13;
        double Coefficient     = std::pow(16.0, FractionalPart);
        double Result          = Base * Coefficient;
        float  Random          = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        Result                *= Random;
        AtrificalStructureMass = boost::multiprecision::uint128_t(Result);
    }
    else if (IntegerPart >= 5.0f && IntegerPart < 6.0f)
    {
        double Base            = Random1 * 1.5e13;
        double Coefficient     = std::pow(4.16666, FractionalPart);
        double Result          = Base * Coefficient;
        float  Random          = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        Result                *= Random;
        AtrificalStructureMass = boost::multiprecision::uint128_t(Result);
    }
    else if (IntegerPart >= 4.0f && IntegerPart < 5.0f)
    {
        double Base            = Random1 * 6e12;
        double Coefficient     = std::pow(2.5, FractionalPart);
        double Result          = Base * Coefficient;
        float  Random          = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        Result                *= Random;
        AtrificalStructureMass = boost::multiprecision::uint128_t(Result);
    }
    else if (IntegerPart >= 3.0f && IntegerPart < 4.0f)
    {
        double Base            = Random1 * 6e9;
        double Coefficient     = std::pow(1000.0, FractionalPart);
        double Result          = Base * Coefficient;
        float  Random          = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        Result                *= Random;
        AtrificalStructureMass = boost::multiprecision::uint128_t(Result);
    }
    else
    {
        AtrificalStructureMass = 0;
    }
    CivilizationData->SetAtrificalStructureMass(AtrificalStructureMass);

    // 文明总功率（CitizenUsedPower）
    float CitizenUsedPower = 0.0;
    if (IntegerPart >= 7.0f && IntegerPart <= 8.0f)
    {
        float Base          = Random1 * 1e13f;
        float Coefficient   = std::pow(1000.0f, FractionalPart);
        float Result        = Base * Coefficient;
        float Pcivilization = std::max(Result, 0.1f * PoyntingVector * kPi * std::pow(Planet->GetRadius(), 2.0f));
        float Random        = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        CitizenUsedPower    = Pcivilization * Random;
    }
    else if (IntegerPart >= 6.0f && IntegerPart < 7.0f)
    {
        float Base          = Random1 * 4e12f;
        float Coefficient   = std::pow(5.0f, FractionalPart);
        float Result        = Base * Coefficient;
        float Random        = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        CitizenUsedPower    = Result * Random;
    }
    else if (IntegerPart >= 5.0f && IntegerPart < 6.0f)
    {
        float Base          = Random1 * 2.5e11f;
        float Coefficient   = std::pow(16.0f, FractionalPart);
        float Result        = Base * Coefficient;
        float Random        = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        CitizenUsedPower    = Result * Random;
    }
    else if (IntegerPart >= 4.0f && IntegerPart < 5.0f)
    {
        float Base          = Random1 * 6e10f;
        float Coefficient   = std::pow(4.16666f, FractionalPart);
        float Result        = Base * Coefficient;
        float Random        = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        CitizenUsedPower    = Result * Random;
    }
    else if (IntegerPart >= 3.0f && IntegerPart < 4.0f)
    {
        float Base          = Random1 * 6e8f;
        float Coefficient   = std::pow(100.0f, FractionalPart);
        float Result        = Base * Coefficient;
        float Random        = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        CitizenUsedPower    = Result * Random;
    }
    else if (IntegerPart >= 2.0f && IntegerPart < 3.0f)
    {
        float Base          = Random1 * 1e8f;
        float Coefficient   = std::pow(6.0f, FractionalPart);
        float Result        = Base * Coefficient;
        float Random        = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        CitizenUsedPower    = Result * Random;
    }
    else if (IntegerPart >= 1.0f && IntegerPart < 2.0f)
    {
        float Base          = Random1 * 1e7f;
        float Coefficient   = std::pow(10.0f, FractionalPart);
        float Result        = Base * Coefficient;
        float Random        = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        CitizenUsedPower    = Result * Random;
    }
    else
    {
        CitizenUsedPower    = 0.0;
    }
    CivilizationData->SetCitizenUsedPower(static_cast<float>(CitizenUsedPower));

    // 存储的历史总信息量（StoragedHistoryDataSize）
    float StoragedHistoryDataSize = 0.0;
    if (IntegerPart >= 7.2f && IntegerPart <= 8.0f)
    {
        float Base              = Random1 * 2.5e25f;
        float Coefficient       = std::pow(10.0f, FractionalPart / 0.2f);
        float Result            = Base * Coefficient;
        float Random            = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        StoragedHistoryDataSize = Result * Random;
    }
    else if (IntegerPart >= 7.0f && IntegerPart < 7.2f)
    {
        float Base              = Random1 * 1e22f;
        float Coefficient       = std::pow(50.0f, FractionalPart / 0.1f);
        float Result            = Base * Coefficient;
        float Random            = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        StoragedHistoryDataSize = Result * Random;
    }
    else if (IntegerPart >= 6.0f && IntegerPart < 7.0f)
    {
        float Base              = Random1 * 1e20f;
        float Coefficient       = std::pow(100.0f, FractionalPart);
        float Result            = Base * Coefficient;
        float Random            = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        StoragedHistoryDataSize = Result * Random;
    }
    else if (IntegerPart >= 5.0f && IntegerPart < 6.0f)
    {
        float Base              = Random1 * 5e17f;
        float Coefficient       = std::pow(200.0f, FractionalPart);
        float Result            = Base * Coefficient;
        float Random            = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        StoragedHistoryDataSize = Result * Random;
    }
    else if (IntegerPart >= 4.0f && IntegerPart < 5.0f)
    {
        float Base              = Random1 * 1e15f;
        float Coefficient       = std::pow(500.0f, FractionalPart);
        float Result            = Base * Coefficient;
        float Random            = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        StoragedHistoryDataSize = Result * Random;
    }
    else if (IntegerPart >= 3.0f && IntegerPart < 4.0f)
    {
        float Base              = Random1 * 1e12f;
        float Coefficient       = std::pow(1000.0f, FractionalPart);
        float Result            = Base * Coefficient;
        float Random            = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        StoragedHistoryDataSize = Result * Random;
    }
    else if (IntegerPart >= 2.0f && IntegerPart < 3.0f)
    {
        float Base              = Random1 * 1e10f;
        float Coefficient       = std::pow(100.0f, FractionalPart);
        float Result            = Base * Coefficient;
        float Random            = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        StoragedHistoryDataSize = Result * Random;
    }
    else
    {
        StoragedHistoryDataSize = 0.0;
    }
    CivilizationData->SetStoragedHistoryDataSize(static_cast<float>(StoragedHistoryDataSize));

    // 通用智能个体平均体重（AverageWeight）
    float AverageWeight = Random1 * Random2 * 1e4f;

    // 通用智能个体平均突触数量（GeneralIntelligenceSynapseCount）
    float AverageSynapses = AverageWeight * std::sqrt(GenerateRandom1()) * 2e13f;
    CivilizationData->SetGeneralIntelligenceSynapseCount(AverageSynapses);

    // 通用智能个体平均算力（GeneralIntelligenceAverageSynapseActivationCount）
    float AverageCompute = AverageSynapses * 50.0f * GenerateRandom1();
    CivilizationData->SetGeneralIntelligenceAverageSynapseActivationCount(AverageCompute);

    // 个体平均寿命（GeneralIntelligenceAverageLifetime）
    float AverageLifetime = std::sqrt(GenerateRandom1()) * 1.6e-13f * (AverageSynapses * AverageSynapses) / AverageCompute;
    CivilizationData->SetGeneralIntelligenceAverageLifetime(AverageLifetime);

    // 合作系数（TeamworkCoefficient）
    float TeamworkCoefficient = std::sqrt(GenerateRandom1());
    CivilizationData->SetTeamworkCoefficient(TeamworkCoefficient);

    // 可用含能核素（UseableEnergeticNuclide）
    boost::multiprecision::uint128_t UseableEnergeticNuclide;
    if (IntegerPart >= 6.0f && IntegerPart <= 8.0f)
    {
        double StarAge = Star->GetAge();
        double Base = Random1 * FractionalPart * 1e9 * 0.63 * std::pow(0.5, StarAge / (8e8));

        if (IntegerPart >= 7.0f)
        {
            double Coefficient = std::pow(1e4, FractionalPart);
            Base *= Coefficient;
        }

        float Random = 0.9f + 0.2f * _CommonGenerator(_RandomEngine);
        Base *= Random;

        UseableEnergeticNuclide = static_cast<boost::multiprecision::uint128_t>(Base);
        CivilizationData->SetUseableEnergeticNuclide(UseableEnergeticNuclide);
    }

    // 最大入轨能力（LaunchCapability）
    double LaunchCapability = 0.0;
    if (IntegerPart >= 7.0f && IntegerPart <= 8.0f)
    {
        double PlanetMass   = Planet->GetMassDigital<double>();
        double PlanetRadius = Planet->GetRadius();
        double Base         = 5e-6;

        LaunchCapability = Base * std::pow(kEarthMass / PlanetMass, 3) * std::pow(PlanetRadius / kEarthRadius, 3) *
            CitizenUsedPower / std::sqrt(Random1);
    }

    // 轨道资产总质量
    double OrbitAssetsMass = 0.0;
    if (LaunchCapability > 0.0)
    {
        OrbitAssetsMass = GenerateRandom1() * LaunchCapability * (IntegerPart - 6.0f) / TeamworkCoefficient;
    }
}

const std::array<float, 7> CivilizationGenerator::_kProbabilityListForCenoziocEra{
	0.02f, 0.005f, 1e-4f, 1e-6f, 5e-7f, 4e-7f, 1e-6f
};

const std::array<float, 7> CivilizationGenerator::_kProbabilityListForSatTeeTouyButAsi{
	0.2f, 0.05f, 0.001f, 1e-5f, 1e-4f, 1e-4f, 1e-4f
};

_MODULE_END
_NPGS_END
