#pragma once

#include "Planet.h"

_NPGS_BEGIN
_ASTROOBJECT_BEGIN

inline Planet& Planet::SetExtendedProperties(const ExtendedProperties& PlanetExtraProperties) {
    _PlanetExtraProperties = PlanetExtraProperties;
    return *this;
}

inline const Planet::ExtendedProperties& Planet::GetExtendedProperties() const {
    return _PlanetExtraProperties;
}

inline Planet& Planet::SetAtmosphereMass(const ComplexMass& AtmosphereMass) {
    _PlanetExtraProperties.AtmosphereMass = AtmosphereMass;
    return *this;
}

inline Planet& Planet::SetCoreMass(const ComplexMass& CoreMass) {
    _PlanetExtraProperties.CoreMass = CoreMass;
    return *this;
}

inline Planet& Planet::SetOceanMass(const ComplexMass& OceanMass) {
    _PlanetExtraProperties.OceanMass = OceanMass;
    return *this;
}

inline Planet& Planet::SetCrustMineralMass(float CrustMineralMass) {
    _PlanetExtraProperties.CrustMineralMass = boost::multiprecision::int128_t(CrustMineralMass);
    return *this;
}

inline Planet& Planet::SetCrustMineralMass(const boost::multiprecision::int128_t& CrustMineralMass) {
    _PlanetExtraProperties.CrustMineralMass = CrustMineralMass;
    return *this;
}

inline Planet& Planet::SetRingsMass(float RingsMass) {
    _PlanetExtraProperties.RingsMass = boost::multiprecision::int128_t(RingsMass);
    return *this;
}

inline Planet& Planet::SetRingsMass(const boost::multiprecision::int128_t& RingsMass) {
    _PlanetExtraProperties.RingsMass = RingsMass;
    return *this;
}

inline Planet& Planet::SetMigration(bool bIsMigrated) {
    _PlanetExtraProperties.bIsMigrated = bIsMigrated;
    return *this;
}

inline Planet& Planet::SetPlanetType(PlanetType Type) {
    _PlanetExtraProperties.Type = Type;
    return *this;
}

inline Planet& Planet::SetAtmosphereMassZ(float AtmosphereMassZ) {
    _PlanetExtraProperties.AtmosphereMass.Z = boost::multiprecision::int128_t(AtmosphereMassZ);
    return *this;
}

inline Planet& Planet::SetAtmosphereMassZ(const boost::multiprecision::int128_t& AtmosphereMassZ) {
    _PlanetExtraProperties.AtmosphereMass.Z = AtmosphereMassZ;
    return *this;
}

inline Planet& Planet::SetAtmosphereMassVolatiles(float AtmosphereMassVolatiles) {
    _PlanetExtraProperties.AtmosphereMass.Volatiles = boost::multiprecision::int128_t(AtmosphereMassVolatiles);
    return *this;
}

inline Planet& Planet::SetAtmosphereMassVolatiles(const boost::multiprecision::int128_t& AtmosphereMassVolatiles) {
    _PlanetExtraProperties.AtmosphereMass.Volatiles = AtmosphereMassVolatiles;
    return *this;
}

inline Planet& Planet::SetAtmosphereMassEnergeticNuclide(float AtmosphereMassEnergeticNuclide) {
    _PlanetExtraProperties.AtmosphereMass.EnergeticNuclide = boost::multiprecision::int128_t(AtmosphereMassEnergeticNuclide);
    return *this;
}

inline Planet& Planet::SetAtmosphereMassEnergeticNuclide(const boost::multiprecision::int128_t& AtmosphereMassEnergeticNuclide) {
    _PlanetExtraProperties.AtmosphereMass.EnergeticNuclide = AtmosphereMassEnergeticNuclide;
}

inline Planet& Planet::SetCoreMassZ(float CoreMassZ) {
    _PlanetExtraProperties.CoreMass.Z = boost::multiprecision::int128_t(CoreMassZ);
    return *this;
}

inline Planet& Planet::SetCoreMassZ(const boost::multiprecision::int128_t& CoreMassZ) {
    _PlanetExtraProperties.CoreMass.Z = CoreMassZ;
    return *this;
}

