#include "Civilization.h"

_NPGS_BEGIN

inline static float ConvertToFloat(const boost::multiprecision::uint128_t& MultiPrecision) {
    return MultiPrecision.convert_to<float>();
}

inline Civilization& Civilization::SetTotalOrganismBiomass(float TotalOrganismBiomass) {
    _LifeProperties.TotalOrganismBiomass = boost::multiprecision::uint128_t(TotalOrganismBiomass);
    return *this;
}

inline Civilization& Civilization::SetTotalOrganismBiomass(const boost::multiprecision::uint128_t& TotalOrganismBiomass) {
    _LifeProperties.TotalOrganismBiomass = TotalOrganismBiomass;
    return *this;
}

inline Civilization& Civilization::SetTotalOrganismUsedPower(float TotalOrganismUsedPower) {
    _LifeProperties.TotalOrganismUsedPower = TotalOrganismUsedPower;
    return *this;
}

inline Civilization& Civilization::SetLifePhase(LifePhase Phase) {
    _LifeProperties.Phase = Phase;
    return *this;
}

inline Civilization& Civilization::SetTotalAtrificalStructureMass(float TotalAtrificalStructureMass) {
    _CivilizationProperties.TotalAtrificalStructureMass = boost::multiprecision::uint128_t(TotalAtrificalStructureMass);
    return *this;
}

inline Civilization& Civilization::SetTotalAtrificalStructureMass(const boost::multiprecision::uint128_t& TotalAtrificalStructureMass) {
    _CivilizationProperties.TotalAtrificalStructureMass = TotalAtrificalStructureMass;
    return *this;
}

inline Civilization& Civilization::SetTotalCitizenBiomass(float TotalCitizenBiomass) {
    _CivilizationProperties.TotalAtrificalStructureMass = boost::multiprecision::uint128_t(TotalCitizenBiomass);
    return *this;
}

inline Civilization& Civilization::SetTotalCitizenBiomass(const boost::multiprecision::uint128_t& TotalCitizenBiomass) {
    _CivilizationProperties.TotalAtrificalStructureMass = TotalCitizenBiomass;
    return *this;
}

inline Civilization& Civilization::SetTotalUseableEnergeticNuclide(float TotalUseableEnergeticNuclide) {
    _CivilizationProperties.TotalAtrificalStructureMass = boost::multiprecision::uint128_t(TotalUseableEnergeticNuclide);
    return *this;
}

inline Civilization& Civilization::SetTotalUseableEnergeticNuclide(const boost::multiprecision::uint128_t& TotalUseableEnergeticNuclide) {
    _CivilizationProperties.TotalAtrificalStructureMass = TotalUseableEnergeticNuclide;
    return *this;
}

inline Civilization& Civilization::SetTotalCitizenUsedPower(float TotalCitizenUsedPower) {
    _CivilizationProperties.TotalCitizenUsedPower = TotalCitizenUsedPower;
    return *this;
}

inline Civilization& Civilization::SetCivilizationProgress(float CivilizationProgress) {
    _CivilizationProperties.CivilizationProgress = CivilizationProgress;
    return *this;
}

inline Civilization& Civilization::SetGeneralIntelligenceAverageSynapseActivationCount(float GeneralIntelligenceAverageSynapseActivationCount) {
    _CivilizationProperties.GeneralIntelligenceAverageSynapseActivationCount = GeneralIntelligenceAverageSynapseActivationCount;
    return *this;
}

inline Civilization& Civilization::SetGeneralIntelligenceSynapseCount(float GeneralIntelligenceSynapseCount) {
    _CivilizationProperties.GeneralIntelligenceSynapseCount = GeneralIntelligenceSynapseCount;
    return *this;
}

inline Civilization& Civilization::SetGeneralIntelligenceAverageLifetime(float GeneralIntelligenceAverageLifetime) {
    _CivilizationProperties.GeneralIntelligenceAverageLifetime = GeneralIntelligenceAverageLifetime;
    return *this;
}

inline Civilization& Civilization::SetGeneralintelligenceCount(float GeneralintelligenceCount) {
    _CivilizationProperties.GeneralIntelligenceSynapseCount = GeneralintelligenceCount;
    return *this;
}

inline Civilization& Civilization::SetTotalStoragedHistoryDataSize(float TotalStoragedHistoryDataSize) {
    _CivilizationProperties.TotalStoragedHistoryDataSize = TotalStoragedHistoryDataSize;
    return *this;
}

inline Civilization& Civilization::SetTeamworkCoefficient(float TeamworkCoefficient) {
    _CivilizationProperties.TeamworkCoefficient = TeamworkCoefficient;
    return *this;
}

inline Civilization& Civilization::SetIsIndependentIndividual(bool bIsIndependentIndividual) {
    _CivilizationProperties.bIsIndependentIndividual = bIsIndependentIndividual;
    return *this;
}

inline float Civilization::GetTotalOrganismBiomassFloat() const {
    return ConvertToFloat(GetTotalOrganismBiomass());
}

inline const boost::multiprecision::uint128_t& Civilization::GetTotalOrganismBiomass() const {
    return _LifeProperties.TotalOrganismBiomass;
}

inline float Civilization::GetTotalOrganismUsedPower() const {
    return _LifeProperties.TotalOrganismUsedPower;
}

inline Civilization::LifePhase Civilization::GetPhase() const {
    return _LifeProperties.Phase;
}

inline float Civilization::GetTotalAtrificalStructureMassFloat() const {
    return ConvertToFloat(GetTotalAtrificalStructureMass());
}

inline const boost::multiprecision::uint128_t& Civilization::GetTotalAtrificalStructureMass() const {
    return _CivilizationProperties.TotalAtrificalStructureMass;
}

inline float Civilization::GetTotalCitizenBiomassFloat() const {
    return ConvertToFloat(GetTotalCitizenBiomass());
}

inline const boost::multiprecision::uint128_t& Civilization::GetTotalCitizenBiomass() const {
    return _CivilizationProperties.TotalCitizenBiomass;
}

inline float Civilization::GetTotalUseableEnergeticNuclideFloat() const {
    return ConvertToFloat(GetTotalUseableEnergeticNuclide());
}

inline const boost::multiprecision::uint128_t& Civilization::GetTotalUseableEnergeticNuclide() const {
    return _CivilizationProperties.TotalUseableEnergeticNuclide;
}

inline float Civilization::GetTotalCitizenUsedPower() const {
    return _CivilizationProperties.TotalCitizenUsedPower;
}

inline float Civilization::GetCivilizationProgress() const {
    return _CivilizationProperties.CivilizationProgress;
}

inline float Civilization::GetGeneralIntelligenceAverageSynapseActivationCount() const {
    return _CivilizationProperties.GeneralIntelligenceAverageSynapseActivationCount;
}

inline float Civilization::GetGeneralIntelligenceSynapseCount() const {
    return _CivilizationProperties.GeneralIntelligenceSynapseCount;
}

inline float Civilization::GetGeneralIntelligenceAverageLifetime() const {
    return _CivilizationProperties.GeneralIntelligenceAverageLifetime;
}

inline float Civilization::GetGeneralintelligenceCount() const {
    return _CivilizationProperties.GeneralintelligenceCount;
}

inline float Civilization::GetTotalStoragedHistoryDataSize() const {
    return _CivilizationProperties.TotalStoragedHistoryDataSize;
}

inline float Civilization::GetTeamworkCoefficient() const {
    return _CivilizationProperties.TeamworkCoefficient;
}

inline bool Civilization::IsIndependentIndividual() const {
    return _CivilizationProperties.bIsIndependentIndividual;
}

_NPGS_END
