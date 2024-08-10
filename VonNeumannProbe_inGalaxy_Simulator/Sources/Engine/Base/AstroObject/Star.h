#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Engine/Base/AstroObject/CelestialObject.h"
#include "Engine/Core/Modules/Stellar/StellarClass.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_ASTROOBJECT_BEGIN

class Star : public CelestialBody {
public:
    enum class Phase : int {
        kPrevMainSequence = -1,
        kMainSequence     =  0,
        kRedGiant         =  2,
        kCoreHeBurn       =  3,
        kEarlyAgb         =  4,
        kThermalPulseAgb  =  5,
        kPostAgb          =  6,
        kWolfRayet        =  9
    };

    struct ExtendedProperties {
        double Luminosity;
        double AbsoluteMagnitude;
        double FeH;
        double SurfaceFeH;
        double Teff;
        double CoreTemp;
        double CoreDensity;
        double StellarWindSpeed;
        double StellarWindMassLossRate;
        double EvolutionProgress;
        double Lifetime;

        std::string SpectralType;
        Modules::StellarClass StellarClass;

        Phase EvolutionPhase;
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
    Star& SetAbsoluteMagnitude(double AbsoluteMagnitude);
    Star& SetFeH(double FeH);
    Star& SetSurfaceFeH(double SurfaceFeH);
    Star& SetTeff(double Teff);
    Star& SetCoreTemp(double CoreTemp);
    Star& SetCoreDensity(double CoreDensity);
    Star& SetStellarWindSpeed(double StellarWindSpeed);
    Star& SetStellarWindMassLossRate(double StellarWindMassLossRate);
    Star& SetSpectralType(const std::string& SpectralType);
    Star& SetStellarClass(const Modules::StellarClass& StellarClass);
    Star& SetEvolutionPhase(Phase EvolutionPhase);
    Star& SetEvolutionProgress(double EvolutionProgress);
    Star& SetLifetime(double Lifetime);

    // Getters
    // Getters for ExtendedProperties
    // ------------------------------
    double GetLuminosity() const;
    double GetAbsoluteMagnitude() const;
    double GetFeH() const;
    double GetSurfaceFeH() const;
    double GetTeff() const;
    double GetCoreTemp() const;
    double GetCoreDensity() const;
    double GetStellarWindSpeed() const;
    double GetStellarWindMassLossRate() const;
    const std::string& GetSpectralType() const;
    const Modules::StellarClass& GetStellarClass() const;
    Phase GetEvolutionPhase() const;
    double GetEvolutionProgress() const;
    double GetLifetime() const;

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
    ExtendedProperties _StarExtraProperties;
};

_ASTROOBJECT_END
_NPGS_END

#include "Star.inl"
