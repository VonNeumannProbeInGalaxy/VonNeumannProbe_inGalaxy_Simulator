#pragma once

#include "Engine/Base/CelestialObject/CelestialObject.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN

class Planet : public CelestialBody {
public:
    enum class PlanetType {
        kTerrestrial,
        kGasGiant,
        kIceGiant,
        kDwarfPlanet,
        kAsteroid,
        kComet,
        kRoguePlanet,
        kRogueMoon,
        kPulsarPlanet,
        kExoplanet,
        kUnknown
    };

    struct ExtendedProperties {
        double AverageDensity;
        double SurfGravity;
        double GreenhouseFactor;
        double Hydrosphere;
        double IceCover;
        double CloudCover;
        double AtmospherePressure;
        double AtmosphereDensity;
        double AtmosphereComposition;
        double TectonicActivity;
        double Volcanism;
        double Life;

        PlanetType Type;
    };

public:
    Planet() = default;
    Planet(const CelestialBody::BaseProperties& PlanetBaseProperties, const ExtendedProperties& PlanetExtraProperties);

public:
    // Setters
    // Setters for ExtendedProperties
    // ------------------------------
    void SetAverageDensity(double AverageDensity) {
        _PlanetExtraProperties.AverageDensity = AverageDensity;
    }

    void SetSurfGravity(double SurfGravity) {
        _PlanetExtraProperties.SurfGravity = SurfGravity;
    }

    void SetGreenhouseFactor(double GreenhouseFactor) {
        _PlanetExtraProperties.GreenhouseFactor = GreenhouseFactor;
    }

    void SetHydrosphere(double Hydrosphere) {
        _PlanetExtraProperties.Hydrosphere = Hydrosphere;
    }

    void SetIceCover(double IceCover) {
        _PlanetExtraProperties.IceCover = IceCover;
    }

    void SetCloudCover(double CloudCover) {
        _PlanetExtraProperties.CloudCover = CloudCover;
    }

    void SetAtmospherePressure(double AtmospherePressure) {
        _PlanetExtraProperties.AtmospherePressure = AtmospherePressure;
    }

    void SetAtmosphereDensity(double AtmosphereDensity) {
        _PlanetExtraProperties.AtmosphereDensity = AtmosphereDensity;
    }

    void SetAtmosphereComposition(double AtmosphereComposition) {
        _PlanetExtraProperties.AtmosphereComposition = AtmosphereComposition;
    }

    void SetTectonicActivity(double TectonicActivity) {
        _PlanetExtraProperties.TectonicActivity = TectonicActivity;
    }

    void SetVolcanism(double Volcanism) {
        _PlanetExtraProperties.Volcanism = Volcanism;
    }

    void SetLife(double Life) {
        _PlanetExtraProperties.Life = Life;
    }

    void SetType(PlanetType Type) {
        _PlanetExtraProperties.Type = Type;
    }

private:
    ExtendedProperties _PlanetExtraProperties;
};

_NPGS_END
