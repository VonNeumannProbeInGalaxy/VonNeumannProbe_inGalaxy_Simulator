#pragma once

#include "CelestialObject.h"

#include <string>

constexpr inline double kSolarRadius = 695700;
constexpr inline double kSolarMass   = 1.9884e30;
constexpr inline double kSolarLum    = 3.828e26;
constexpr inline double kSolarFeH    = 0.0;

class Star : public CelestialBody {
public:
    Star() = delete;
    Star(double Radius, double Mass, double Oblateness, double AxisTilt,
        double Age, double EscapeVelocity, double MagneticField,
        double Luminosity, double AbsMagn, double FeH, double SurfFeH,
        double EffTemp, double StellarWindSpeed, double StellarWindDensity,
        double StellarWindMomentum, double StellarWindMassLossRate,
        const std::string& SpectralType);

public:
    void SetLuminosity(double Luminosity) {
        _Luminosity = Luminosity;
    }

    void SetAbsMagn(double AbsMagn) {
        _AbsMagn = AbsMagn;
    }

    void SetFeH(double FeH) {
        _FeH = FeH;
    }

    void SetSurfFeH(double SurfFeH) {
        _SurfFeH = SurfFeH;
    }

    void SetEffTemp(double EffTemp) {
        _EffTemp = EffTemp;
    }

    void SetStellarWindSpeed(double StellarWindSpeed) {
        _StellarWindSpeed = StellarWindSpeed;
    }

    void SetStellarWindDensity(double StellarWindDensity) {
        _StellarWindDensity = StellarWindDensity;
    }

    void SetStellarWindMomentum(double StellarWindMomentum) {
        _StellarWindMomentum = StellarWindMomentum;
    }

    void SetStellarWindMassLossRate(double StellarWindMassLossRate) {
        _StellarWindMassLossRate = StellarWindMassLossRate;
    }

    void SetSpectralType(const std::string& SpectralType) {
        _SpectralType = SpectralType;
    }

public:
    double GetLuminosity() const {
        return _Luminosity;
    }

    double GetAbsMagn() const {
        return _AbsMagn;
    }

    double GetFeH() const {
        return _FeH;
    }

    double GetSurfFeH() const {
        return _SurfFeH;
    }

    double GetEffTemp() const {
        return _EffTemp;
    }

    double GetStellarWindSpeed() const {
        return _StellarWindSpeed;
    }

    double GetStellarWindDensity() const {
        return _StellarWindDensity;
    }

    double GetStellarWindMomentum() const {
        return _StellarWindMomentum;
    }

    double GetStellarWindMassLossRate() const {
        return _StellarWindMassLossRate;
    }

    const std::string& GetSpectralType() const {
        return _SpectralType;
    }

private:
    double _Luminosity;
    double _AbsMagn;
    double _FeH;
    double _SurfFeH;
    double _EffTemp;
    double _StellarWindSpeed;
    double _StellarWindDensity;
    double _StellarWindMomentum;
    double _StellarWindMassLossRate;

    std::string _SpectralType;
};
