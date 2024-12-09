#pragma once

#include "Planet.h"

_NPGS_BEGIN
_ASTRO_BEGIN

NPGS_INLINE Planet& Planet::SetExtendedProperties(ExtendedProperties&& PlanetExtraProperties)
{
	_PlanetExtraProperties = std::move(PlanetExtraProperties);
	return *this;
}

NPGS_INLINE const Planet::ExtendedProperties& Planet::GetExtendedProperties() const
{
	return _PlanetExtraProperties;
}

NPGS_INLINE Planet& Planet::SetAtmosphereMass(const ComplexMass& AtmosphereMass)
{
	_PlanetExtraProperties.AtmosphereMass = AtmosphereMass;
	return *this;
}

NPGS_INLINE Planet& Planet::SetCoreMass(const ComplexMass& CoreMass)
{
	_PlanetExtraProperties.CoreMass = CoreMass;
	return *this;
}

NPGS_INLINE Planet& Planet::SetOceanMass(const ComplexMass& OceanMass)
{
	_PlanetExtraProperties.OceanMass = OceanMass;
	return *this;
}

NPGS_INLINE Planet& Planet::SetCrustMineralMass(float CrustMineralMass)
{
	_PlanetExtraProperties.CrustMineralMass = boost::multiprecision::uint128_t(CrustMineralMass);
	return *this;
}

NPGS_INLINE Planet& Planet::SetCrustMineralMass(const boost::multiprecision::uint128_t& CrustMineralMass)
{
	_PlanetExtraProperties.CrustMineralMass = CrustMineralMass;
	return *this;
}

NPGS_INLINE Planet& Planet::SetBalanceTemperature(float BalanceTemperature)
{
	_PlanetExtraProperties.BalanceTemperature = BalanceTemperature;
	return *this;
}

NPGS_INLINE Planet& Planet::SetMigration(bool bIsMigrated)
{
	_PlanetExtraProperties.bIsMigrated = bIsMigrated;
	return *this;
}

NPGS_INLINE Planet& Planet::SetPlanetType(PlanetType Type)
{
	_PlanetExtraProperties.Type = Type;
	return *this;
}

NPGS_INLINE Planet& Planet::SetAtmosphereMassZ(float AtmosphereMassZ)
{
	_PlanetExtraProperties.AtmosphereMass.Z = boost::multiprecision::uint128_t(AtmosphereMassZ);
	return *this;
}

NPGS_INLINE Planet& Planet::SetAtmosphereMassZ(const boost::multiprecision::uint128_t& AtmosphereMassZ)
{
	_PlanetExtraProperties.AtmosphereMass.Z = AtmosphereMassZ;
	return *this;
}

NPGS_INLINE Planet& Planet::SetAtmosphereMassVolatiles(float AtmosphereMassVolatiles)
{
	_PlanetExtraProperties.AtmosphereMass.Volatiles = boost::multiprecision::uint128_t(AtmosphereMassVolatiles);
	return *this;
}

NPGS_INLINE Planet& Planet::SetAtmosphereMassVolatiles(const boost::multiprecision::uint128_t& AtmosphereMassVolatiles)
{
	_PlanetExtraProperties.AtmosphereMass.Volatiles = AtmosphereMassVolatiles;
	return *this;
}

NPGS_INLINE Planet& Planet::SetAtmosphereMassEnergeticNuclide(float AtmosphereMassEnergeticNuclide)
{
	_PlanetExtraProperties.AtmosphereMass.EnergeticNuclide = boost::multiprecision::uint128_t(AtmosphereMassEnergeticNuclide);
	return *this;
}

NPGS_INLINE Planet& Planet::SetAtmosphereMassEnergeticNuclide(const boost::multiprecision::uint128_t& AtmosphereMassEnergeticNuclide)
{
	_PlanetExtraProperties.AtmosphereMass.EnergeticNuclide = AtmosphereMassEnergeticNuclide;
	return *this;
}

NPGS_INLINE Planet& Planet::SetCoreMassZ(float CoreMassZ)
{
	_PlanetExtraProperties.CoreMass.Z = boost::multiprecision::uint128_t(CoreMassZ);
	return *this;
}

