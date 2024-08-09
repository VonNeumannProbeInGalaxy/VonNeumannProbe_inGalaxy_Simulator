#pragma once

#include "Star.h"

_NPGS_BEGIN
_ASTROOBJECT_BEGIN

inline Star& Star::SetExtendedProperties(const ExtendedProperties& StarExtraProperties) {
    _StarExtraProperties = StarExtraProperties;
    return *this;
}

inline const Star::ExtendedProperties& Star::GetExtendedProperties() const {
    return _StarExtraProperties;
}

inline Star& Star::SetLuminosity(double Luminosity) {
    _StarExtraProperties.Luminosity = Luminosity;
    return *this;
}

inline Star& Star::SetAbsoluteMagnitude(double AbsoluteMagnitude) {
    _StarExtraProperties.AbsoluteMagnitude = AbsoluteMagnitude;
    return *this;
}

inline Star& Star::SetFeH(double FeH) {
    _StarExtraProperties.FeH = FeH;
    return *this;
}

inline Star& Star::SetSurfaceFeH(double SurfaceFeH) {
    _StarExtraProperties.SurfaceFeH = SurfaceFeH;
    return *this;
}

inline Star& Star::SetEffectiveTemp(double EffectiveTemp) {
    _StarExtraProperties.EffectiveTemp = EffectiveTemp;
    return *this;
}

inline Star& Star::SetCoreTemp(double CoreTemp) {
    _StarExtraProperties.CoreTemp = CoreTemp;
    return *this;
}

inline Star& Star::SetCoreDensity(double CoreDensity) {
    _StarExtraProperties.CoreDensity = CoreDensity;
    return *this;
}

inline Star& Star::SetStellarWindSpeed(double StellarWindSpeed) {
    _StarExtraProperties.StellarWindSpeed = StellarWindSpeed;
    return *this;
}

inline Star& Star::SetStellarWindDensity(double StellarWindDensity) {
    _StarExtraProperties.StellarWindDensity = StellarWindDensity;
    return *this;
}

inline Star& Star::SetStellarWindMomentum(double StellarWindMomentum) {
    _StarExtraProperties.StellarWindMomentum = StellarWindMomentum;
    return *this;
}

inline Star& Star::SetStellarWindMassLossRate(double StellarWindMassLossRate) {
    _StarExtraProperties.StellarWindMassLossRate = StellarWindMassLossRate;
    return *this;
}

inline Star& Star::SetSpectralType(const std::string& SpectralType) {
    _StarExtraProperties.SpectralType = SpectralType;
    return *this;
}

inline Star& Star::SetStellarClass(const Modules::StellarClass& StellarClass) {
    _StarExtraProperties.StellarClass = StellarClass;
    return *this;
}

inline Star& Star::SetEvolutionPhase(Phase EvolutionPhase) {
    _StarExtraProperties.EvolutionPhase = EvolutionPhase;
    return *this;
}

inline Star& Star::SetEvolutionProgress(double EvolutionProgress) {
    _StarExtraProperties.EvolutionProgress = EvolutionProgress;
    return *this;
}

inline double Star::GetLuminosity() const {
    return _StarExtraProperties.Luminosity;
}

inline double Star::GetAbsoluteMagnitude() const {
    return _StarExtraProperties.AbsoluteMagnitude;
}

inline double Star::GetFeH() const {
    return _StarExtraProperties.FeH;
}

inline double Star::GetSurfaceFeH() const {
    return _StarExtraProperties.SurfaceFeH;
}

inline double Star::GetEffectiveTemp() const {
    return _StarExtraProperties.EffectiveTemp;
}

inline double Star::GetCoreTemp() const {
    return _StarExtraProperties.CoreTemp;
}

inline double Star::GetCoreDensity() const {
    return _StarExtraProperties.CoreDensity;
}

inline double Star::GetStellarWindSpeed() const {
    return _StarExtraProperties.StellarWindSpeed;
}

inline double Star::GetStellarWindDensity() const {
    return _StarExtraProperties.StellarWindDensity;
}

inline double Star::GetStellarWindMomentum() const {
    return _StarExtraProperties.StellarWindMomentum;
}

inline double Star::GetStellarWindMassLossRate() const {
    return _StarExtraProperties.StellarWindMassLossRate;
}

inline const std::string& Star::GetSpectralType() const {
    return _StarExtraProperties.SpectralType;
}

inline const Modules::StellarClass& Star::GetStellarClass() const {
    return _StarExtraProperties.StellarClass;
}

inline Star::Phase Star::GetEvolutionPhase() const {
    return _StarExtraProperties.EvolutionPhase;
}

inline double Star::GetEvolutionProgress() const {
    return _StarExtraProperties.EvolutionProgress;
}

_ASTROOBJECT_END
_NPGS_END
