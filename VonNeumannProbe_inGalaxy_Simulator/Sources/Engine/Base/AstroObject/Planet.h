#pragma once

#include "Engine/Base/AstroObject/CelestialObject.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN

namespace AstroObject {

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
        bool   bHasLife;

        PlanetType Type;
    };

public:
    Planet() = default;
    Planet(const CelestialBody::BasicProperties& PlanetBasicProperties, const ExtendedProperties& PlanetExtraProperties);
    ~Planet() = default;

public:
    Planet& SetExtendedProperties(const ExtendedProperties& PlanetExtraProperties) {
        _PlanetExtraProperties = PlanetExtraProperties;
        return *this;
    }

    ExtendedProperties GetExtendedProperties() const {
        return _PlanetExtraProperties;
    }

    ExtendedProperties Data() const {
        return GetExtendedProperties();
    }

public:
    // Setters
    // Setters for ExtendedProperties
    // ------------------------------
    Planet& SetAverageDensity(double AverageDensity) {
        _PlanetExtraProperties.AverageDensity = AverageDensity;
        return *this;
    }

    Planet& SetSurfGravity(double SurfGravity) {
        _PlanetExtraProperties.SurfGravity = SurfGravity;
        return *this;
    }

    Planet& SetGreenhouseFactor(double GreenhouseFactor) {
        _PlanetExtraProperties.GreenhouseFactor = GreenhouseFactor;
        return *this;
    }

    Planet& SetHydrosphere(double Hydrosphere) {
        _PlanetExtraProperties.Hydrosphere = Hydrosphere;
        return *this;
    }

    Planet& SetIceCover(double IceCover) {
        _PlanetExtraProperties.IceCover = IceCover;
        return *this;
    }

    Planet& SetCloudCover(double CloudCover) {
        _PlanetExtraProperties.CloudCover = CloudCover;
        return *this;
    }

    Planet& SetAtmospherePressure(double AtmospherePressure) {
        _PlanetExtraProperties.AtmospherePressure = AtmospherePressure;
        return *this;
    }

    Planet& SetAtmosphereDensity(double AtmosphereDensity) {
        _PlanetExtraProperties.AtmosphereDensity = AtmosphereDensity;
        return *this;
    }

    Planet& SetAtmosphereComposition(double AtmosphereComposition) {
        _PlanetExtraProperties.AtmosphereComposition = AtmosphereComposition;
        return *this;
    }

    Planet& SetTectonicActivity(double TectonicActivity) {
        _PlanetExtraProperties.TectonicActivity = TectonicActivity;
        return *this;
    }

    Planet& SetVolcanism(double Volcanism) {
        _PlanetExtraProperties.Volcanism = Volcanism;
        return *this;
    }

    Planet& SetLife(bool bHasLife) {
        _PlanetExtraProperties.bHasLife = bHasLife;
        return *this;
    }

    Planet& SetType(PlanetType Type) {
        _PlanetExtraProperties.Type = Type;
        return *this;
    }

    // Getters
    // Getters for ExtendedProperties
    // ------------------------------
    double GetAverageDensity() const {
        return _PlanetExtraProperties.AverageDensity;
    }

    double GetSurfGravity() const {
        return _PlanetExtraProperties.SurfGravity;
    }

    double GetGreenhouseFactor() const {
        return _PlanetExtraProperties.GreenhouseFactor;
    }

    double GetHydrosphere() const {
        return _PlanetExtraProperties.Hydrosphere;
    }

    double GetIceCover() const {
        return _PlanetExtraProperties.IceCover;
    }

    double GetCloudCover() const {
        return _PlanetExtraProperties.CloudCover;
    }

    double GetAtmospherePressure() const {
        return _PlanetExtraProperties.AtmospherePressure;
    }

    double GetAtmosphereDensity() const {
        return _PlanetExtraProperties.AtmosphereDensity;
    }

    double GetAtmosphereComposition() const {
        return _PlanetExtraProperties.AtmosphereComposition;
    }

    double GetTectonicActivity() const {
        return _PlanetExtraProperties.TectonicActivity;
    }

    double GetVolcanism() const {
        return _PlanetExtraProperties.Volcanism;
    }

    bool GetbLife() const {
        return _PlanetExtraProperties.bHasLife;
    }

    PlanetType GetType() const {
        return _PlanetExtraProperties.Type;
    }

private:
    ExtendedProperties _PlanetExtraProperties;
};

}

_NPGS_END
