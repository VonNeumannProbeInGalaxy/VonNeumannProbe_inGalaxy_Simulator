#pragma once

#include "Planet.h"

_NPGS_BEGIN
_ASTROOBJECT_BEGIN

inline Planet& Planet::SetExtendedProperties(const ExtendedProperties& PlanetExtraProperties) {
    _PlanetExtraProperties = PlanetExtraProperties;
    return *this;
}

inline const Planet::ExtendedProperties& Planet::GetExtendedProperties() const {
    return _PlanetExtraProperties;
}

inline Planet& Planet::SetAbledo(double Albedo) {
    _PlanetExtraProperties.Albedo = Albedo;
    return *this;
}

inline Planet& Planet::SetAverageDensity(double AverageDensity) {
    _PlanetExtraProperties.AverageDensity = AverageDensity;
    return *this;
}

inline Planet& Planet::SetSurfaceGravity(double SurfaceGravity) {
    _PlanetExtraProperties.SurfaceGravity = SurfaceGravity;
    return *this;
}

inline Planet& Planet::SetGreenhouseFactor(double GreenhouseFactor) {
    _PlanetExtraProperties.GreenhouseFactor = GreenhouseFactor;
    return *this;
}

inline Planet& Planet::SetHydrosphere(double Hydrosphere) {
    _PlanetExtraProperties.Hydrosphere = Hydrosphere;
    return *this;
}

inline Planet& Planet::SetIceCover(double IceCover) {
    _PlanetExtraProperties.IceCover = IceCover;
    return *this;
}

inline Planet& Planet::SetCloudCover(double CloudCover) {
    _PlanetExtraProperties.CloudCover = CloudCover;
    return *this;
}

inline Planet& Planet::SetAtmospherePressure(double AtmospherePressure) {
    _PlanetExtraProperties.AtmospherePressure = AtmospherePressure;
    return *this;
}

inline Planet& Planet::SetAtmosphereDensity(double AtmosphereDensity) {
    _PlanetExtraProperties.AtmosphereDensity = AtmosphereDensity;
    return *this;
}

inline Planet& Planet::SetAtmosphereComposition(double AtmosphereComposition) {
    _PlanetExtraProperties.AtmosphereComposition = AtmosphereComposition;
    return *this;
}

inline Planet& Planet::SetTectonicActivity(double TectonicActivity) {
    _PlanetExtraProperties.TectonicActivity = TectonicActivity;
    return *this;
}

inline Planet& Planet::SetVolcanism(double Volcanism) {
    _PlanetExtraProperties.Volcanism = Volcanism;
    return *this;
}

inline Planet& Planet::SetLife(bool bHasLife) {
    _PlanetExtraProperties.bHasLife = bHasLife;
    return *this;
}

inline Planet& Planet::SetType(PlanetType Type) {
    _PlanetExtraProperties.Type = Type;
    return *this;
}

inline double Planet::GetAbledo() const {
    return _PlanetExtraProperties.Albedo;
}

inline double Planet::GetAverageDensity() const {
    return _PlanetExtraProperties.AverageDensity;
}

inline double Planet::GetSurfaceGravity() const {
    return _PlanetExtraProperties.SurfaceGravity;
}

inline double Planet::GetGreenhouseFactor() const {
    return _PlanetExtraProperties.GreenhouseFactor;
}

inline double Planet::GetHydrosphere() const {
    return _PlanetExtraProperties.Hydrosphere;
}

inline double Planet::GetIceCover() const {
    return _PlanetExtraProperties.IceCover;
}

inline double Planet::GetCloudCover() const {
    return _PlanetExtraProperties.CloudCover;
}

inline double Planet::GetAtmospherePressure() const {
    return _PlanetExtraProperties.AtmospherePressure;
}

inline double Planet::GetAtmosphereDensity() const {
    return _PlanetExtraProperties.AtmosphereDensity;
}

inline double Planet::GetAtmosphereComposition() const {
    return _PlanetExtraProperties.AtmosphereComposition;
}

inline double Planet::GetTectonicActivity() const {
    return _PlanetExtraProperties.TectonicActivity;
}

inline double Planet::GetVolcanism() const {
    return _PlanetExtraProperties.Volcanism;
}

inline bool Planet::GetLife() const {
    return _PlanetExtraProperties.bHasLife;
}

inline Planet::PlanetType Planet::GetType() const {
    return _PlanetExtraProperties.Type;
}

_ASTROOBJECT_END
_NPGS_END
