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

inline Star& Star::SetFeH(double FeH) {
    _StarExtraProperties.FeH = FeH;
    return *this;
}

inline Star& Star::SetSurfaceFeH(double SurfaceFeH) {
    _StarExtraProperties.SurfaceFeH = SurfaceFeH;
    return *this;
}

inline Star& Star::SetSurfaceEnergeticNuclide(double SurfaceEnergeticNuclide) {
    _StarExtraProperties.SurfaceEnergeticNuclide = SurfaceEnergeticNuclide;
    return *this;
}

inline Star& Star::SetSurfaceVolatiles(double SurfaceVolatiles) {
    _StarExtraProperties.SurfaceVolatiles = SurfaceVolatiles;
    return *this;
}

inline Star& Star::SetTeff(double Teff) {
    _StarExtraProperties.Teff = Teff;
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

inline Star& Star::SetStellarWindMassLossRate(double StellarWindMassLossRate) {
    _StarExtraProperties.StellarWindMassLossRate = StellarWindMassLossRate;
    return *this;
}

inline Star& Star::SetEvolutionProgress(double EvolutionProgress) {
    _StarExtraProperties.EvolutionProgress = EvolutionProgress;
    return *this;
}

inline Star& Star::SetLifetime(double Lifetime) {
    _StarExtraProperties.Lifetime = Lifetime;
    return *this;
}

inline Star& Star::SetMinCoilMass(double MinCoilMass) {
    _StarExtraProperties.MinCoilMass = MinCoilMass;
    return *this;
}

inline Star& Star::SetNormal(const glm::vec2 Normal) {
    _StarExtraProperties.Normal = Normal;
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

inline double Star::GetLuminosity() const {
    return _StarExtraProperties.Luminosity;
}

inline double Star::GetFeH() const {
    return _StarExtraProperties.FeH;
}

inline double Star::GetSurfaceFeH() const {
    return _StarExtraProperties.SurfaceFeH;
}

inline double Star::GetSurfaceEnergeticNuclide() const {
    return _StarExtraProperties.SurfaceEnergeticNuclide;
}

inline double Star::GetSurfaceVolatiles() const {
    return _StarExtraProperties.SurfaceVolatiles;
}

inline double Star::GetTeff() const {
    return _StarExtraProperties.Teff;
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

inline double Star::GetStellarWindMassLossRate() const {
    return _StarExtraProperties.StellarWindMassLossRate;
}

inline double Star::GetEvolutionProgress() const {
    return _StarExtraProperties.EvolutionProgress;
}

inline double Star::GetLifetime() const {
    return _StarExtraProperties.Lifetime;
}

inline double Star::GetMinCoilMass() const {
    return _StarExtraProperties.MinCoilMass;
}

inline const glm::vec2& Star::GetNormal() const {
    return _StarExtraProperties.Normal;
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
