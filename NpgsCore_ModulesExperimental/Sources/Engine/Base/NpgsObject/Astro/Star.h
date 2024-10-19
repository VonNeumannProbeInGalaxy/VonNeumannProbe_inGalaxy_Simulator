#pragma once

#include <limits>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Engine/Base/NpgsObject/Astro/CelestialObject.h"
#include "Engine/Core/Modules/StellarClass.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_ASTRO_BEGIN

class NPGS_API Star : public CelestialBody {
public:
    enum class Phase : int {
        kPrevMainSequence          = -1,
        kMainSequence              =  0,
        kRedGiant                  =  2,
        kCoreHeBurn                =  3,
        kEarlyAgb                  =  4,
        kThermalPulseAgb           =  5,
        kPostAgb                   =  6,
        kWolfRayet                 =  9,
        kHeliumWhiteDwarf          =  11,
        kCarbonOxygenWhiteDwarf    =  12,
        kOxygenNeonMagnWhiteDwarf  =  13,
        kNeutronStar               =  14,
        kStellarBlackHole          =  15,
        kMiddleBlackHole           =  114514,
        kSuperMassiveBlackHole     =  1919810,
        kNull                      =  std::numeric_limits<int>::max()
    };

    enum class Death : int {
        kStillAlive                = 0,
        kWhiteDwarfMerge           = 1,
        kSlowColdingDown           = 2,
        kEnvelopeDisperse          = 3,
        kElectronCaptureSupernova  = 4,
        kIronCoreCollapseSupernova = 5,
        kRelativisticJetHypernova  = 6,
        kPairInstabilitySupernova  = 7,
        kPhotondisintegration      = 8,
    };

    struct ExtendedProperties {
        Modules::StellarClass StellarClass;

        double Mass;                    // 质量，单位 kg
        double Luminosity;              // 辐射光度，单位 W
        double Lifetime;                // 寿命，单位 yr
        double EvolutionProgress;       // 演化进度
        float  FeH;                     // 金属丰度
        float  InitialMass;             // 恒星诞生时的质量，单位 kg
        float  SurfaceH1;               // 表面氕质量分数
        float  SurfaceZ;                // 表面金属丰度
        float  SurfaceEnergeticNuclide; // 表面含能核素质量分数
        float  SurfaceVolatiles;        // 表面挥发物质量分数
        float  Teff;                    // 有效温度
        float  CoreTemp;                // 核心温度
        float  CoreDensity;             // 核心密度，单位 kg/m^3
        float  StellarWindSpeed;        // 恒星风速度，单位 m/s
        float  StellarWindMassLossRate; // 恒星风质量损失率，单位 kg/s
        float  MinCoilMass;             // 最小举星器赤道偏转线圈质量，单位 kg
        bool   bIsSingleStar;

        Phase EvolutionPhase;
        Death EvolutionEnding;
    };

public:
    Star() = default;
    Star(const CelestialBody::BasicProperties& StarBasicProperties, const ExtendedProperties& StarExtraProperties);
    ~Star() = default;

    Star& SetExtendedProperties(const ExtendedProperties& StarExtraProperties);
    const ExtendedProperties& GetExtendedProperties() const;

    // Setters
    // Setters for ExtendedProperties
    // ------------------------------
    Star& SetMass(double Mass);
    Star& SetLuminosity(double Luminosity);
    Star& SetLifetime(double Lifetime);
    Star& SetEvolutionProgress(double EvolutionProgress);
    Star& SetFeH(float FeH);
    Star& SetInitialMass(float InitialMass);
    Star& SetSurfaceH1(float SurfaceH1);
    Star& SetSurfaceZ(float SurfaceZ);
    Star& SetSurfaceEnergeticNuclide(float SurfaceEnergeticNuclide);
    Star& SetSurfaceVolatiles(float SurfaceVolatiles);
    Star& SetTeff(float Teff);
    Star& SetCoreTemp(float CoreTemp);
    Star& SetCoreDensity(float CoreDensity);
    Star& SetStellarWindSpeed(float StellarWindSpeed);
    Star& SetStellarWindMassLossRate(float StellarWindMassLossRate);
    Star& SetMinCoilMass(float MinCoilMass);
    Star& SetIsSingleStar(bool bIsSingleStar);
    Star& SetEvolutionPhase(Phase EvolutionPhase);
    Star& SetEvolutionEnding(Death EvolutionEnding);
    Star& SetStellarClass(const Modules::StellarClass& StellarClass);

    // Getters
    // Getters for ExtendedProperties
    // ------------------------------
    double GetMass() const;
    double GetLuminosity() const;
    double GetLifetime() const;
    double GetEvolutionProgress() const;
    float GetFeH() const;
    float GetInitialMass() const;
    float GetSurfaceH1() const;
    float GetSurfaceZ() const;
    float GetSurfaceEnergeticNuclide() const;
    float GetSurfaceVolatiles() const;
    float GetTeff() const;
    float GetCoreTemp() const;
    float GetCoreDensity() const;
    float GetStellarWindSpeed() const;
    float GetStellarWindMassLossRate() const;
    float GetMinCoilMass() const;
    bool  GetIsSingleStar() const;
    Phase GetEvolutionPhase() const;
    Death GetEvolutionEnding() const;
    const Modules::StellarClass& GetStellarClass() const;

    static const std::vector<std::pair<int, int>> _kSpectralSubclassMap_O;
    static const std::vector<std::pair<int, int>> _kSpectralSubclassMap_B;
    static const std::vector<std::pair<int, int>> _kSpectralSubclassMap_A;
    static const std::vector<std::pair<int, int>> _kSpectralSubclassMap_F;
    static const std::vector<std::pair<int, int>> _kSpectralSubclassMap_G;
    static const std::vector<std::pair<int, int>> _kSpectralSubclassMap_K;
    static const std::vector<std::pair<int, int>> _kSpectralSubclassMap_M;
    static const std::vector<std::pair<int, int>> _kSpectralSubclassMap_L;
    static const std::vector<std::pair<int, int>> _kSpectralSubclassMap_T;
    static const std::vector<std::pair<int, int>> _kSpectralSubclassMap_Y;
    static const std::vector<std::pair<int, int>> _kSpectralSubclassMap_WC;
    static const std::vector<std::pair<int, int>> _kSpectralSubclassMap_WN;
    static const std::vector<std::pair<int, int>> _kSpectralSubclassMap_WO;
    static const std::vector<std::pair<int, int>> _kSpectralSubclassMap_WNxh;
    static const std::vector<std::pair<int, std::vector<std::pair<int, int>>>> _kInitialCommonMap;
    static const std::vector<std::pair<int, std::vector<std::pair<int, int>>>> _kInitialWolfRayetMap;
    static const std::unordered_map<Phase, Npgs::Modules::StellarClass::LuminosityClass> _kLuminosityMap;
    static const std::unordered_map<float, float> _kFeHSurfaceH1Map;

private:
    ExtendedProperties _StarExtraProperties{};
};

_ASTRO_END
_NPGS_END

#include "Star.inl"
