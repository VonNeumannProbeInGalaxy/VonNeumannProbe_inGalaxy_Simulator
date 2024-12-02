#pragma once

#include "Civilization.h"

_NPGS_BEGIN
_INTELLI_BEGIN

inline Standard& Standard::SetOrganismBiomass(float OrganismBiomass)
{
	_LifeProperties.OrganismBiomass = boost::multiprecision::uint128_t(OrganismBiomass);
	return *this;
}

inline Standard& Standard::SetOrganismBiomass(const boost::multiprecision::uint128_t& OrganismBiomass)
{
	_LifeProperties.OrganismBiomass = OrganismBiomass;
	return *this;
}

inline Standard& Standard::SetOrganismUsedPower(float OrganismUsedPower)
{
	_LifeProperties.OrganismUsedPower = OrganismUsedPower;
	return *this;
}

inline Standard& Standard::SetLifePhase(LifePhase Phase)
{
	_LifeProperties.Phase = Phase;
	return *this;
}

inline Standard& Standard::SetAtrificalStructureMass(float AtrificalStructureMass)
{
	_CivilizationProperties.AtrificalStructureMass = boost::multiprecision::uint128_t(AtrificalStructureMass);
	return *this;
}

inline Standard& Standard::SetAtrificalStructureMass(const boost::multiprecision::uint128_t& AtrificalStructureMass)
{
	_CivilizationProperties.AtrificalStructureMass = AtrificalStructureMass;
	return *this;
}

inline Standard& Standard::SetCitizenBiomass(float CitizenBiomass)
{
	_CivilizationProperties.CitizenBiomass = boost::multiprecision::uint128_t(CitizenBiomass);
	return *this;
}

inline Standard& Standard::SetCitizenBiomass(const boost::multiprecision::uint128_t& CitizenBiomass)
{
	_CivilizationProperties.CitizenBiomass = CitizenBiomass;
	return *this;
}

inline Standard& Standard::SetUseableEnergeticNuclide(float UseableEnergeticNuclide)
{
	_CivilizationProperties.UseableEnergeticNuclide = boost::multiprecision::uint128_t(UseableEnergeticNuclide);
	return *this;
}

inline Standard& Standard::SetUseableEnergeticNuclide(const boost::multiprecision::uint128_t& UseableEnergeticNuclide)
{
	_CivilizationProperties.UseableEnergeticNuclide = UseableEnergeticNuclide;
	return *this;
}

inline Standard& Standard::SetOrbitAssetsMass(float OrbitAssetsMass)
{
	_CivilizationProperties.OrbitAssetsMass = boost::multiprecision::uint128_t(OrbitAssetsMass);
	return *this;
}

inline Standard& Standard::SetOrbitAssetsMass(const boost::multiprecision::uint128_t& OrbitAssetsMass)
{
	_CivilizationProperties.OrbitAssetsMass = OrbitAssetsMass;
	return *this;
}

inline Standard& Standard::SetGeneralintelligenceCount(std::uint64_t GeneralintelligenceCount)
{
	_CivilizationProperties.GeneralintelligenceCount = GeneralintelligenceCount;
	return *this;
}

inline Standard& Standard::SetGeneralIntelligenceAverageSynapseActivationCount(float GeneralIntelligenceAverageSynapseActivationCount)
{
	_CivilizationProperties.GeneralIntelligenceAverageSynapseActivationCount = GeneralIntelligenceAverageSynapseActivationCount;
	return *this;
}

inline Standard& Standard::SetGeneralIntelligenceSynapseCount(float GeneralIntelligenceSynapseCount)
{
	_CivilizationProperties.GeneralIntelligenceSynapseCount = GeneralIntelligenceSynapseCount;
	return *this;
}

inline Standard& Standard::SetGeneralIntelligenceAverageLifetime(float GeneralIntelligenceAverageLifetime)
{
	_CivilizationProperties.GeneralIntelligenceAverageLifetime = GeneralIntelligenceAverageLifetime;
	return *this;
}

