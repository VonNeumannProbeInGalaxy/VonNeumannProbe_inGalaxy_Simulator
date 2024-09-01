#pragma once

#include <boost/multiprecision/cpp_int.hpp>
#include "Engine/Base/AstroObject/CelestialObject.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_ASTROOBJECT_BEGIN

class NPGS_API Planet : public CelestialBody {
public:
    enum class PlanetType {
        kRocky,
        kTerra,
        kIcePlanet,
        kChthonian,
        kOceanic,
        kSubIceGiant,
        kIceGiant,
        kGasGiant,
        kHotSubIceGiant,
        kHotIceGiant,
        kHotGasGiant
    };

    struct MassProperties {
        boost::multiprecision::int128_t Z;
        boost::multiprecision::int128_t Volatiles;
        boost::multiprecision::int128_t EnergeticNuclide;
    };

    struct ExtendedProperties {
        MassProperties OceanMass;
        MassProperties AtmosphereMass;
        MassProperties CoreMass;
        boost::multiprecision::int128_t CrustMineralMass;
        boost::multiprecision::int128_t RingsMass;

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
    Planet& SetOceanMass(const MassProperties& OceanMass);
    Planet& SetAtmosphereMass(const MassProperties& AtmosphereMass);
    Planet& SetCoreMass(const MassProperties& CoreMass);
    Planet& SetCrustMineralMass(const boost::multiprecision::int128_t& CrustMineralMass);
    Planet& SetRingsMass(const boost::multiprecision::int128_t& RingsMass);
    Planet& SetPlanetType(PlanetType Type);

    // Getters
    // Getters for ExtendedProperties
    // ------------------------------
    const MassProperties& GetOceanMass() const;
    const MassProperties& GetAtmosphereMass() const;
    const MassProperties& GetCoreMass() const;
    const boost::multiprecision::int128_t& GetCrustMineralMass() const;
    const boost::multiprecision::int128_t& GetRingsMass() const;
    PlanetType GetPlanetType() const;

private:
    ExtendedProperties _PlanetExtraProperties{};
};

_ASTROOBJECT_END
_NPGS_END

#include "Planet.inl"
