#pragma once

#include <memory>
#include <boost/multiprecision/cpp_int.hpp>
#include "Engine/Base/Astro/CelestialObject.h"
#include "Engine/Base/Civilization.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_ASTRO_BEGIN

struct ComplexMass
{
	boost::multiprecision::uint128_t Z;
	boost::multiprecision::uint128_t Volatiles;
	boost::multiprecision::uint128_t EnergeticNuclide;
};

class Planet : public CelestialBody
{
public:
	enum class PlanetType : int
	{
		kRocky                            = 0,
		kTerra                            = 1,
		kIcePlanet                        = 2,
		kChthonian                        = 3,
		kOceanic                          = 4,
		kSubIceGiant                      = 5,
		kIceGiant                         = 6,
		kGasGiant                         = 7,
		kHotSubIceGiant                   = 8,
		kHotIceGiant                      = 9,
		kHotGasGiant                      = 10,
		kRockyAsteroidCluster             = 11,
		kRockyIceAsteroidCluster          = 12,
		kArtificalOrbitalStructureCluster = 13
	};

	struct ExtendedProperties
	{
		ComplexMass AtmosphereMass;                        // 大气层质量，单位 kg
		ComplexMass CoreMass;                              // 核心质量，单位 kg
		ComplexMass OceanMass;                             // 海洋质量，单位 kg
		boost::multiprecision::uint128_t CrustMineralMass; // 地壳矿脉质量，单位 kg
		std::unique_ptr<Civilization>    CivilizationData; // 文明数据
		PlanetType Type{ PlanetType::kRocky };             // 行星类型
		float BalanceTemperature{};                        // 平衡温度，单位 K
		bool  bIsMigrated{ false };                        // 是否为迁移行星
	};

public:
	Planet() = default;
	Planet(const CelestialBody::BasicProperties& PlanetBasicProperties, ExtendedProperties&& PlanetExtraProperties);
	Planet(const Planet&) = delete;
	Planet(Planet&&) = default;
	~Planet() = default;

	Planet& operator=(const Planet&) = delete;
	Planet& operator=(Planet&&) = default;

	Planet& SetExtendedProperties(ExtendedProperties&& PlanetExtraProperties);
	const ExtendedProperties& GetExtendedProperties() const;

	// Setters
	// Setters for ExtendedProperties
	// ------------------------------
	Planet& SetAtmosphereMass(const ComplexMass& AtmosphereMass);
	Planet& SetCoreMass(const ComplexMass& CoreMass);
	Planet& SetOceanMass(const ComplexMass& OceanMass);
	Planet& SetCrustMineralMass(float CrustMineralMass);
	Planet& SetCrustMineralMass(const boost::multiprecision::uint128_t& CrustMineralMass);
	Planet& SetBalanceTemperature(float BalanceTemperature);
	Planet& SetMigration(bool bIsMigrated);
	Planet& SetPlanetType(PlanetType Type);

	// Setters for every mass property
	// -------------------------------
	Planet& SetAtmosphereMassZ(float AtmosphereMassZ);
	Planet& SetAtmosphereMassZ(const boost::multiprecision::uint128_t& AtmosphereMassZ);
	Planet& SetAtmosphereMassVolatiles(float AtmosphereMassVolatiles);
	Planet& SetAtmosphereMassVolatiles(const boost::multiprecision::uint128_t& AtmosphereMassVolatiles);
	Planet& SetAtmosphereMassEnergeticNuclide(float AtmosphereMassEnergeticNuclide);
	Planet& SetAtmosphereMassEnergeticNuclide(const boost::multiprecision::uint128_t& AtmosphereMassEnergeticNuclide);
	Planet& SetCoreMassZ(float CoreMassZ);
	Planet& SetCoreMassZ(const boost::multiprecision::uint128_t& CoreMassZ);
	Planet& SetCoreMassVolatiles(float CoreMassVolatiles);
	Planet& SetCoreMassVolatiles(const boost::multiprecision::uint128_t& CoreMassVolatiles);
	Planet& SetCoreMassEnergeticNuclide(float CoreMassEnergeticNuclide);
	Planet& SetCoreMassEnergeticNuclide(const boost::multiprecision::uint128_t& CoreMassEnergeticNuclide);
	Planet& SetOceanMassZ(float OceanMassZ);
	Planet& SetOceanMassZ(const boost::multiprecision::uint128_t& OceanMassZ);
	Planet& SetOceanMassVolatiles(float OceanMassVolatiles);
	Planet& SetOceanMassVolatiles(const boost::multiprecision::uint128_t& OceanMassVolatiles);
	Planet& SetOceanMassEnergeticNuclide(float OceanMassEnergeticNuclide);
	Planet& SetOceanMassEnergeticNuclide(const boost::multiprecision::uint128_t& OceanMassEnergeticNuclide);

