#pragma once

#include <string>
#include <unordered_map>

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
        double EffectiveTemp;
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
    Star& SetEffectiveTemp(double EffectiveTemp);
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
    double GetEffectiveTemp() const;
    double GetCoreTemp() const;
    double GetCoreDensity() const;
    double GetStellarWindSpeed() const;
    double GetStellarWindMassLossRate() const;
    const std::string& GetSpectralType() const;
    const Modules::StellarClass& GetStellarClass() const;
    Phase GetEvolutionPhase() const;
    double GetEvolutionProgress() const;
    double GetLifetime() const;

    static const std::unordered_map<int, std::string> _kSpectralTemp_O;
    static const std::unordered_map<int, std::string> _kSpectralTemp_B;
    static const std::unordered_map<int, std::string> _kSpectralTemp_A;
    static const std::unordered_map<int, std::string> _kSpectralTemp_F;
    static const std::unordered_map<int, std::string> _kSpectralTemp_G;
    static const std::unordered_map<int, std::string> _kSpectralTemp_K;
    static const std::unordered_map<int, std::string> _kSpectralTemp_M;
    static const std::unordered_map<int, std::string> _kSpectralTemp_WC;
    static const std::unordered_map<int, std::string> _kSpectralTemp_WN;
    static const std::unordered_map<int, std::string> _kSpectralTemp_WO;
    static const std::unordered_map<int, std::string> _kSpectralTemp_WNxh;

    static const std::unordered_map<int, std::unordered_map<int, std::string>> _kInitialCommonTemp;
    static const std::unordered_map<int, std::unordered_map<int, std::string>> _kInitialWolfRayetTemp;

    static const std::unordered_map<int, int> _kLuminosity;

private:
    ExtendedProperties _StarExtraProperties{};
};

_ASTROOBJECT_END
_NPGS_END

#include "Star.inl"
