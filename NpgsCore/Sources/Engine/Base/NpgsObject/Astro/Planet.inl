#pragma once

#include "Planet.h"

_NPGS_BEGIN
_ASTRO_BEGIN

inline static float ConvertToFloat(const boost::multiprecision::uint128_t& MultiPrecision) {
    return MultiPrecision.convert_to<float>();
}

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
    _PlanetExtraProperties.CrustMineralMass = boost::multiprecision::uint128_t(CrustMineralMass);
    return *this;
}

inline Planet& Planet::SetCrustMineralMass(const boost::multiprecision::uint128_t& CrustMineralMass) {
    _PlanetExtraProperties.CrustMineralMass = CrustMineralMass;
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
    _PlanetExtraProperties.AtmosphereMass.Z = boost::multiprecision::uint128_t(AtmosphereMassZ);
    return *this;
}

inline Planet& Planet::SetAtmosphereMassZ(const boost::multiprecision::uint128_t& AtmosphereMassZ) {
    _PlanetExtraProperties.AtmosphereMass.Z = AtmosphereMassZ;
    return *this;
}

inline Planet& Planet::SetAtmosphereMassVolatiles(float AtmosphereMassVolatiles) {
    _PlanetExtraProperties.AtmosphereMass.Volatiles = boost::multiprecision::uint128_t(AtmosphereMassVolatiles);
    return *this;
}

inline Planet& Planet::SetAtmosphereMassVolatiles(const boost::multiprecision::uint128_t& AtmosphereMassVolatiles) {
    _PlanetExtraProperties.AtmosphereMass.Volatiles = AtmosphereMassVolatiles;
    return *this;
}

inline Planet& Planet::SetAtmosphereMassEnergeticNuclide(float AtmosphereMassEnergeticNuclide) {
    _PlanetExtraProperties.AtmosphereMass.EnergeticNuclide = boost::multiprecision::uint128_t(AtmosphereMassEnergeticNuclide);
    return *this;
}

inline Planet& Planet::SetAtmosphereMassEnergeticNuclide(const boost::multiprecision::uint128_t& AtmosphereMassEnergeticNuclide) {
    _PlanetExtraProperties.AtmosphereMass.EnergeticNuclide = AtmosphereMassEnergeticNuclide;
}

inline Planet& Planet::SetCoreMassZ(float CoreMassZ) {
    _PlanetExtraProperties.CoreMass.Z = boost::multiprecision::uint128_t(CoreMassZ);
    return *this;
}

inline Planet& Planet::SetCoreMassZ(const boost::multiprecision::uint128_t& CoreMassZ) {
    _PlanetExtraProperties.CoreMass.Z = CoreMassZ;
    return *this;
}

inline Planet& Planet::SetCoreMassVolatiles(float CoreMassVolatiles) {
    _PlanetExtraProperties.CoreMass.Volatiles = boost::multiprecision::uint128_t(CoreMassVolatiles);
    return *this;
}

inline Planet& Planet::SetCoreMassVolatiles(const boost::multiprecision::uint128_t& CoreMassVolatiles) {
    _PlanetExtraProperties.CoreMass.Volatiles = CoreMassVolatiles;
    return *this;
}

inline Planet& Planet::SetCoreMassEnergeticNuclide(float CoreMassEnergeticNuclide) {
    _PlanetExtraProperties.CoreMass.EnergeticNuclide = boost::multiprecision::uint128_t(CoreMassEnergeticNuclide);
    return *this;
}

inline Planet& Planet::SetCoreMassEnergeticNuclide(const boost::multiprecision::uint128_t& CoreMassEnergeticNuclide) {
    _PlanetExtraProperties.CoreMass.EnergeticNuclide = CoreMassEnergeticNuclide;
    return *this;
}

inline Planet& Planet::SetOceanMassZ(float OceanMassZ) {
    _PlanetExtraProperties.OceanMass.Z = boost::multiprecision::uint128_t(OceanMassZ);
    return *this;
}

inline Planet& Planet::SetOceanMassZ(const boost::multiprecision::uint128_t& OceanMassZ) {
    _PlanetExtraProperties.OceanMass.Z = OceanMassZ;
    return *this;
}

inline Planet& Planet::SetOceanMassVolatiles(float OceanMassVolatiles) {
    _PlanetExtraProperties.OceanMass.Volatiles = boost::multiprecision::uint128_t(OceanMassVolatiles);
    return *this;
}

inline Planet& Planet::SetOceanMassVolatiles(const boost::multiprecision::uint128_t& OceanMassVolatiles) {
    _PlanetExtraProperties.OceanMass.Volatiles = OceanMassVolatiles;
    return *this;
}

inline Planet& Planet::SetOceanMassEnergeticNuclide(float OceanMassEnergeticNuclide) {
    _PlanetExtraProperties.OceanMass.EnergeticNuclide = boost::multiprecision::uint128_t(OceanMassEnergeticNuclide);
    return *this;
}

inline Planet& Planet::SetOceanMassEnergeticNuclide(const boost::multiprecision::uint128_t& OceanMassEnergeticNuclide) {
    _PlanetExtraProperties.OceanMass.EnergeticNuclide = OceanMassEnergeticNuclide;
    return *this;
}

