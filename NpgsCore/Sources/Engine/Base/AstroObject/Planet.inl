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

inline Planet& Planet::SetOceanMass(const MassProperties& OceanMass) {
    _PlanetExtraProperties.OceanMass = OceanMass;
    return *this;
}

inline Planet& Planet::SetAtmosphereMass(const MassProperties& AtmosphereMass) {
    _PlanetExtraProperties.AtmosphereMass = AtmosphereMass;
    return *this;
}

inline Planet& Planet::SetCoreMass(const MassProperties& CoreMass) {
    _PlanetExtraProperties.CoreMass = CoreMass;
    return *this;
}

inline Planet& Planet::SetCrustMineralMass(const boost::multiprecision::int128_t& CrustMineralMass) {
    _PlanetExtraProperties.CrustMineralMass = CrustMineralMass;
    return *this;
}

inline Planet& Planet::SetRingsMass(const boost::multiprecision::int128_t& RingsMass) {
    _PlanetExtraProperties.RingsMass = RingsMass;
    return *this;
}

inline Planet& Planet::SetPlanetType(PlanetType Type) {
    _PlanetExtraProperties.Type = Type;
    return *this;
}

inline const Planet::MassProperties& Planet::GetOceanMass() const {
    return _PlanetExtraProperties.OceanMass;
}

inline const Planet::MassProperties& Planet::GetAtmosphereMass() const {
    return _PlanetExtraProperties.AtmosphereMass;
}

inline const Planet::MassProperties& Planet::GetCoreMass() const {
    return _PlanetExtraProperties.CoreMass;
}

inline const boost::multiprecision::int128_t& Planet::GetCrustMineralMass() const {
    return _PlanetExtraProperties.CrustMineralMass;
}

inline const boost::multiprecision::int128_t& Planet::GetRingsMass() const {
    return _PlanetExtraProperties.RingsMass;
}

inline Planet::PlanetType Planet::GetPlanetType() const {
    return _PlanetExtraProperties.Type;
}

_ASTROOBJECT_END
_NPGS_END
