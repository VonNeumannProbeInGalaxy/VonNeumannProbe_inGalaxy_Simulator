#pragma once

#include <memory>
#include <boost/multiprecision/cpp_int.hpp>

#include "Engine/Base/NpgsObject/Astro/CelestialObject.h"
#include "Engine/Base/NpgsObject/Civilization.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_ASTRO_BEGIN

class NPGS_API Planet : public CelestialBody {
public:
    enum class PlanetType : int {
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

    struct ComplexMass {
        boost::multiprecision::uint128_t Z;
        boost::multiprecision::uint128_t Volatiles;
        boost::multiprecision::uint128_t EnergeticNuclide;
    };

    struct ExtendedProperties {
        ComplexMass AtmosphereMass;
        ComplexMass CoreMass;
        ComplexMass OceanMass;
        boost::multiprecision::uint128_t CrustMineralMass;
        bool bIsMigrated;

        PlanetType Type;
        std::shared_ptr<Civilization> CivilizationData;
    };

public:
    Planet() = default;
    Planet(const CelestialBody::BasicProperties& PlanetBasicProperties, const ExtendedProperties& PlanetExtraProperties);
    ~Planet() = default;

    Planet& SetExtendedProperties(const ExtendedProperties& PlanetExtraProperties);
    const ExtendedProperties& GetExtendedProperties() const;

    // Setters
    // Setters for ExtendedProperties
    // ------------------------------
    Planet& SetAtmosphereMass(const ComplexMass& AtmosphereMass);
    Planet& SetCoreMass(const ComplexMass& CoreMass);
    Planet& SetOceanMass(const ComplexMass& OceanMass);
    Planet& SetCrustMineralMass(float CrustMineralMass);
    Planet& SetCrustMineralMass(const boost::multiprecision::uint128_t& CrustMineralMass);
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
    float GetAtmosphereMassFloat() const;
    const ComplexMass& GetAtmosphereMassStruct() const;
    const boost::multiprecision::uint128_t GetAtmosphereMass() const;
    float GetAtmosphereMassZFloat() const;
    const boost::multiprecision::uint128_t& GetAtmosphereMassZ() const;
    float GetAtmosphereMassVolatilesFloat() const;
    const boost::multiprecision::uint128_t& GetAtmosphereMassVolatiles() const;
    float GetAtmosphereMassEnergeticNuclideFloat() const;
    const boost::multiprecision::uint128_t& GetAtmosphereMassEnergeticNuclide() const;
    float GetCoreMassFloat() const;
    const ComplexMass& GetCoreMassStruct() const;
    const boost::multiprecision::uint128_t GetCoreMass() const;
    float GetCoreMassZFloat() const;
    const boost::multiprecision::uint128_t& GetCoreMassZ() const;
    float GetCoreMassVolatilesFloat() const;
    const boost::multiprecision::uint128_t& GetCoreMassVolatiles() const;
    float GetCoreMassEnergeticNuclideFloat() const;
    const boost::multiprecision::uint128_t& GetCoreMassEnergeticNuclide() const;
    float GetOceanMassFloat() const;
    const ComplexMass& GetOceanMassStruct() const;
    const boost::multiprecision::uint128_t GetOceanMass() const;
    float GetOceanMassZFloat() const;
    const boost::multiprecision::uint128_t& GetOceanMassZ() const;
    float GetOceanMassVolatilesFloat() const;
    const boost::multiprecision::uint128_t& GetOceanMassVolatiles() const;
    float GetOceanMassEnergeticNuclideFloat() const;
    const boost::multiprecision::uint128_t& GetOceanMassEnergeticNuclide() const;
    float GetMassFloat() const;
    const boost::multiprecision::uint128_t GetMass() const;
    float GetCrustMineralMassFloat() const;
    const boost::multiprecision::uint128_t& GetCrustMineralMass() const;
    bool  GetMigration() const;
    PlanetType GetPlanetType() const;

    std::shared_ptr<Civilization>& CivilizationData();

private:
    ExtendedProperties _PlanetExtraProperties{};
};

_ASTRO_END
_NPGS_END

#include "Planet.inl"