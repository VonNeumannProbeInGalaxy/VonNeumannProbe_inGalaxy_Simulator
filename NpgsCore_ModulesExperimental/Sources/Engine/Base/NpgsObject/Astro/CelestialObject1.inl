#pragma once

#include "Engine/Core/Base.h"

_NPGS_BEGIN
_ASTRO_BEGIN

inline CelestialBody& CelestialBody::SetBasicProperties(const BasicProperties& Properties) {
    _Properties = Properties;
    return *this;
}

inline const CelestialBody::BasicProperties& CelestialBody::GetBasicProperties() const {
    return _Properties;
}

inline CelestialBody& CelestialBody::SetNormal(const glm::vec2& Normal) {
    _Properties.Normal = Normal;
    return *this;
}

inline CelestialBody& CelestialBody::SetName(const std::string& Name) {
    _Properties.Name = Name;
    return *this;
}

inline CelestialBody& CelestialBody::SetAge(double Age) {
    _Properties.Age = Age;
    return *this;
}

inline CelestialBody& CelestialBody::SetRadius(float Radius) {
    _Properties.Radius = Radius;
    return *this;
}

inline CelestialBody& CelestialBody::SetSpin(float Spin) {
    _Properties.Spin = Spin;
    return *this;
}

inline CelestialBody& CelestialBody::SetOblateness(float Oblateness) {
    _Properties.Oblateness = Oblateness;
    return *this;
}

inline CelestialBody& CelestialBody::SetEscapeVelocity(float EscapeVelocity) {
    _Properties.EscapeVelocity = EscapeVelocity;
    return *this;
}

inline CelestialBody& CelestialBody::SetMagneticField(float MagneticField) {
    _Properties.MagneticField = MagneticField;
    return *this;
}

inline const glm::vec2& CelestialBody::GetNormal() const {
    return _Properties.Normal;
}

inline const std::string& CelestialBody::GetName() const {
    return _Properties.Name;
}

inline double CelestialBody::GetAge() const {
    return _Properties.Age;
}

inline float CelestialBody::GetRadius() const {
    return _Properties.Radius;
}

inline float CelestialBody::GetSpin() const {
    return _Properties.Spin;
}

inline float CelestialBody::GetOblateness() const {
    return _Properties.Oblateness;
}

inline float CelestialBody::GetEscapeVelocity() const {
    return _Properties.EscapeVelocity;
}

inline float CelestialBody::GetMagneticField() const {
    return _Properties.MagneticField;
}

_ASTRO_END
_NPGS_END
