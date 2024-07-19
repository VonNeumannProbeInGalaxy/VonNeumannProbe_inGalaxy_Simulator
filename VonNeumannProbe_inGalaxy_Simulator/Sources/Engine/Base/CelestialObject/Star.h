#pragma once

#include "CelestialObject.h"

#include <string>
#include "Engine/Core/Constants.h"
#include "Engine/Core/Modules/Stellar/StellarClass.h"

class Star : public CelestialBody {
public:
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
        StellarClass StellarClass;
    };

public:
    Star() = delete;
    Star(const CelestialBody::BaseProperties& StarBaseProperties, const ExtendedProperties& StarExtraProperties);

public:
    // Setters
    // Setters for ExtendedProperties
    // ------------------------------
    void SetLuminosity(double Luminosity) {
        _StarExtraProperties.Luminosity = Luminosity;
    }

    void SetAbsMagn(double AbsMagn) {
        _StarExtraProperties.AbsMagn = AbsMagn;
    }

    void SetFeH(double FeH) {
        _StarExtraProperties.FeH = FeH;
    }

    void SetSurfFeH(double SurfFeH) {
        _StarExtraProperties.SurfFeH = SurfFeH;
    }

    void SetEffTemp(double EffTemp) {
        _StarExtraProperties.EffTemp = EffTemp;
    }

    void SetStellarWindSpeed(double StellarWindSpeed) {
        _StarExtraProperties.StellarWindSpeed = StellarWindSpeed;
    }

    void SetStellarWindDensity(double StellarWindDensity) {
        _StarExtraProperties.StellarWindDensity = StellarWindDensity;
    }

    void SetStellarWindMomentum(double StellarWindMomentum) {
        _StarExtraProperties.StellarWindMomentum = StellarWindMomentum;
    }

    void SetStellarWindMassLossRate(double StellarWindMassLossRate) {
        _StarExtraProperties.StellarWindMassLossRate = StellarWindMassLossRate;
    }

    void SetSpectralType(const std::string& SpectralType) {
        _StarExtraProperties.SpectralType = SpectralType;
    }

    void SetStellarClass(const StellarClass& StellarClass) {
        _StarExtraProperties.StellarClass = StellarClass;
    }

private:
    ExtendedProperties _StarExtraProperties;
};