inline Planet& Planet::SetCoreMassVolatiles(float CoreMassVolatiles) {
    _PlanetExtraProperties.CoreMass.Volatiles = boost::multiprecision::int128_t(CoreMassVolatiles);
    return *this;
}

inline Planet& Planet::SetCoreMassVolatiles(const boost::multiprecision::int128_t& CoreMassVolatiles) {
    _PlanetExtraProperties.CoreMass.Volatiles = CoreMassVolatiles;
    return *this;
}

inline Planet& Planet::SetCoreMassEnergeticNuclide(float CoreMassEnergeticNuclide) {
    _PlanetExtraProperties.CoreMass.EnergeticNuclide = boost::multiprecision::int128_t(CoreMassEnergeticNuclide);
    return *this;
}

inline Planet& Planet::SetCoreMassEnergeticNuclide(const boost::multiprecision::int128_t& CoreMassEnergeticNuclide) {
    _PlanetExtraProperties.CoreMass.EnergeticNuclide = CoreMassEnergeticNuclide;
    return *this;
}

inline Planet& Planet::SetOceanMassZ(float OceanMassZ) {
    _PlanetExtraProperties.OceanMass.Z = boost::multiprecision::int128_t(OceanMassZ);
    return *this;
}

inline Planet& Planet::SetOceanMassZ(const boost::multiprecision::int128_t& OceanMassZ) {
    _PlanetExtraProperties.OceanMass.Z = OceanMassZ;
    return *this;
}

inline Planet& Planet::SetOceanMassVolatiles(float OceanMassVolatiles) {
    _PlanetExtraProperties.OceanMass.Volatiles = boost::multiprecision::int128_t(OceanMassVolatiles);
    return *this;
}

inline Planet& Planet::SetOceanMassVolatiles(const boost::multiprecision::int128_t& OceanMassVolatiles) {
    _PlanetExtraProperties.OceanMass.Volatiles = OceanMassVolatiles;
    return *this;
}

inline Planet& Planet::SetOceanMassEnergeticNuclide(float OceanMassEnergeticNuclide) {
    _PlanetExtraProperties.OceanMass.EnergeticNuclide = boost::multiprecision::int128_t(OceanMassEnergeticNuclide);
    return *this;
}

inline Planet& Planet::SetOceanMassEnergeticNuclide(const boost::multiprecision::int128_t& OceanMassEnergeticNuclide) {
    _PlanetExtraProperties.OceanMass.EnergeticNuclide = OceanMassEnergeticNuclide;
    return *this;
}

inline const Planet::ComplexMass& Planet::GetAtmosphereMass() const {
    return _PlanetExtraProperties.AtmosphereMass;
}

inline const Planet::ComplexMass& Planet::GetCoreMass() const {
    return _PlanetExtraProperties.CoreMass;
}

inline const Planet::ComplexMass& Planet::GetOceanMass() const {
    return _PlanetExtraProperties.OceanMass;
}

inline const float Planet::GetMass() const {
    float AtmosphereMass = std::stof((GetAtmosphereMass().EnergeticNuclide + GetAtmosphereMass().Volatiles + GetAtmosphereMass().Z).str());
    float OceanMass      = std::stof((GetOceanMass().EnergeticNuclide + GetOceanMass().Volatiles + GetOceanMass().Z).str());
    float CoreMass       = std::stof((GetCoreMass().EnergeticNuclide + GetCoreMass().Volatiles + GetCoreMass().Z).str());

    return AtmosphereMass + OceanMass + CoreMass;
}

inline const boost::multiprecision::int128_t& Planet::GetCrustMineralMass() const {
    return _PlanetExtraProperties.CrustMineralMass;
}

inline const boost::multiprecision::int128_t& Planet::GetRingsMass() const {
    return _PlanetExtraProperties.RingsMass;
}

inline bool Planet::GetMigration() const {
    return _PlanetExtraProperties.bIsMigrated;
}

inline Planet::PlanetType Planet::GetPlanetType() const {
    return _PlanetExtraProperties.Type;
}

_ASTROOBJECT_END
_NPGS_END
