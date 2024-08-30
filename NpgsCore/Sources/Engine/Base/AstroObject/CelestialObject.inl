#pragma once

#include "CelestialObject.h"

_NPGS_BEGIN
_ASTROOBJECT_BEGIN

inline CelestialBody& CelestialBody::SetBasicProperties(const BasicProperties& Properties) {
    _Properties = Properties;
    return *this;
}

inline CelestialBody& CelestialBody::SetOrbitProperties(const OrbitProperties& Properties) {
    _Properties.Orbit = Properties;
    return *this;
}

inline const CelestialBody::BasicProperties& CelestialBody::GetBasicProperties() const {
    return _Properties;
}

inline const CelestialBody::OrbitProperties& CelestialBody::GetOrbitProperties() const {
    return _Properties.Orbit;
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

inline CelestialBody& CelestialBody::SetAxisTilt(float AxisTilt) {
    _Properties.AxisTilt = AxisTilt;
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

inline CelestialBody& CelestialBody::SetParentBody(const BaryCenter& ParentBody) {
    _Properties.Orbit.ParentBody = ParentBody;
    return *this;
}

inline CelestialBody& CelestialBody::SetEpoch(float Epoch) {
    _Properties.Orbit.Epoch = Epoch;
    return *this;
}

inline CelestialBody& CelestialBody::SetPeriod(float Period) {
    _Properties.Orbit.Period = Period;
    return *this;
}

inline CelestialBody& CelestialBody::SetSemiMajorAxis(float SemiMajorAxis) {
    _Properties.Orbit.SemiMajorAxis = SemiMajorAxis;
    return *this;
}

inline CelestialBody& CelestialBody::SetEccentricity(float Eccentricity) {
    _Properties.Orbit.Eccentricity = Eccentricity;
    return *this;
}

inline CelestialBody& CelestialBody::SetInclination(float Inclination) {
    _Properties.Orbit.Inclination = Inclination;
    return *this;
}

inline CelestialBody& CelestialBody::SetLongitudeOfAscendingNode(float LongitudeOfAscendingNode) {
    _Properties.Orbit.LongitudeOfAscendingNode = LongitudeOfAscendingNode;
    return *this;
}

inline CelestialBody& CelestialBody::SetArgumentOfPeriapsis(float ArgumentOfPeriapsis) {
    _Properties.Orbit.ArgumentOfPeriapsis = ArgumentOfPeriapsis;
    return *this;
}

inline CelestialBody& CelestialBody::SetMeanAnomaly(float MeanAnomaly) {
    _Properties.Orbit.MeanAnomaly = MeanAnomaly;
    return *this;
}

inline std::string CelestialBody::GetName() const {
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

inline float CelestialBody::GetAxisTilt() const {
    return _Properties.AxisTilt;
}

inline float CelestialBody::GetEscapeVelocity() const {
    return _Properties.EscapeVelocity;
}

inline float CelestialBody::GetMagneticField() const {
    return _Properties.MagneticField;
}

inline CelestialBody::BaryCenter CelestialBody::GetParentBody() const {
    return _Properties.Orbit.ParentBody;
}

inline float CelestialBody::GetEpoch() const {
    return _Properties.Orbit.Epoch;
}

inline float CelestialBody::GetPeriod() const {
    return _Properties.Orbit.Period;
}

inline float CelestialBody::GetSemiMajorAxis() const {
    return _Properties.Orbit.SemiMajorAxis;
}

inline float CelestialBody::GetEccentricity() const {
    return _Properties.Orbit.Eccentricity;
}

inline float CelestialBody::GetInclination() const {
    return _Properties.Orbit.Inclination;
}

inline float CelestialBody::GetLongitudeOfAscendingNode() const {
    return _Properties.Orbit.LongitudeOfAscendingNode;
}

inline float CelestialBody::GetArgumentOfPeriapsis() const {
    return _Properties.Orbit.ArgumentOfPeriapsis;
}

inline float CelestialBody::GetMeanAnomaly() const {
    return _Properties.Orbit.MeanAnomaly;
}

_ASTROOBJECT_END
_NPGS_END
