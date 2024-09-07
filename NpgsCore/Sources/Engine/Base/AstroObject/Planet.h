#pragma once

#include <boost/multiprecision/cpp_int.hpp>
#include "Engine/Base/AstroObject/CelestialObject.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_ASTROOBJECT_BEGIN

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
        boost::multiprecision::int128_t Z;
        boost::multiprecision::int128_t Volatiles;
        boost::multiprecision::int128_t EnergeticNuclide;
    };

    struct ExtendedProperties {
        ComplexMass AtmosphereMass;
        ComplexMass CoreMass;
        ComplexMass OceanMass;
        boost::multiprecision::int128_t CrustMineralMass;
        boost::multiprecision::int128_t RingsMass;
        bool bIsMigrated;

        PlanetType Type;
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
    Planet& SetCrustMineralMass(const boost::multiprecision::int128_t& CrustMineralMass);
    Planet& SetRingsMass(float RingsMass);
    Planet& SetRingsMass(const boost::multiprecision::int128_t& RingsMass);
    Planet& SetMigration(bool bIsMigrated);
    Planet& SetPlanetType(PlanetType Type);

    // Setters
    // Setters for every mass property
    // -------------------------------
    Planet& SetAtmosphereMassZ(float AtmosphereMassZ);
    Planet& SetAtmosphereMassZ(const boost::multiprecision::int128_t& AtmosphereMassZ);
    Planet& SetAtmosphereMassVolatiles(float AtmosphereMassVolatiles);
    Planet& SetAtmosphereMassVolatiles(const boost::multiprecision::int128_t& AtmosphereMassVolatiles);
    Planet& SetAtmosphereMassEnergeticNuclide(float AtmosphereMassEnergeticNuclide);
    Planet& SetAtmosphereMassEnergeticNuclide(const boost::multiprecision::int128_t& AtmosphereMassEnergeticNuclide);
    Planet& SetCoreMassZ(float CoreMassZ);
    Planet& SetCoreMassZ(const boost::multiprecision::int128_t& CoreMassZ);
    Planet& SetCoreMassVolatiles(float CoreMassVolatiles);
    Planet& SetCoreMassVolatiles(const boost::multiprecision::int128_t& CoreMassVolatiles);
    Planet& SetCoreMassEnergeticNuclide(float CoreMassEnergeticNuclide);
    Planet& SetCoreMassEnergeticNuclide(const boost::multiprecision::int128_t& CoreMassEnergeticNuclide);
    Planet& SetOceanMassZ(float OceanMassZ);
    Planet& SetOceanMassZ(const boost::multiprecision::int128_t& OceanMassZ);
    Planet& SetOceanMassVolatiles(float OceanMassVolatiles);
    Planet& SetOceanMassVolatiles(const boost::multiprecision::int128_t& OceanMassVolatiles);
    Planet& SetOceanMassEnergeticNuclide(float OceanMassEnergeticNuclide);
    Planet& SetOceanMassEnergeticNuclide(const boost::multiprecision::int128_t& OceanMassEnergeticNuclide);

    // Getters
    // Getters for ExtendedProperties
    // ------------------------------
    const ComplexMass& GetAtmosphereMass() const;
    const ComplexMass& GetCoreMass() const;
    const ComplexMass& GetOceanMass() const;
    const float GetMass() const;
    const boost::multiprecision::int128_t& GetCrustMineralMass() const;
    const boost::multiprecision::int128_t& GetRingsMass() const;
    bool GetMigration() const;
    PlanetType GetPlanetType() const;

private:
    ExtendedProperties _PlanetExtraProperties{};
};

_ASTROOBJECT_END
_NPGS_END

#include "Planet.inl"
