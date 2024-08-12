#pragma once

#include "Engine/Base/AstroObject/CelestialObject.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_ASTROOBJECT_BEGIN

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
        double AverageDensity;        // 平均密度，单位 kg/m^3
        double SurfaceGravity;        // 表面重力，单位 m/s^2
        double GreenhouseFactor;      // 温室效应因子
        double Hydrosphere;           // 水圈覆盖率
        double IceCover;              // 冰覆盖率
        double CloudCover;            // 云覆盖率
        double AtmospherePressure;    // 大气压，单位 atm
        double AtmosphereDensity;     // 大气密度，单位 kg/m^3
        double AtmosphereComposition; // 大气成分
        double TectonicActivity;      // 构造活动
        double Volcanism;             // 火山活动
        bool   bHasLife;

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
    Planet& SetAverageDensity(double AverageDensity);
    Planet& SetSurfaceGravity(double SurfaceGravity);
    Planet& SetGreenhouseFactor(double GreenhouseFactor);
    Planet& SetHydrosphere(double Hydrosphere);
    Planet& SetIceCover(double IceCover);
    Planet& SetCloudCover(double CloudCover);
    Planet& SetAtmospherePressure(double AtmospherePressure);
    Planet& SetAtmosphereDensity(double AtmosphereDensity);
    Planet& SetAtmosphereComposition(double AtmosphereComposition);
    Planet& SetTectonicActivity(double TectonicActivity);
    Planet& SetVolcanism(double Volcanism);
    Planet& SetLife(bool bHasLife);
    Planet& SetType(PlanetType Type);

    // Getters
    // Getters for ExtendedProperties
    // ------------------------------
    double GetAverageDensity() const;
    double GetSurfaceGravity() const;
    double GetGreenhouseFactor() const;
    double GetHydrosphere() const;
    double GetIceCover() const;
    double GetCloudCover() const;
    double GetAtmospherePressure() const;
    double GetAtmosphereDensity() const;
    double GetAtmosphereComposition() const;
    double GetTectonicActivity() const;
    double GetVolcanism() const;
    bool   GetLife() const;
    PlanetType GetType() const;

private:
    ExtendedProperties _PlanetExtraProperties{};
};

_ASTROOBJECT_END
_NPGS_END

#include "Planet.inl"
