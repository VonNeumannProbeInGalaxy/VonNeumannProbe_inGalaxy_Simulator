#pragma once

#include <cstdint>
#include <boost/multiprecision/cpp_int.hpp>
#include "Engine/Base/NpgsObject.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_INTELLI_BEGIN

class NPGS_API Standard : public NpgsObject
{
public:
	enum class LifePhase : int
	{
		kNull                  = 0,
		kLuca                  = 1,
		kGreatOxygenationEvent = 2,
		kMultiCellularLife     = 3,
		kCenoziocEra           = 4,
		kSatTeeTouy            = 5, // 被土著去城市化了
		kSatTeeTouyButByAsi    = 6, // 土著被 Artifical Stupid Idiot 图图了
		kNewCivilization       = 7,
	};

	struct LifeProperties
	{
		boost::multiprecision::uint128_t OrganismBiomass; // 生物量，单位 kg
		float OrganismUsedPower{};                        // 生物圈使用的总功率，单位 W
		LifePhase Phase{ LifePhase::kNull };              // 生命阶段
	};

	struct CivilizationProperties
	{
		boost::multiprecision::uint128_t AtrificalStructureMass;   // 文明造物总质量，单位 kg
		boost::multiprecision::uint128_t CitizenBiomass;           // 文明生物生物量，单位 kg
		boost::multiprecision::uint128_t UseableEnergeticNuclide;  // 可用含能核素总质量，单位 kg
		boost::multiprecision::uint128_t OrbitAssetsMass;          // 轨道资产总质量，单位 kg
		std::uint64_t GeneralintelligenceCount{};                  // 通用智能个体的数量
		float GeneralIntelligenceAverageSynapseActivationCount{};  // 通用智能个体的智力活动，单位 o/s
		float GeneralIntelligenceSynapseCount{};                   // 通用智能个体的突触数
		float GeneralIntelligenceAverageLifetime{};                // 通用智能个体的平均寿命，单位 yr
		float CitizenUsedPower{};                                  // 文明使用的总功率，单位 W
		float CivilizationProgress{};                              // 文明演化进度
		float StoragedHistoryDataSize{};                           // 存储的历史数据总大小，单位 bit
		float TeamworkCoefficient{};                               // 合作系数
		bool  bIsIndependentIndividual{ false };                   // 是否为独立个体
	};

public:
	Standard() = default;
	Standard(const LifeProperties& LifeProperties, const CivilizationProperties& CivilizationProperties);
	~Standard() = default;

	// Setters
	// Setters for LifeProperties
	// --------------------------
	Standard& SetOrganismBiomass(float OrganismBiomass);
	Standard& SetOrganismBiomass(const boost::multiprecision::uint128_t& OrganismBiomass);
	Standard& SetOrganismUsedPower(float OrganismUsedPower);
	Standard& SetLifePhase(LifePhase Phase);

	// Setters for CivilizationProperties
	// ----------------------------------
	Standard& SetAtrificalStructureMass(float AtrificalStructureMass);
	Standard& SetAtrificalStructureMass(const boost::multiprecision::uint128_t& AtrificalStructureMass);
	Standard& SetCitizenBiomass(float CitizenBiomass);
	Standard& SetCitizenBiomass(const boost::multiprecision::uint128_t& CitizenBiomass);
	Standard& SetUseableEnergeticNuclide(float UseableEnergeticNuclide);
	Standard& SetUseableEnergeticNuclide(const boost::multiprecision::uint128_t& UseableEnergeticNuclide);
	Standard& SetOrbitAssetsMass(float OrbitAssetsMass);
	Standard& SetOrbitAssetsMass(const boost::multiprecision::uint128_t& OrbitAssetsMass);
	Standard& SetGeneralintelligenceCount(std::uint64_t GeneralintelligenceCount);
	Standard& SetGeneralIntelligenceAverageSynapseActivationCount(float GeneralIntelligenceAverageSynapseActivationCount);
	Standard& SetGeneralIntelligenceSynapseCount(float GeneralIntelligenceSynapseCount);
	Standard& SetGeneralIntelligenceAverageLifetime(float GeneralIntelligenceAverageLifetime);
	Standard& SetCitizenUsedPower(float CitizenUsedPower);
	Standard& SetCivilizationProgress(float CivilizationProgress);
	Standard& SetStoragedHistoryDataSize(float StoragedHistoryDataSize);
	Standard& SetTeamworkCoefficient(float TeamworkCoefficient);
	Standard& SetIsIndependentIndividual(bool bIsIndependentIndividual);

	// Getters
	// Getters for LifeProperties
	// --------------------------
	const boost::multiprecision::uint128_t& GetOrganismBiomass() const;
	float GetOrganismUsedPower() const;
	LifePhase GetLifePhase() const;

	template <typename T>
	T GetOrganismBiomassDigital() const;

	// Getters for CivilizationProperties
	// ----------------------------------
	const boost::multiprecision::uint128_t& GetAtrificalStructureMass() const;
	const boost::multiprecision::uint128_t& GetCitizenBiomass() const;
	const boost::multiprecision::uint128_t& GetUseableEnergeticNuclide() const;
	const boost::multiprecision::uint128_t& GetOrbitAssetsMass() const;
	std::uint64_t GetGeneralintelligenceCount() const;
	float GetGeneralIntelligenceAverageSynapseActivationCount() const;
	float GetGeneralIntelligenceSynapseCount() const;
	float GetGeneralIntelligenceAverageLifetime() const;
	float GetCitizenUsedPower() const;
	float GetCivilizationProgress() const;
	float GetStoragedHistoryDataSize() const;
	float GetTeamworkCoefficient() const;
	bool  IsIndependentIndividual() const;

	template <typename T>
	T GetAtrificalStructureMassDigital() const;

	template <typename T>
	T GetCitizenBiomassDigital() const;

	template <typename T>
	T GetUseableEnergeticNuclideDigital() const;

	template <typename T>
	T GetOrbitAssetsMassDigital() const;

public:
	static const float _kNull;
	static const float _kInitialGeneralIntelligence;
	static const float _kUrgesellschaft;
	static const float _kEarlyIndustrielle;
	static const float _kSteamAge;
	static const float _kElectricAge;
	static const float _kAtomicAge;
	static const float _kDigitalAge;
	static const float _kEarlyAsiAge;

private:
	CivilizationProperties _CivilizationProperties;
	LifeProperties         _LifeProperties;
};

class Advanced : public NpgsObject
{
};

_INTELLI_END
_NPGS_END

#include "Civilization.inl"