NPGS_INLINE Planet& Planet::SetCoreMassZ(const boost::multiprecision::uint128_t& CoreMassZ)
{
	_PlanetExtraProperties.CoreMass.Z = CoreMassZ;
	return *this;
}

NPGS_INLINE Planet& Planet::SetCoreMassVolatiles(float CoreMassVolatiles)
{
	_PlanetExtraProperties.CoreMass.Volatiles = boost::multiprecision::uint128_t(CoreMassVolatiles);
	return *this;
}

NPGS_INLINE Planet& Planet::SetCoreMassVolatiles(const boost::multiprecision::uint128_t& CoreMassVolatiles)
{
	_PlanetExtraProperties.CoreMass.Volatiles = CoreMassVolatiles;
	return *this;
}

NPGS_INLINE Planet& Planet::SetCoreMassEnergeticNuclide(float CoreMassEnergeticNuclide)
{
	_PlanetExtraProperties.CoreMass.EnergeticNuclide = boost::multiprecision::uint128_t(CoreMassEnergeticNuclide);
	return *this;
}

NPGS_INLINE Planet& Planet::SetCoreMassEnergeticNuclide(const boost::multiprecision::uint128_t& CoreMassEnergeticNuclide)
{
	_PlanetExtraProperties.CoreMass.EnergeticNuclide = CoreMassEnergeticNuclide;
	return *this;
}

NPGS_INLINE Planet& Planet::SetOceanMassZ(float OceanMassZ)
{
	_PlanetExtraProperties.OceanMass.Z = boost::multiprecision::uint128_t(OceanMassZ);
	return *this;
}

NPGS_INLINE Planet& Planet::SetOceanMassZ(const boost::multiprecision::uint128_t& OceanMassZ)
{
	_PlanetExtraProperties.OceanMass.Z = OceanMassZ;
	return *this;
}

NPGS_INLINE Planet& Planet::SetOceanMassVolatiles(float OceanMassVolatiles)
{
	_PlanetExtraProperties.OceanMass.Volatiles = boost::multiprecision::uint128_t(OceanMassVolatiles);
	return *this;
}

NPGS_INLINE Planet& Planet::SetOceanMassVolatiles(const boost::multiprecision::uint128_t& OceanMassVolatiles)
{
	_PlanetExtraProperties.OceanMass.Volatiles = OceanMassVolatiles;
	return *this;
}

NPGS_INLINE Planet& Planet::SetOceanMassEnergeticNuclide(float OceanMassEnergeticNuclide)
{
	_PlanetExtraProperties.OceanMass.EnergeticNuclide = boost::multiprecision::uint128_t(OceanMassEnergeticNuclide);
	return *this;
}

NPGS_INLINE Planet& Planet::SetOceanMassEnergeticNuclide(const boost::multiprecision::uint128_t& OceanMassEnergeticNuclide)
{
	_PlanetExtraProperties.OceanMass.EnergeticNuclide = OceanMassEnergeticNuclide;
	return *this;
}

NPGS_INLINE const ComplexMass& Planet::GetAtmosphereMassStruct() const
{
	return _PlanetExtraProperties.AtmosphereMass;
}

NPGS_INLINE const boost::multiprecision::uint128_t Planet::GetAtmosphereMass() const
{
	return GetAtmosphereMassZ() + GetAtmosphereMassVolatiles() + GetAtmosphereMassEnergeticNuclide();
}

NPGS_INLINE const boost::multiprecision::uint128_t& Planet::GetAtmosphereMassZ() const
{
	return _PlanetExtraProperties.AtmosphereMass.Z;
}

NPGS_INLINE const boost::multiprecision::uint128_t& Planet::GetAtmosphereMassVolatiles() const
{
	return _PlanetExtraProperties.AtmosphereMass.Volatiles;
}

NPGS_INLINE const boost::multiprecision::uint128_t& Planet::GetAtmosphereMassEnergeticNuclide() const
{
	return _PlanetExtraProperties.AtmosphereMass.EnergeticNuclide;
}

NPGS_INLINE const ComplexMass& Planet::GetCoreMassStruct() const
{
	return _PlanetExtraProperties.CoreMass;
}

