#pragma once

#include "Planet.h"

_NPGS_BEGIN
_ASTRO_BEGIN

inline Planet& Planet::SetExtendedProperties(ExtendedProperties&& PlanetExtraProperties)
{
	_PlanetExtraProperties = std::move(PlanetExtraProperties);
	return *this;
}

inline const Planet::ExtendedProperties& Planet::GetExtendedProperties() const
{
	return _PlanetExtraProperties;
}

inline Planet& Planet::SetAtmosphereMass(const ComplexMass& AtmosphereMass)
{
	_PlanetExtraProperties.AtmosphereMass = AtmosphereMass;
	return *this;
}

inline Planet& Planet::SetCoreMass(const ComplexMass& CoreMass)
{
	_PlanetExtraProperties.CoreMass = CoreMass;
	return *this;
}

inline Planet& Planet::SetOceanMass(const ComplexMass& OceanMass)
{
	_PlanetExtraProperties.OceanMass = OceanMass;
	return *this;
}

inline Planet& Planet::SetCrustMineralMass(float CrustMineralMass)
{
	_PlanetExtraProperties.CrustMineralMass = boost::multiprecision::uint128_t(CrustMineralMass);
	return *this;
}

inline Planet& Planet::SetCrustMineralMass(const boost::multiprecision::uint128_t& CrustMineralMass)
{
	_PlanetExtraProperties.CrustMineralMass = CrustMineralMass;
	return *this;
}

inline Planet& Planet::SetBalanceTemperature(float BalanceTemperature)
{
	_PlanetExtraProperties.BalanceTemperature = BalanceTemperature;
	return *this;
}

inline Planet& Planet::SetMigration(bool bIsMigrated)
{
	_PlanetExtraProperties.bIsMigrated = bIsMigrated;
	return *this;
}

inline Planet& Planet::SetPlanetType(PlanetType Type)
{
	_PlanetExtraProperties.Type = Type;
	return *this;
}

inline Planet& Planet::SetAtmosphereMassZ(float AtmosphereMassZ)
{
	_PlanetExtraProperties.AtmosphereMass.Z = boost::multiprecision::uint128_t(AtmosphereMassZ);
	return *this;
}

inline Planet& Planet::SetAtmosphereMassZ(const boost::multiprecision::uint128_t& AtmosphereMassZ)
{
	_PlanetExtraProperties.AtmosphereMass.Z = AtmosphereMassZ;
	return *this;
}

inline Planet& Planet::SetAtmosphereMassVolatiles(float AtmosphereMassVolatiles)
{
	_PlanetExtraProperties.AtmosphereMass.Volatiles = boost::multiprecision::uint128_t(AtmosphereMassVolatiles);
	return *this;
}

inline Planet& Planet::SetAtmosphereMassVolatiles(const boost::multiprecision::uint128_t& AtmosphereMassVolatiles)
{
	_PlanetExtraProperties.AtmosphereMass.Volatiles = AtmosphereMassVolatiles;
	return *this;
}

inline Planet& Planet::SetAtmosphereMassEnergeticNuclide(float AtmosphereMassEnergeticNuclide)
{
	_PlanetExtraProperties.AtmosphereMass.EnergeticNuclide = boost::multiprecision::uint128_t(AtmosphereMassEnergeticNuclide);
	return *this;
}

inline Planet& Planet::SetAtmosphereMassEnergeticNuclide(const boost::multiprecision::uint128_t& AtmosphereMassEnergeticNuclide)
{
	_PlanetExtraProperties.AtmosphereMass.EnergeticNuclide = AtmosphereMassEnergeticNuclide;
}

inline Planet& Planet::SetCoreMassZ(float CoreMassZ)
{
	_PlanetExtraProperties.CoreMass.Z = boost::multiprecision::uint128_t(CoreMassZ);
	return *this;
}

inline Planet& Planet::SetCoreMassZ(const boost::multiprecision::uint128_t& CoreMassZ)
{
	_PlanetExtraProperties.CoreMass.Z = CoreMassZ;
	return *this;
}

inline Planet& Planet::SetCoreMassVolatiles(float CoreMassVolatiles)
{
	_PlanetExtraProperties.CoreMass.Volatiles = boost::multiprecision::uint128_t(CoreMassVolatiles);
	return *this;
}

inline Planet& Planet::SetCoreMassVolatiles(const boost::multiprecision::uint128_t& CoreMassVolatiles)
{
	_PlanetExtraProperties.CoreMass.Volatiles = CoreMassVolatiles;
	return *this;
}

inline Planet& Planet::SetCoreMassEnergeticNuclide(float CoreMassEnergeticNuclide)
{
	_PlanetExtraProperties.CoreMass.EnergeticNuclide = boost::multiprecision::uint128_t(CoreMassEnergeticNuclide);
	return *this;
}

inline Planet& Planet::SetCoreMassEnergeticNuclide(const boost::multiprecision::uint128_t& CoreMassEnergeticNuclide)
{
	_PlanetExtraProperties.CoreMass.EnergeticNuclide = CoreMassEnergeticNuclide;
	return *this;
}

