module;

#include <boost/multiprecision/cpp_int.hpp>
#include "Engine/Core/Base.h"

export module Base.Civilization;

import Base.NpgsObject;

_NPGS_BEGIN

export class Civilization : public NpgsObject {
public:
    enum class LifePhase : int {
        kNull                  = 0,
        kLuca                  = 1,
        kGreatOxygenationEvent = 2,
        kMultiCellularLife     = 3,
        kCenoziocEra           = 4,
        kSatTeeTouy            = 5, // 被土著去城市化了
        kSatTeeTouyButByAsi    = 6, // 土著被 ASI 图图了
        kNewCivilization       = 7,
    };

    struct LifeProperties {
        boost::multiprecision::uint128_t TotalOrganismBiomass;         // 生物量，单位 kg
        float TotalOrganismUsedPower;                                  // 生物圈使用的总功率，单位 W
        LifePhase Phase;                                               // 生命阶段
    };

    struct CivilizationProperties {
        boost::multiprecision::uint128_t TotalAtrificalStructureMass;  // 文明造物总质量，单位 kg
        boost::multiprecision::uint128_t TotalCitizenBiomass;          // 文明生物生物量，单位 kg
        boost::multiprecision::uint128_t TotalUseableEnergeticNuclide; // 可用含能核素总质量，单位 kg
        float TotalCitizenUsedPower;                                   // 文明使用的总功率，单位 W
        float CivilizationProgress;                                    // 文明演化进度
        float GeneralIntelligenceAverageSynapseActivationCount;        // 通用智能个体的智力活动，单位 o/s
        float GeneralIntelligenceSynapseCount;                         // 通用智能个体的突触数
        float GeneralIntelligenceAverageLifetime;                      // 通用智能个体的平均寿命，单位 s
        float GeneralintelligenceCount;                                // 通用智能个体的数量
        float TotalStoragedHistoryDataSize;                            // 存储的历史数据总大小，单位 bit
        float TeamworkCoefficient;                                     // 合作系数
        bool  bIsIndependentIndividual;                                // 是否为独立个体
    };

public:
    explicit Civilization() = default;
    explicit Civilization(const LifeProperties& LifeProperties, const CivilizationProperties& CivilizationProperties);
    ~Civilization() = default;

    // Setters
    // Setters for LifeProperties
    // --------------------------
    Civilization& SetTotalOrganismBiomass(float TotalOrganismBiomass);
    Civilization& SetTotalOrganismBiomass(const boost::multiprecision::uint128_t& TotalOrganismBiomass);
    Civilization& SetTotalOrganismUsedPower(float TotalOrganismUsedPower);
    Civilization& SetLifePhase(LifePhase Phase);

    // Setters for CivilizationProperties
    // ----------------------------------
    Civilization& SetTotalAtrificalStructureMass(float TotalAtrificalStructureMass);
    Civilization& SetTotalAtrificalStructureMass(const boost::multiprecision::uint128_t& TotalAtrificalStructureMass);
    Civilization& SetTotalCitizenBiomass(float TotalCitizenBiomass);
    Civilization& SetTotalCitizenBiomass(const boost::multiprecision::uint128_t& TotalCitizenBiomass);
    Civilization& SetTotalUseableEnergeticNuclide(float TotalUseableEnergeticNuclide);
    Civilization& SetTotalUseableEnergeticNuclide(const boost::multiprecision::uint128_t& TotalUseableEnergeticNuclide);
    Civilization& SetTotalCitizenUsedPower(float TotalCitizenUsedPower);
    Civilization& SetCivilizationProgress(float CivilizationProgress);
    Civilization& SetGeneralIntelligenceAverageSynapseActivationCount(float GeneralIntelligenceAverageSynapseActivationCount);
    Civilization& SetGeneralIntelligenceSynapseCount(float GeneralIntelligenceSynapseCount);
    Civilization& SetGeneralIntelligenceAverageLifetime(float GeneralIntelligenceAverageLifetime);
    Civilization& SetGeneralintelligenceCount(float GeneralintelligenceCount);
    Civilization& SetTotalStoragedHistoryDataSize(float TotalStoragedHistoryDataSize);
    Civilization& SetTeamworkCoefficient(float TeamworkCoefficient);
    Civilization& SetIsIndependentIndividual(bool bIsIndependentIndividual);

    // Getters
    // Getters for LifeProperties
    // --------------------------
    float GetTotalOrganismBiomassFloat() const;
    const boost::multiprecision::uint128_t& GetTotalOrganismBiomass() const;
    float GetTotalOrganismUsedPower() const;
    LifePhase GetPhase() const;

    // Getters for CivilizationProperties
    // ----------------------------------
    float GetTotalAtrificalStructureMassFloat() const;
    const boost::multiprecision::uint128_t& GetTotalAtrificalStructureMass() const;
    float GetTotalCitizenBiomassFloat() const;
    const boost::multiprecision::uint128_t& GetTotalCitizenBiomass() const;
    float GetTotalUseableEnergeticNuclideFloat() const;
    const boost::multiprecision::uint128_t& GetTotalUseableEnergeticNuclide() const;
    float GetTotalCitizenUsedPower() const;
    float GetCivilizationProgress() const;
    float GetGeneralIntelligenceAverageSynapseActivationCount() const;
    float GetGeneralIntelligenceSynapseCount() const;
    float GetGeneralIntelligenceAverageLifetime() const;
    float GetGeneralintelligenceCount() const;
    float GetTotalStoragedHistoryDataSize() const;
    float GetTeamworkCoefficient() const;
    bool  IsIndependentIndividual() const;

public:
    static const float _kNull;
    static const float _kCarbonBasedGeneralIntelligence;
    static const float _kUrgesellschaft;
    static const float _kPrevIndustrielle;
    static const float _kSteamAge;
    static const float _kElectricAge;
    static const float _kAtomicAge;
    static const float _kDigitalAge;
    static const float _kPrevAsiAge;

private:
    CivilizationProperties _CivilizationProperties;
    LifeProperties         _LifeProperties;
};

_NPGS_END

#include "Civilization.inl"