NPGS_INLINE const boost::multiprecision::uint128_t Planet::GetCoreMass() const
{
	return GetCoreMassZ() + GetCoreMassVolatiles() + GetCoreMassEnergeticNuclide();
}

NPGS_INLINE const boost::multiprecision::uint128_t& Planet::GetCoreMassZ() const
{
	return _PlanetExtraProperties.CoreMass.Z;
}

NPGS_INLINE const boost::multiprecision::uint128_t& Planet::GetCoreMassVolatiles() const
{
	return _PlanetExtraProperties.CoreMass.Volatiles;
}

NPGS_INLINE const boost::multiprecision::uint128_t& Planet::GetCoreMassEnergeticNuclide() const
{
	return _PlanetExtraProperties.CoreMass.EnergeticNuclide;
}

NPGS_INLINE const ComplexMass& Planet::GetOceanMassStruct() const
{
	return _PlanetExtraProperties.OceanMass;
}

NPGS_INLINE const boost::multiprecision::uint128_t Planet::GetOceanMass() const
{
	return GetOceanMassZ() + GetOceanMassVolatiles() + GetOceanMassEnergeticNuclide();
}

NPGS_INLINE const boost::multiprecision::uint128_t& Planet::GetOceanMassZ() const
{
	return _PlanetExtraProperties.OceanMass.Z;
}

NPGS_INLINE const boost::multiprecision::uint128_t& Planet::GetOceanMassVolatiles() const
{
	return _PlanetExtraProperties.OceanMass.Volatiles;
}

NPGS_INLINE const boost::multiprecision::uint128_t& Planet::GetOceanMassEnergeticNuclide() const
{
	return _PlanetExtraProperties.OceanMass.EnergeticNuclide;
}

NPGS_INLINE const boost::multiprecision::uint128_t Planet::GetMass() const
{
	return GetAtmosphereMass() + GetOceanMass() + GetCoreMass() + GetCrustMineralMass();
}

NPGS_INLINE const boost::multiprecision::uint128_t& Planet::GetCrustMineralMass() const
{
	return _PlanetExtraProperties.CrustMineralMass;
}

NPGS_INLINE float Planet::GetBalanceTemperature() const
{
	return _PlanetExtraProperties.BalanceTemperature;
}

NPGS_INLINE bool Planet::GetMigration() const
{
	return _PlanetExtraProperties.bIsMigrated;
}

NPGS_INLINE Planet::PlanetType Planet::GetPlanetType() const
{
	return _PlanetExtraProperties.Type;
}

template <typename T>
NPGS_INLINE T Planet::GetAtmosphereMassDigital() const
{
	return GetAtmosphereMass().convert_to<T>();
}

template <typename T>
NPGS_INLINE T Planet::GetAtmosphereMassZDigital() const
{
	return GetAtmosphereMassZ().convert_to<T>();
}

template <typename T>
NPGS_INLINE T Planet::GetAtmosphereMassVolatilesDigital() const
{
	return GetAtmosphereMassVolatiles().convert_to<T>();
}

template <typename T>
NPGS_INLINE T Planet::GetAtmosphereMassEnergeticNuclideDigital() const
{
	return GetAtmosphereMassEnergeticNuclide().convert_to<T>();
}

template <typename T>
NPGS_INLINE T Planet::GetCoreMassDigital() const
{
	return GetCoreMass().convert_to<T>();
}

template <typename T>
NPGS_INLINE T Planet::GetCoreMassZDigital() const
{
	return GetCoreMassZ().convert_to<T>();
}

template <typename T>
NPGS_INLINE T Planet::GetCoreMassVolatilesDigital() const
{
	return GetCoreMassVolatiles().convert_to<T>();
}

template <typename T>
NPGS_INLINE T Planet::GetCoreMassEnergeticNuclideDigital() const
{
	return GetCoreMassEnergeticNuclide().convert_to<T>();
}

template <typename T>
NPGS_INLINE T Planet::GetOceanMassDigital() const
{
	return GetOceanMass().convert_to<T>();
}

template <typename T>
NPGS_INLINE T Planet::GetOceanMassZDigital() const
{
	return GetOceanMassZ().convert_to<T>();
}