inline Standard& Standard::SetCitizenUsedPower(float CitizenUsedPower)
{
	_CivilizationProperties.CitizenUsedPower = CitizenUsedPower;
	return *this;
}

inline Standard& Standard::SetCivilizationProgress(float CivilizationProgress)
{
	_CivilizationProperties.CivilizationProgress = CivilizationProgress;
	return *this;
}

inline Standard& Standard::SetStoragedHistoryDataSize(float StoragedHistoryDataSize)
{
	_CivilizationProperties.StoragedHistoryDataSize = StoragedHistoryDataSize;
	return *this;
}

inline Standard& Standard::SetTeamworkCoefficient(float TeamworkCoefficient)
{
	_CivilizationProperties.TeamworkCoefficient = TeamworkCoefficient;
	return *this;
}

inline Standard& Standard::SetIsIndependentIndividual(bool bIsIndependentIndividual)
{
	_CivilizationProperties.bIsIndependentIndividual = bIsIndependentIndividual;
	return *this;
}

inline const boost::multiprecision::uint128_t& Standard::GetOrganismBiomass() const
{
	return _LifeProperties.OrganismBiomass;
}

inline float Standard::GetOrganismUsedPower() const
{
	return _LifeProperties.OrganismUsedPower;
}

inline Standard::LifePhase Standard::GetLifePhase() const
{
	return _LifeProperties.Phase;
}

template <typename T>
T Standard::GetOrganismBiomassDigital() const
{
	return _LifeProperties.OrganismBiomass.convert_to<T>();
}

inline const boost::multiprecision::uint128_t& Standard::GetAtrificalStructureMass() const
{
	return _CivilizationProperties.AtrificalStructureMass;
}

inline const boost::multiprecision::uint128_t& Standard::GetCitizenBiomass() const
{
	return _CivilizationProperties.CitizenBiomass;
}

inline const boost::multiprecision::uint128_t& Standard::GetUseableEnergeticNuclide() const
{
	return _CivilizationProperties.UseableEnergeticNuclide;
}

inline const boost::multiprecision::uint128_t& Standard::GetOrbitAssetsMass() const
{
	return _CivilizationProperties.OrbitAssetsMass;
}

inline std::uint64_t Standard::GetGeneralintelligenceCount() const
{
	return _CivilizationProperties.GeneralintelligenceCount;
}

inline float Standard::GetGeneralIntelligenceAverageSynapseActivationCount() const
{
	return _CivilizationProperties.GeneralIntelligenceAverageSynapseActivationCount;
}

inline float Standard::GetGeneralIntelligenceSynapseCount() const
{
	return _CivilizationProperties.GeneralIntelligenceSynapseCount;
}

inline float Standard::GetGeneralIntelligenceAverageLifetime() const
{
	return _CivilizationProperties.GeneralIntelligenceAverageLifetime;
}

inline float Standard::GetCitizenUsedPower() const
{
	return _CivilizationProperties.CitizenUsedPower;
}

inline float Standard::GetCivilizationProgress() const
{
	return _CivilizationProperties.CivilizationProgress;
}

inline float Standard::GetStoragedHistoryDataSize() const
{
	return _CivilizationProperties.StoragedHistoryDataSize;
}

inline float Standard::GetTeamworkCoefficient() const
{
	return _CivilizationProperties.TeamworkCoefficient;
}

inline bool Standard::IsIndependentIndividual() const
{
	return _CivilizationProperties.bIsIndependentIndividual;
}

template <typename T>
T Standard::GetAtrificalStructureMassDigital() const
{
	return _CivilizationProperties.AtrificalStructureMass.convert_to<T>();
}

template <typename T>
T Standard::GetCitizenBiomassDigital() const
{
	return _CivilizationProperties.CitizenBiomass.convert_to<T>();
}

template <typename T>
T Standard::GetUseableEnergeticNuclideDigital() const
{
	return _CivilizationProperties.UseableEnergeticNuclide.convert_to<T>();
}

template<typename T>
inline T Standard::GetOrbitAssetsMassDigital() const
{
	return _CivilizationProperties.OrbitAssetsMass.convert_to<T>();
}

_INTELLI_END
_NPGS_END