inline Planet& Planet::SetOceanMassZ(float OceanMassZ)
{
	_PlanetExtraProperties.OceanMass.Z = boost::multiprecision::uint128_t(OceanMassZ);
	return *this;
}

inline Planet& Planet::SetOceanMassZ(const boost::multiprecision::uint128_t& OceanMassZ)
{
	_PlanetExtraProperties.OceanMass.Z = OceanMassZ;
	return *this;
}

inline Planet& Planet::SetOceanMassVolatiles(float OceanMassVolatiles)
{
	_PlanetExtraProperties.OceanMass.Volatiles = boost::multiprecision::uint128_t(OceanMassVolatiles);
	return *this;
}

inline Planet& Planet::SetOceanMassVolatiles(const boost::multiprecision::uint128_t& OceanMassVolatiles)
{
	_PlanetExtraProperties.OceanMass.Volatiles = OceanMassVolatiles;
	return *this;
}

inline Planet& Planet::SetOceanMassEnergeticNuclide(float OceanMassEnergeticNuclide)
{
	_PlanetExtraProperties.OceanMass.EnergeticNuclide = boost::multiprecision::uint128_t(OceanMassEnergeticNuclide);
	return *this;
}

inline Planet& Planet::SetOceanMassEnergeticNuclide(const boost::multiprecision::uint128_t& OceanMassEnergeticNuclide)
{
	_PlanetExtraProperties.OceanMass.EnergeticNuclide = OceanMassEnergeticNuclide;
	return *this;
}

inline const ComplexMass& Planet::GetAtmosphereMassStruct() const
{
	return _PlanetExtraProperties.AtmosphereMass;
}

inline const boost::multiprecision::uint128_t Planet::GetAtmosphereMass() const
{
	return GetAtmosphereMassZ() + GetAtmosphereMassVolatiles() + GetAtmosphereMassEnergeticNuclide();
}

inline const boost::multiprecision::uint128_t& Planet::GetAtmosphereMassZ() const
{
	return _PlanetExtraProperties.AtmosphereMass.Z;
}

inline const boost::multiprecision::uint128_t& Planet::GetAtmosphereMassVolatiles() const
{
	return _PlanetExtraProperties.AtmosphereMass.Volatiles;
}

inline const boost::multiprecision::uint128_t& Planet::GetAtmosphereMassEnergeticNuclide() const
{
	return _PlanetExtraProperties.AtmosphereMass.EnergeticNuclide;
}

inline const ComplexMass& Planet::GetCoreMassStruct() const
{
	return _PlanetExtraProperties.CoreMass;
}

inline const boost::multiprecision::uint128_t Planet::GetCoreMass() const
{
	return GetCoreMassZ() + GetCoreMassVolatiles() + GetCoreMassEnergeticNuclide();
}

inline const boost::multiprecision::uint128_t& Planet::GetCoreMassZ() const
{
	return _PlanetExtraProperties.CoreMass.Z;
}

inline const boost::multiprecision::uint128_t& Planet::GetCoreMassVolatiles() const
{
	return _PlanetExtraProperties.CoreMass.Volatiles;
}

inline const boost::multiprecision::uint128_t& Planet::GetCoreMassEnergeticNuclide() const
{
	return _PlanetExtraProperties.CoreMass.EnergeticNuclide;
}

inline const ComplexMass& Planet::GetOceanMassStruct() const
{
	return _PlanetExtraProperties.OceanMass;
}

inline const boost::multiprecision::uint128_t Planet::GetOceanMass() const
{
	return GetOceanMassZ() + GetOceanMassVolatiles() + GetOceanMassEnergeticNuclide();
}

inline const boost::multiprecision::uint128_t& Planet::GetOceanMassZ() const
{
	return _PlanetExtraProperties.OceanMass.Z;
}

inline const boost::multiprecision::uint128_t& Planet::GetOceanMassVolatiles() const
{
	return _PlanetExtraProperties.OceanMass.Volatiles;
}

inline const boost::multiprecision::uint128_t& Planet::GetOceanMassEnergeticNuclide() const
{
	return _PlanetExtraProperties.OceanMass.EnergeticNuclide;
}

inline const boost::multiprecision::uint128_t Planet::GetMass() const
{
	return GetAtmosphereMass() + GetOceanMass() + GetCoreMass() + GetCrustMineralMass();
}

inline const boost::multiprecision::uint128_t& Planet::GetCrustMineralMass() const
{
	return _PlanetExtraProperties.CrustMineralMass;
}

inline float Planet::GetBalanceTemperature() const
{
	return _PlanetExtraProperties.BalanceTemperature;
}

inline bool Planet::GetMigration() const
{
	return _PlanetExtraProperties.bIsMigrated;
}

inline Planet::PlanetType Planet::GetPlanetType() const
{
	return _PlanetExtraProperties.Type;
}

template <typename T>
inline T Planet::GetAtmosphereMassDigital() const
{
	return GetAtmosphereMass().convert_to<T>();
}

