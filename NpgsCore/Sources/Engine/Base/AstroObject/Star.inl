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

inline Star& Star::SetMass(double Mass) {
    _StarExtraProperties.Mass = Mass;
    return *this;
}

inline Star& Star::SetLuminosity(double Luminosity) {
    _StarExtraProperties.Luminosity = Luminosity;
    return *this;
}

inline Star& Star::SetLifetime(double Lifetime) {
    _StarExtraProperties.Lifetime = Lifetime;
    return *this;
}

inline Star& Star::SetEvolutionProgress(double EvolutionProgress) {
    _StarExtraProperties.EvolutionProgress = EvolutionProgress;
    return *this;
}

inline Star& Star::SetFeH(float FeH) {
    _StarExtraProperties.FeH = FeH;
    return *this;
}

inline Star& Star::SetInitialMass(float InitialMass) {
    _StarExtraProperties.InitialMass = InitialMass;
    return *this;
}

inline Star& Star::SetSurfaceH1(float SurfaceH1) {
    _StarExtraProperties.SurfaceH1 = SurfaceH1;
    return *this;
}

inline Star& Star::SetSurfaceZ(float SurfaceZ) {
    _StarExtraProperties.SurfaceZ = SurfaceZ;
    return *this;
}

inline Star& Star::SetSurfaceEnergeticNuclide(float SurfaceEnergeticNuclide) {
    _StarExtraProperties.SurfaceEnergeticNuclide = SurfaceEnergeticNuclide;
    return *this;
}

inline Star& Star::SetSurfaceVolatiles(float SurfaceVolatiles) {
    _StarExtraProperties.SurfaceVolatiles = SurfaceVolatiles;
    return *this;
}

inline Star& Star::SetTeff(float Teff) {
    _StarExtraProperties.Teff = Teff;
    return *this;
}

inline Star& Star::SetCoreTemp(float CoreTemp) {
    _StarExtraProperties.CoreTemp = CoreTemp;
    return *this;
}

inline Star& Star::SetCoreDensity(float CoreDensity) {
    _StarExtraProperties.CoreDensity = CoreDensity;
    return *this;
}

inline Star& Star::SetStellarWindSpeed(float StellarWindSpeed) {
    _StarExtraProperties.StellarWindSpeed = StellarWindSpeed;
    return *this;
}

inline Star& Star::SetStellarWindMassLossRate(float StellarWindMassLossRate) {
    _StarExtraProperties.StellarWindMassLossRate = StellarWindMassLossRate;
    return *this;
}

inline Star& Star::SetMinCoilMass(float MinCoilMass) {
    _StarExtraProperties.MinCoilMass = MinCoilMass;
    return *this;
}

inline Star& Star::SetEvolutionPhase(Phase EvolutionPhase) {
    _StarExtraProperties.EvolutionPhase = EvolutionPhase;
    return *this;
}

inline Star& Star::SetEvolutionEnding(Death EvolutionEnding) {
    _StarExtraProperties.EvolutionEnding = EvolutionEnding;
    return *this;
}

inline Star& Star::SetStellarClass(const Modules::StellarClass& StellarClass) {
    _StarExtraProperties.StellarClass = StellarClass;
    return *this;
}

inline double Star::GetMass() const {
    return _StarExtraProperties.Mass;
}

inline double Star::GetLuminosity() const {
    return _StarExtraProperties.Luminosity;
}

inline double Star::GetLifetime() const {
    return _StarExtraProperties.Lifetime;
}

inline double Star::GetEvolutionProgress() const {
    return _StarExtraProperties.EvolutionProgress;
}

inline float Star::GetFeH() const {
    return _StarExtraProperties.FeH;
}

inline float Star::GetInitialMass() const {
    return _StarExtraProperties.InitialMass;
}

inline float Star::GetSurfaceH1() const {
    return _StarExtraProperties.SurfaceH1;
}

inline float Star::GetSurfaceZ() const {
    return _StarExtraProperties.SurfaceZ;
}

inline float Star::GetSurfaceEnergeticNuclide() const {
    return _StarExtraProperties.SurfaceEnergeticNuclide;
}

inline float Star::GetSurfaceVolatiles() const {
    return _StarExtraProperties.SurfaceVolatiles;
}

inline float Star::GetTeff() const {
    return _StarExtraProperties.Teff;
}

inline float Star::GetCoreTemp() const {
    return _StarExtraProperties.CoreTemp;
}

inline float Star::GetCoreDensity() const {
    return _StarExtraProperties.CoreDensity;
}

inline float Star::GetStellarWindSpeed() const {
    return _StarExtraProperties.StellarWindSpeed;
}

inline float Star::GetStellarWindMassLossRate() const {
    return _StarExtraProperties.StellarWindMassLossRate;
}

inline float Star::GetMinCoilMass() const {
    return _StarExtraProperties.MinCoilMass;
}

inline Star::Phase Star::GetEvolutionPhase() const {
    return _StarExtraProperties.EvolutionPhase;
}

inline Star::Death Star::GetEvolutionEnding() const {
    return _StarExtraProperties.EvolutionEnding;
}

inline const Modules::StellarClass& Star::GetStellarClass() const {
    return _StarExtraProperties.StellarClass;
}

_ASTROOBJECT_END
_NPGS_END