template <typename T>
NPGS_INLINE T Planet::GetOceanMassVolatilesDigital() const
{
	return GetOceanMassVolatiles().convert_to<T>();
}

template <typename T>
NPGS_INLINE T Planet::GetOceanMassEnergeticNuclideDigital() const
{
	return GetOceanMassEnergeticNuclide().convert_to<T>();
}

template <typename T>
NPGS_INLINE T Planet::GetMassDigital() const
{
	return GetMass().convert_to<T>();
}

template <typename T>
NPGS_INLINE T Planet::GetCrustMineralMassDigital() const
{
	return GetCrustMineralMass().convert_to<T>();
}

NPGS_INLINE std::unique_ptr<Intelli::Standard>& Planet::CivilizationData()
{
	return _PlanetExtraProperties.CivilizationData;
}

NPGS_INLINE AsteroidCluster& AsteroidCluster::SetMass(const ComplexMass& Mass)
{
	_Properties.Mass = Mass;
	return *this;
}

NPGS_INLINE AsteroidCluster& AsteroidCluster::SetMassZ(float MassZ)
{
	_Properties.Mass.Z = boost::multiprecision::uint128_t(MassZ);
	return *this;
}

NPGS_INLINE AsteroidCluster& AsteroidCluster::SetMassZ(const boost::multiprecision::uint128_t& MassZ)
{
	_Properties.Mass.Z = MassZ;
	return *this;
}

NPGS_INLINE AsteroidCluster& AsteroidCluster::SetMassVolatiles(float MassVolatiles)
{
	_Properties.Mass.Volatiles = boost::multiprecision::uint128_t(MassVolatiles);
	return *this;
}

NPGS_INLINE AsteroidCluster& AsteroidCluster::SetMassVolatiles(const boost::multiprecision::uint128_t& MassVolatiles)
{
	_Properties.Mass.Volatiles = MassVolatiles;
	return *this;
}

NPGS_INLINE AsteroidCluster& AsteroidCluster::SetMassEnergeticNuclide(float MassEnergeticNuclide)
{
	_Properties.Mass.EnergeticNuclide = boost::multiprecision::uint128_t(MassEnergeticNuclide);
	return *this;
}

NPGS_INLINE AsteroidCluster& AsteroidCluster::SetMassEnergeticNuclide(const boost::multiprecision::uint128_t& MassEnergeticNuclide)
{
	_Properties.Mass.EnergeticNuclide = MassEnergeticNuclide;
	return *this;
}

NPGS_INLINE AsteroidCluster& AsteroidCluster::SetAsteroidType(AsteroidType Type)
{
	_Properties.Type = Type;
	return *this;
}

NPGS_INLINE const boost::multiprecision::uint128_t AsteroidCluster::GetMass() const
{
	return GetMassZ() + GetMassVolatiles() + GetMassEnergeticNuclide();
}

NPGS_INLINE const boost::multiprecision::uint128_t& AsteroidCluster::GetMassZ() const
{
	return _Properties.Mass.Z;
}

NPGS_INLINE const boost::multiprecision::uint128_t& AsteroidCluster::GetMassVolatiles() const
{
	return _Properties.Mass.Volatiles;
}

NPGS_INLINE const boost::multiprecision::uint128_t& AsteroidCluster::GetMassEnergeticNuclide() const
{
	return _Properties.Mass.EnergeticNuclide;
}

NPGS_INLINE AsteroidCluster::AsteroidType AsteroidCluster::GetAsteroidType() const
{
	return _Properties.Type;
}

template <typename T>
NPGS_INLINE T AsteroidCluster::GetMassDigital() const
{
	return GetMass().convert_to<T>();
}

template <typename T>
NPGS_INLINE T AsteroidCluster::GetMassZDigital() const
{
	return GetMassZ().convert_to<T>();
}

template <typename T>
NPGS_INLINE T AsteroidCluster::GetMassVolatilesDigital() const
{
	return GetMassVolatiles().convert_to<T>();
}

template <typename T>
NPGS_INLINE T AsteroidCluster::GetMassEnergeticNuclideDigital() const
{
	return GetMassEnergeticNuclide().convert_to<T>();
}

_ASTRO_END
_NPGS_END
