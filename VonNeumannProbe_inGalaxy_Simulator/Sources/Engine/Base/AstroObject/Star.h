#pragma once


#include <string>

#include "Engine/Base/AstroObject/CelestialObject.h"
#include "Engine/Core/Base.h"
#include "Engine/Core/Constants.h"
#include "Engine/Core/Modules/Stellar/StellarClass.h"

_NPGS_BEGIN

namespace AstroObject {

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
        double AbsMagn;
        double FeH;
        double SurfFeH;
        double EffTemp;
        double StellarWindSpeed;
        double StellarWindDensity;
        double StellarWindMomentum;
        double StellarWindMassLossRate;

        std::string SpectralType;
        Modules::StellarClass StellarClass;

        Phase EvolutionPhase;
    };

public:
    Star() = default;
    Star(const CelestialBody::BasicProperties& StarBasicProperties, const ExtendedProperties& StarExtraProperties);
    ~Star() = default;

public:
    Star& SetExtendedProperties(const ExtendedProperties& StarExtraProperties) {
        _StarExtraProperties = StarExtraProperties;
        return *this;
    }

    ExtendedProperties GetExtendedProperties() const {
        return _StarExtraProperties;
    }

    ExtendedProperties Data() const {
        return GetExtendedProperties();
    }

public:
    // Setters
    // Setters for ExtendedProperties
    // ------------------------------
    Star& SetLuminosity(double Luminosity) {
        _StarExtraProperties.Luminosity = Luminosity;
        return *this;
    }

    Star& SetAbsMagn(double AbsMagn) {
        _StarExtraProperties.AbsMagn = AbsMagn;
        return *this;
    }

    Star& SetFeH(double FeH) {
        _StarExtraProperties.FeH = FeH;
        return *this;
    }

    Star& SetSurfFeH(double SurfFeH) {
        _StarExtraProperties.SurfFeH = SurfFeH;
        return *this;
    }

    Star& SetEffTemp(double EffTemp) {
        _StarExtraProperties.EffTemp = EffTemp;
        return *this;
    }

    Star& SetStellarWindSpeed(double StellarWindSpeed) {
        _StarExtraProperties.StellarWindSpeed = StellarWindSpeed;
        return *this;
    }

    Star& SetStellarWindDensity(double StellarWindDensity) {
        _StarExtraProperties.StellarWindDensity = StellarWindDensity;
        return *this;
    }

    Star& SetStellarWindMomentum(double StellarWindMomentum) {
        _StarExtraProperties.StellarWindMomentum = StellarWindMomentum;
        return *this;
    }

    Star& SetStellarWindMassLossRate(double StellarWindMassLossRate) {
        _StarExtraProperties.StellarWindMassLossRate = StellarWindMassLossRate;
        return *this;
    }

    Star& SetSpectralType(const std::string& SpectralType) {
        _StarExtraProperties.SpectralType = SpectralType;
        return *this;
    }

    Star& SetStellarClass(const Modules::StellarClass& StellarClass) {
        _StarExtraProperties.StellarClass = StellarClass;
        return *this;
    }

    Star& SetEvolutionPhase(Phase EvolutionPhase) {
        _StarExtraProperties.EvolutionPhase = EvolutionPhase;
        return *this;
    }

    // Getters
    // Getters for ExtendedProperties
    // ------------------------------
    double GetLuminosity() const {
        return _StarExtraProperties.Luminosity;
    }

    double GetAbsMagn() const {
        return _StarExtraProperties.AbsMagn;
    }

    double GetFeH() const {
        return _StarExtraProperties.FeH;
    }

    double GetSurfFeH() const {
        return _StarExtraProperties.SurfFeH;
    }

    double GetEffTemp() const {
        return _StarExtraProperties.EffTemp;
    }

    double GetStellarWindSpeed() const {
        return _StarExtraProperties.StellarWindSpeed;
    }

    double GetStellarWindDensity() const {
        return _StarExtraProperties.StellarWindDensity;
    }

    double GetStellarWindMomentum() const {
        return _StarExtraProperties.StellarWindMomentum;
    }

    double GetStellarWindMassLossRate() const {
        return _StarExtraProperties.StellarWindMassLossRate;
    }

    const std::string& GetSpectralType() const {
        return _StarExtraProperties.SpectralType;
    }

    const Modules::StellarClass& GetStellarClass() const {
        return _StarExtraProperties.StellarClass;
    }

    Phase GetEvolPhase() const {
        return _StarExtraProperties.EvolutionPhase;
    }

private:
    ExtendedProperties _StarExtraProperties;
};

}

_NPGS_END
