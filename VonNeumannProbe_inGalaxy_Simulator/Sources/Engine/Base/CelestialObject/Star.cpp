#include "Star.h"

Star::Star(double Radius, double Mass, double Oblateness, double AxisTilt,
    double Age, double EscapeVelocity, double MagneticField,
    double Luminosity, double AbsMagn, double FeH, double SurfFeH,
    double EffTemp, double StellarWindSpeed, double StellarWindDensity,
    double StellarWindMomentum, double StellarWindMassLossRate,
    const std::string& SpectralType) :

    CelestialBody(Radius, Mass, Oblateness, AxisTilt, Age, EscapeVelocity, MagneticField),
    _Luminosity(Luminosity), _AbsMagn(AbsMagn), _FeH(FeH), _SurfFeH(SurfFeH),
    _EffTemp(EffTemp), _StellarWindSpeed(StellarWindSpeed), _StellarWindDensity(StellarWindDensity),
    _StellarWindMomentum(StellarWindMomentum), _StellarWindMassLossRate(StellarWindMassLossRate),
    _SpectralType(SpectralType)
{}
