#pragma once

#include <limits>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Engine/Base/AstroObject/CelestialObject.h"
#include "Engine/Core/Modules/Stellar/StellarClass.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_ASTROOBJECT_BEGIN

class NPGS_API Star : public CelestialBody {
public:
    enum class Phase : int {
        kPrevMainSequence         = -1,
        kMainSequence             =  0,
        kRedGiant                 =  2,
        kCoreHeBurn               =  3,
        kEarlyAgb                 =  4,
        kThermalPulseAgb          =  5,
        kPostAgb                  =  6,
        kWolfRayet                =  9,
        kHeliumWhiteDwarf         =  11,
        kCarbonOxygenWhiteDwarf   =  12,
        kOxygenNeonMagnWhiteDwarf =  13,
        kNeutronStar              =  14,
        kStellarBlackHole         =  15,
        kMiddleBlackHole          =  114514,
        kSuperMassiveBlackHole    =  1919810,
        kNull                     =  std::numeric_limits<int>::max()
    };

    enum class Death {
        kStillAlive,
        kWhiteDwarfMerge,
        kSlowColdingDown,
        kEnvelopeDisperse,
        kElectronCaptureSupernova,
        kIronCoreCollapseSupernova,
        kRelativisticJetHypernova,
        kPairInstabilitySupernova,
        kPhotondisintegration,
    };

    struct ExtendedProperties {
        Modules::StellarClass StellarClass;

        glm::vec2 Normal;               // 法向量，球坐标表示，(theta, phi)

        double Mass;                    // 质量，单位 kg
        double Luminosity;              // 辐射光度，单位 W
        float  FeH;                     // 金属丰度
        float  SurfaceH1;               // 表面氕质量分数
        float  SurfaceZ;                // 表面金属丰度
        float  SurfaceEnergeticNuclide; // 表面含能核素质量分数
        float  SurfaceVolatiles;        // 表面挥发物质量分数
        float  Teff;                    // 有效温度
        float  CoreTemp;                // 核心温度
        float  CoreDensity;             // 核心密度，单位 kg/m^3
        float  StellarWindSpeed;        // 恒星风速度，单位 m/s
        float  StellarWindMassLossRate; // 恒星风质量损失率，单位 kg/s
        float  EvolutionProgress;       // 演化进度
        float  Lifetime;                // 寿命，单位 yr
        float  MinCoilMass;             // 最小举星器赤道偏转线圈质量，单位 kg

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
    Star& SetFeH(float FeH);
    Star& SetSurfaceH1(float SurfaceH1);
    Star& SetSurfaceZ(float SurfaceZ);
    Star& SetSurfaceEnergeticNuclide(float SurfaceEnergeticNuclide);
    Star& SetSurfaceVolatiles(float SurfaceVolatiles);
    Star& SetTeff(float Teff);
    Star& SetCoreTemp(float CoreTemp);
    Star& SetCoreDensity(float CoreDensity);
    Star& SetStellarWindSpeed(float StellarWindSpeed);
    Star& SetStellarWindMassLossRate(float StellarWindMassLossRate);
    Star& SetEvolutionProgress(float EvolutionProgress);
    Star& SetLifetime(float Lifetime);
    Star& SetMinCoilMass(float MinCoilMass);
    Star& SetNormal(const glm::vec2 Normal);
    Star& SetEvolutionPhase(Phase EvolutionPhase);
    Star& SetEvolutionEnding(Death EvolutionEnding);
    Star& SetStellarClass(const Modules::StellarClass& StellarClass);

    // Getters
    // Getters for ExtendedProperties
    // ------------------------------
    double GetMass() const;
    double GetLuminosity() const;
    float  GetFeH() const;
    float  GetSurfaceH1() const;
    float  GetSurfaceZ() const;
    float  GetSurfaceEnergeticNuclide() const;
    float  GetSurfaceVolatiles() const;
    float  GetTeff() const;
    float  GetCoreTemp() const;
    float  GetCoreDensity() const;
    float  GetStellarWindSpeed() const;
    float  GetStellarWindMassLossRate() const;
    float  GetEvolutionProgress() const;
    float  GetLifetime() const;
    float  GetMinCoilMass() const;
    const  glm::vec2& GetNormal() const;
    Phase  GetEvolutionPhase() const;
    Death  GetEvolutionEnding() const;
    const  Modules::StellarClass& GetStellarClass() const;

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

_ASTROOBJECT_END
_NPGS_END

#include "Star.inl"