	// Getters
	// Getters for ExtendedProperties
	// ------------------------------
	const ComplexMass& GetAtmosphereMassStruct() const;
	const boost::multiprecision::uint128_t  GetAtmosphereMass() const;
	const boost::multiprecision::uint128_t& GetAtmosphereMassZ() const;
	const boost::multiprecision::uint128_t& GetAtmosphereMassVolatiles() const;
	const boost::multiprecision::uint128_t& GetAtmosphereMassEnergeticNuclide() const;
	const ComplexMass& GetCoreMassStruct() const;
	const boost::multiprecision::uint128_t  GetCoreMass() const;
	const boost::multiprecision::uint128_t& GetCoreMassZ() const;
	const boost::multiprecision::uint128_t& GetCoreMassVolatiles() const;
	const boost::multiprecision::uint128_t& GetCoreMassEnergeticNuclide() const;
	const ComplexMass& GetOceanMassStruct() const;
	const boost::multiprecision::uint128_t  GetOceanMass() const;
	const boost::multiprecision::uint128_t& GetOceanMassZ() const;
	const boost::multiprecision::uint128_t& GetOceanMassVolatiles() const;
	const boost::multiprecision::uint128_t& GetOceanMassEnergeticNuclide() const;
	const boost::multiprecision::uint128_t  GetMass() const;
	const boost::multiprecision::uint128_t& GetCrustMineralMass() const;
	float GetBalanceTemperature() const;
	bool  GetMigration() const;
	PlanetType GetPlanetType() const;

	template <typename T>
	T GetAtmosphereMassDigital() const;

	template <typename T>
	T GetAtmosphereMassZDigital() const;
	
	template <typename T>
	T GetAtmosphereMassVolatilesDigital() const;
	
	template <typename T>
	T GetAtmosphereMassEnergeticNuclideDigital() const;
	
	template <typename T>
	T GetCoreMassDigital() const;
	
	template <typename T>
	T GetCoreMassZDigital() const;
	
	template <typename T>
	T GetCoreMassVolatilesDigital() const;
	
	template <typename T>
	T GetCoreMassEnergeticNuclideDigital() const;
	
	template <typename T>
	T GetOceanMassDigital() const;
	
	template <typename T>
	T GetOceanMassZDigital() const;
	
	template <typename T>
	T GetOceanMassVolatilesDigital() const;
	
	template <typename T>
	T GetOceanMassEnergeticNuclideDigital() const;
	
	template <typename T>
	T GetMassDigital() const;
	
	template <typename T>
	T GetCrustMineralMassDigital() const;

	std::unique_ptr<Civilization>& CivilizationData();

private:
	ExtendedProperties _PlanetExtraProperties{};
};

class AsteroidCluster : public AstroObject
{
public:
	enum class AsteroidType
	{
		kRocky,
		kRockyIce
	};

	struct BasicProperties
	{
		ComplexMass  Mass;
		AsteroidType Type{ AsteroidType::kRocky };
	};

public:
	AsteroidCluster() = default;
	AsteroidCluster(const BasicProperties& Properties);
	~AsteroidCluster() = default;

	// Setters
	// Setters for BasicProperties
	// ---------------------------
	AsteroidCluster& SetMass(const ComplexMass& Mass);

	// Setters for every mass property
	// -------------------------------
	AsteroidCluster& SetMassZ(float MassZ);
	AsteroidCluster& SetMassZ(const boost::multiprecision::uint128_t& MassZ);
	AsteroidCluster& SetMassVolatiles(float MassVolatiles);
	AsteroidCluster& SetMassVolatiles(const boost::multiprecision::uint128_t& MassVolatiles);
	AsteroidCluster& SetMassEnergeticNuclide(float MassEnergeticNuclide);
	AsteroidCluster& SetMassEnergeticNuclide(const boost::multiprecision::uint128_t& MassEnergeticNuclide);
	AsteroidCluster& SetAsteroidType(AsteroidType Type);

	// Getters
	// Getters for BasicProperties
	// ---------------------------
	const boost::multiprecision::uint128_t  GetMass() const;
	const boost::multiprecision::uint128_t& GetMassZ() const;
	const boost::multiprecision::uint128_t& GetMassVolatiles() const;
	const boost::multiprecision::uint128_t& GetMassEnergeticNuclide() const;
	AsteroidType GetAsteroidType() const;

	template <typename T>
	T GetMassDigital() const;

	template <typename T>
	T GetMassZDigital() const;
	
	template <typename T>
	T GetMassVolatilesDigital() const;
	
	template <typename T>
	T GetMassEnergeticNuclideDigital() const;

private:
	BasicProperties _Properties{};
};

_ASTRO_END
_NPGS_END

#include "Planet.inl"