template <typename T>
inline T Planet::GetAtmosphereMassZDigital() const
{
	return GetAtmosphereMassZ().convert_to<T>();
}

template <typename T>
inline T Planet::GetAtmosphereMassVolatilesDigital() const
{
	return GetAtmosphereMassVolatiles().convert_to<T>();
}

template <typename T>
inline T Planet::GetAtmosphereMassEnergeticNuclideDigital() const
{
	return GetAtmosphereMassEnergeticNuclide().convert_to<T>();
}

template <typename T>
inline T Planet::GetCoreMassDigital() const
{
	return GetCoreMass().convert_to<T>();
}

template <typename T>
inline T Planet::GetCoreMassZDigital() const
{
	return GetCoreMassZ().convert_to<T>();
}

template <typename T>
inline T Planet::GetCoreMassVolatilesDigital() const
{
	return GetCoreMassVolatiles().convert_to<T>();
}

template <typename T>
inline T Planet::GetCoreMassEnergeticNuclideDigital() const
{
	return GetCoreMassEnergeticNuclide().convert_to<T>();
}

template <typename T>
inline T Planet::GetOceanMassDigital() const
{
	return GetOceanMass().convert_to<T>();
}

template <typename T>
inline T Planet::GetOceanMassZDigital() const
{
	return GetOceanMassZ().convert_to<T>();
}

template <typename T>
inline T Planet::GetOceanMassVolatilesDigital() const
{
	return GetOceanMassVolatiles().convert_to<T>();
}

template <typename T>
inline T Planet::GetOceanMassEnergeticNuclideDigital() const
{
	return GetOceanMassEnergeticNuclide().convert_to<T>();
}

template <typename T>
inline T Planet::GetMassDigital() const
{
	return GetMass().convert_to<T>();
}

template <typename T>
inline T Planet::GetCrustMineralMassDigital() const
{
	return GetCrustMineralMass().convert_to<T>();
}

inline std::unique_ptr<Civilization>& Planet::CivilizationData()
{
	return _PlanetExtraProperties.CivilizationData;
}

inline AsteroidCluster& AsteroidCluster::SetMass(const ComplexMass& Mass)
{
	_Properties.Mass = Mass;
	return *this;
}

inline AsteroidCluster& AsteroidCluster::SetMassZ(float MassZ)
{
	_Properties.Mass.Z = boost::multiprecision::uint128_t(MassZ);
	return *this;
}

inline AsteroidCluster& AsteroidCluster::SetMassZ(const boost::multiprecision::uint128_t& MassZ)
{
	_Properties.Mass.Z = MassZ;
	return *this;
}

inline AsteroidCluster& AsteroidCluster::SetMassVolatiles(float MassVolatiles)
{
	_Properties.Mass.Volatiles = boost::multiprecision::uint128_t(MassVolatiles);
	return *this;
}

inline AsteroidCluster& AsteroidCluster::SetMassVolatiles(const boost::multiprecision::uint128_t& MassVolatiles)
{
	_Properties.Mass.Volatiles = MassVolatiles;
	return *this;
}

inline AsteroidCluster& AsteroidCluster::SetMassEnergeticNuclide(float MassEnergeticNuclide)
{
	_Properties.Mass.EnergeticNuclide = boost::multiprecision::uint128_t(MassEnergeticNuclide);
	return *this;
}

inline AsteroidCluster& AsteroidCluster::SetMassEnergeticNuclide(const boost::multiprecision::uint128_t& MassEnergeticNuclide)
{
	_Properties.Mass.EnergeticNuclide = MassEnergeticNuclide;
	return *this;
}

inline AsteroidCluster& AsteroidCluster::SetAsteroidType(AsteroidType Type)
{
	_Properties.Type = Type;
	return *this;
}

inline const boost::multiprecision::uint128_t AsteroidCluster::GetMass() const
{
	return GetMassZ() + GetMassVolatiles() + GetMassEnergeticNuclide();
}

inline const boost::multiprecision::uint128_t& AsteroidCluster::GetMassZ() const
{
	return _Properties.Mass.Z;
}

inline const boost::multiprecision::uint128_t& AsteroidCluster::GetMassVolatiles() const
{
	return _Properties.Mass.Volatiles;
}

inline const boost::multiprecision::uint128_t& AsteroidCluster::GetMassEnergeticNuclide() const
{
	return _Properties.Mass.EnergeticNuclide;
}

inline AsteroidCluster::AsteroidType AsteroidCluster::GetAsteroidType() const
{
	return _Properties.Type;
}

template <typename T>
inline T AsteroidCluster::GetMassDigital() const
{
	return GetMass().convert_to<T>();
}

template <typename T>
inline T AsteroidCluster::GetMassZDigital() const
{
	return GetMassZ().convert_to<T>();
}

template <typename T>
inline T AsteroidCluster::GetMassVolatilesDigital() const
{
	return GetMassVolatiles().convert_to<T>();
}

template <typename T>
inline T AsteroidCluster::GetMassEnergeticNuclideDigital() const
{
	return GetMassEnergeticNuclide().convert_to<T>();
}

_ASTRO_END
_NPGS_END
