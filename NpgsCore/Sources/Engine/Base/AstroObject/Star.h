#pragma once

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
        kBlackHole                =  15,
        kNull                     =  16
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
        double Luminosity;              // 辐射光度，单位 W
        double FeH;                     // 金属丰度
        double SurfaceFeH;              // 表面金属丰度
        double SurfaceEnergeticNuclide; // 表面含能核素质量分数
        double SurfaceVolatiles;        // 表面挥发物质量分数
        double Teff;                    // 有效温度
        double CoreTemp;                // 核心温度
        double CoreDensity;             // 核心密度，单位 kg/m^3
        double StellarWindSpeed;        // 恒星风速度，单位 m/s
        double StellarWindMassLossRate; // 恒星风质量损失率，单位 kg/s
        double EvolutionProgress;       // 演化进度
        double Lifetime;                // 寿命，单位 yr
        double MinCoilMass;             // 最小举星器赤道偏转线圈质量，单位 kg

        glm::vec2 Normal;               // 法向量，球坐标表示，(theta, phi)

        Phase EvolutionPhase;
        Death EvolutionEnding;

        Modules::StellarClass StellarClass;
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
    Star& SetLuminosity(double Luminosity);
    Star& SetFeH(double FeH);
    Star& SetSurfaceFeH(double SurfaceFeH);
    Star& SetSurfaceEnergeticNuclide(double SurfaceEnergeticNuclide);
    Star& SetSurfaceVolatiles(double SurfaceVolatiles);
    Star& SetTeff(double Teff);
    Star& SetCoreTemp(double CoreTemp);
    Star& SetCoreDensity(double CoreDensity);
    Star& SetStellarWindSpeed(double StellarWindSpeed);
    Star& SetStellarWindMassLossRate(double StellarWindMassLossRate);
    Star& SetEvolutionProgress(double EvolutionProgress);
    Star& SetLifetime(double Lifetime);
    Star& SetMinCoilMass(double MinCoilMass);
    Star& SetNormal(const glm::vec2 Normal);
    Star& SetEvolutionPhase(Phase EvolutionPhase);
    Star& SetEvolutionEnding(Death EvolutionEnding);
    Star& SetStellarClass(const Modules::StellarClass& StellarClass);

    // Getters
    // Getters for ExtendedProperties
    // ------------------------------
    double GetLuminosity() const;
    double GetFeH() const;
    double GetSurfaceFeH() const;
    double GetSurfaceEnergeticNuclide() const;
    double GetSurfaceVolatiles() const;
    double GetTeff() const;
    double GetCoreTemp() const;
    double GetCoreDensity() const;
    double GetStellarWindSpeed() const;
    double GetStellarWindMassLossRate() const;
    double GetEvolutionProgress() const;
    double GetLifetime() const;
    double GetMinCoilMass() const;
    const glm::vec2& GetNormal() const;
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
    static const std::vector<std::pair<int, int>> _kSpectralSubclassMap_WC;
    static const std::vector<std::pair<int, int>> _kSpectralSubclassMap_WN;
    static const std::vector<std::pair<int, int>> _kSpectralSubclassMap_WO;
    static const std::vector<std::pair<int, int>> _kSpectralSubclassMap_WNxh;

    static const std::vector<std::pair<int, std::vector<std::pair<int, int>>>> _kInitialCommonMap;
    static const std::vector<std::pair<int, std::vector<std::pair<int, int>>>> _kInitialWolfRayetMap;

    static const std::unordered_map<Phase, Npgs::Modules::StellarClass::LuminosityClass> _kLuminosity;

private:
    ExtendedProperties _StarExtraProperties{};
};

_ASTROOBJECT_END
_NPGS_END

#include "Star.inl"
