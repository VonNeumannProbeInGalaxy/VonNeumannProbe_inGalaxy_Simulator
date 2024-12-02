#pragma once

#include "Civilization.h"

_NPGS_BEGIN

inline Civilization& Civilization::SetOrganismBiomass(float OrganismBiomass)
{
	_LifeProperties.OrganismBiomass = boost::multiprecision::uint128_t(OrganismBiomass);
	return *this;
}

inline Civilization& Civilization::SetOrganismBiomass(const boost::multiprecision::uint128_t& OrganismBiomass)
{
	_LifeProperties.OrganismBiomass = OrganismBiomass;
	return *this;
}

inline Civilization& Civilization::SetOrganismUsedPower(float OrganismUsedPower)
{
	_LifeProperties.OrganismUsedPower = OrganismUsedPower;
	return *this;
}

inline Civilization& Civilization::SetLifePhase(LifePhase Phase)
{
	_LifeProperties.Phase = Phase;
	return *this;
}

inline Civilization& Civilization::SetAtrificalStructureMass(float AtrificalStructureMass)
{
	_CivilizationProperties.AtrificalStructureMass = boost::multiprecision::uint128_t(AtrificalStructureMass);
	return *this;
}

inline Civilization& Civilization::SetAtrificalStructureMass(const boost::multiprecision::uint128_t& AtrificalStructureMass)
{
	_CivilizationProperties.AtrificalStructureMass = AtrificalStructureMass;
	return *this;
}

inline Civilization& Civilization::SetCitizenBiomass(float CitizenBiomass)
{
	_CivilizationProperties.AtrificalStructureMass = boost::multiprecision::uint128_t(CitizenBiomass);
	return *this;
}

inline Civilization& Civilization::SetCitizenBiomass(const boost::multiprecision::uint128_t& CitizenBiomass)
{
	_CivilizationProperties.AtrificalStructureMass = CitizenBiomass;
	return *this;
}

inline Civilization& Civilization::SetUseableEnergeticNuclide(float UseableEnergeticNuclide)
{
	_CivilizationProperties.AtrificalStructureMass = boost::multiprecision::uint128_t(UseableEnergeticNuclide);
	return *this;
}

inline Civilization& Civilization::SetUseableEnergeticNuclide(const boost::multiprecision::uint128_t& UseableEnergeticNuclide)
{
	_CivilizationProperties.AtrificalStructureMass = UseableEnergeticNuclide;
	return *this;
}

inline Civilization& Civilization::SetCitizenUsedPower(float CitizenUsedPower)
{
	_CivilizationProperties.CitizenUsedPower = CitizenUsedPower;
	return *this;
}

inline Civilization& Civilization::SetCivilizationProgress(float CivilizationProgress)
{
	_CivilizationProperties.CivilizationProgress = CivilizationProgress;
	return *this;
}

inline Civilization& Civilization::SetGeneralIntelligenceAverageSynapseActivationCount(float GeneralIntelligenceAverageSynapseActivationCount)
{
	_CivilizationProperties.GeneralIntelligenceAverageSynapseActivationCount = GeneralIntelligenceAverageSynapseActivationCount;
	return *this;
}

inline Civilization& Civilization::SetGeneralIntelligenceSynapseCount(float GeneralIntelligenceSynapseCount)
{
	_CivilizationProperties.GeneralIntelligenceSynapseCount = GeneralIntelligenceSynapseCount;
	return *this;
}

inline Civilization& Civilization::SetGeneralIntelligenceAverageLifetime(float GeneralIntelligenceAverageLifetime)
{
	_CivilizationProperties.GeneralIntelligenceAverageLifetime = GeneralIntelligenceAverageLifetime;
	return *this;
}

inline Civilization& Civilization::SetGeneralintelligenceCount(float GeneralintelligenceCount)
{
	_CivilizationProperties.GeneralIntelligenceSynapseCount = GeneralintelligenceCount;
	return *this;
}

inline Civilization& Civilization::SetStoragedHistoryDataSize(float StoragedHistoryDataSize)
{
	_CivilizationProperties.StoragedHistoryDataSize = StoragedHistoryDataSize;
	return *this;
}

inline Civilization& Civilization::SetTeamworkCoefficient(float TeamworkCoefficient)
{
	_CivilizationProperties.TeamworkCoefficient = TeamworkCoefficient;
	return *this;
}

inline Civilization& Civilization::SetIsIndependentIndividual(bool bIsIndependentIndividual)
{
	_CivilizationProperties.bIsIndependentIndividual = bIsIndependentIndividual;
	return *this;
}

inline const boost::multiprecision::uint128_t& Civilization::GetOrganismBiomass() const
{
	return _LifeProperties.OrganismBiomass;
}

inline float Civilization::GetOrganismUsedPower() const
{
	return _LifeProperties.OrganismUsedPower;
}

inline Civilization::LifePhase Civilization::GetLifePhase() const
{
	return _LifeProperties.Phase;
}

template <typename T>
T Civilization::GetOrganismBiomassDigital() const
{
	return _LifeProperties.OrganismBiomass.convert_to<T>();
}

inline const boost::multiprecision::uint128_t& Civilization::GetAtrificalStructureMass() const
{
	return _CivilizationProperties.AtrificalStructureMass;
}

inline const boost::multiprecision::uint128_t& Civilization::GetCitizenBiomass() const
{
	return _CivilizationProperties.CitizenBiomass;
}

inline const boost::multiprecision::uint128_t& Civilization::GetUseableEnergeticNuclide() const
{
	return _CivilizationProperties.UseableEnergeticNuclide;
}

inline float Civilization::GetCitizenUsedPower() const
{
	return _CivilizationProperties.CitizenUsedPower;
}

inline float Civilization::GetCivilizationProgress() const
{
	return _CivilizationProperties.CivilizationProgress;
}

inline float Civilization::GetGeneralIntelligenceAverageSynapseActivationCount() const
{
	return _CivilizationProperties.GeneralIntelligenceAverageSynapseActivationCount;
}

inline float Civilization::GetGeneralIntelligenceSynapseCount() const
{
	return _CivilizationProperties.GeneralIntelligenceSynapseCount;
}

inline float Civilization::GetGeneralIntelligenceAverageLifetime() const
{
	return _CivilizationProperties.GeneralIntelligenceAverageLifetime;
}

inline float Civilization::GetGeneralintelligenceCount() const
{
	return _CivilizationProperties.GeneralintelligenceCount;
}

inline float Civilization::GetStoragedHistoryDataSize() const
{
	return _CivilizationProperties.StoragedHistoryDataSize;
}

inline float Civilization::GetTeamworkCoefficient() const
{
	return _CivilizationProperties.TeamworkCoefficient;
}

inline bool Civilization::IsIndependentIndividual() const
{
	return _CivilizationProperties.bIsIndependentIndividual;
}

template <typename T>
T Civilization::GetAtrificalStructureMassDigital() const
{
	return _CivilizationProperties.AtrificalStructureMass.convert_to<T>();
}

template <typename T>
T Civilization::GetCitizenBiomassDigital() const
{
	return _CivilizationProperties.CitizenBiomass.convert_to<T>();
}

template <typename T>
T Civilization::GetUseableEnergeticNuclideDigital() const
{
	return _CivilizationProperties.UseableEnergeticNuclide.convert_to<T>();
}

_NPGS_END