inline float Planet::GetAtmosphereMassFloat() const {
    return ConvertToFloat(GetAtmosphereMass());
}

inline const Planet::ComplexMass& Planet::GetAtmosphereMassStruct() const {
    return _PlanetExtraProperties.AtmosphereMass;
}

inline const boost::multiprecision::uint128_t Planet::GetAtmosphereMass() const {
    return GetAtmosphereMassZ() + GetAtmosphereMassVolatiles() + GetAtmosphereMassEnergeticNuclide();
}

inline float Planet::GetAtmosphereMassZFloat() const {
    return ConvertToFloat(GetAtmosphereMassZ());
}

inline const boost::multiprecision::uint128_t& Planet::GetAtmosphereMassZ() const {
    return _PlanetExtraProperties.AtmosphereMass.Z;
}

inline float Planet::GetAtmosphereMassVolatilesFloat() const {
    return ConvertToFloat(GetAtmosphereMassVolatiles());
}

inline const boost::multiprecision::uint128_t& Planet::GetAtmosphereMassVolatiles() const {
    return _PlanetExtraProperties.AtmosphereMass.Volatiles;
}

inline float Planet::GetAtmosphereMassEnergeticNuclideFloat() const {
    return ConvertToFloat(GetAtmosphereMassEnergeticNuclide());
}

inline const boost::multiprecision::uint128_t& Planet::GetAtmosphereMassEnergeticNuclide() const {
    return _PlanetExtraProperties.AtmosphereMass.EnergeticNuclide;
}

inline float Planet::GetCoreMassFloat() const {
    return ConvertToFloat(GetCoreMass());
}

inline const Planet::ComplexMass& Planet::GetCoreMassStruct() const {
    return _PlanetExtraProperties.CoreMass;
}

inline const boost::multiprecision::uint128_t Planet::GetCoreMass() const {
    return GetCoreMassZ() + GetCoreMassVolatiles() + GetCoreMassEnergeticNuclide();
}

inline float Planet::GetCoreMassZFloat() const {
    return ConvertToFloat(GetCoreMassZ());
}

inline const boost::multiprecision::uint128_t& Planet::GetCoreMassZ() const {
    return _PlanetExtraProperties.CoreMass.Z;
}

inline float Planet::GetCoreMassVolatilesFloat() const {
    return ConvertToFloat(GetCoreMassVolatiles());
}

inline const boost::multiprecision::uint128_t& Planet::GetCoreMassVolatiles() const {
    return _PlanetExtraProperties.CoreMass.Volatiles;
}

inline float Planet::GetCoreMassEnergeticNuclideFloat() const {
    return ConvertToFloat(GetCoreMassEnergeticNuclide());
}

inline const boost::multiprecision::uint128_t& Planet::GetCoreMassEnergeticNuclide() const {
    return _PlanetExtraProperties.CoreMass.EnergeticNuclide;
}

inline float Planet::GetOceanMassFloat() const {
    return ConvertToFloat(GetOceanMass());
}

inline const Planet::ComplexMass& Planet::GetOceanMassStruct() const {
    return _PlanetExtraProperties.OceanMass;
}

inline const boost::multiprecision::uint128_t Planet::GetOceanMass() const {
    return GetOceanMassZ() + GetOceanMassVolatiles() + GetOceanMassEnergeticNuclide();
}

inline float Planet::GetOceanMassZFloat() const {
    return ConvertToFloat(GetOceanMassZ());
}

inline const boost::multiprecision::uint128_t& Planet::GetOceanMassZ() const {
    return _PlanetExtraProperties.OceanMass.Z;
}

inline float Planet::GetOceanMassVolatilesFloat() const {
    return ConvertToFloat(GetOceanMassVolatiles());
}

inline const boost::multiprecision::uint128_t& Planet::GetOceanMassVolatiles() const {
    return _PlanetExtraProperties.OceanMass.Volatiles;
}

inline float Planet::GetOceanMassEnergeticNuclideFloat() const {
    return ConvertToFloat(GetOceanMassEnergeticNuclide());
}

inline const boost::multiprecision::uint128_t& Planet::GetOceanMassEnergeticNuclide() const {
    return _PlanetExtraProperties.OceanMass.EnergeticNuclide;
}

inline float Planet::GetMassFloat() const {
    return ConvertToFloat(GetMass());
}

inline const boost::multiprecision::uint128_t Planet::GetMass() const {
    return GetAtmosphereMass() + GetOceanMass() + GetCoreMass();
}

inline float Planet::GetCrustMineralMassFloat() const {
    return ConvertToFloat(GetCrustMineralMass());
}

inline const boost::multiprecision::uint128_t& Planet::GetCrustMineralMass() const {
    return _PlanetExtraProperties.CrustMineralMass;
}

inline bool Planet::GetMigration() const {
    return _PlanetExtraProperties.bIsMigrated;
}

inline Planet::PlanetType Planet::GetPlanetType() const {
    return _PlanetExtraProperties.Type;
}

inline std::shared_ptr<Civilization>& Planet::CivilizationData() {
    return _PlanetExtraProperties.CivilizationData;
}

_ASTRO_END
_NPGS_END
