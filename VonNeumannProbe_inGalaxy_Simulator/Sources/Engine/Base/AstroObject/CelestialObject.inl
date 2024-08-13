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

inline CelestialBody& CelestialBody::SetRadius(double Radius) {
    _Properties.Radius = Radius;
    return *this;
}

inline CelestialBody& CelestialBody::SetMass(double Mass) {
    _Properties.Mass = Mass;
    return *this;
}

inline CelestialBody& CelestialBody::SetRotationPeriod(double RotationPeriod) {
    _Properties.RotationPeriod = RotationPeriod;
    return *this;
}

inline CelestialBody& CelestialBody::SetOblateness(double Oblateness) {
    _Properties.Oblateness = Oblateness;
    return *this;
}

inline CelestialBody& CelestialBody::SetAxisTilt(double AxisTilt) {
    _Properties.AxisTilt = AxisTilt;
    return *this;
}

inline CelestialBody& CelestialBody::SetAlbedo(double Albedo) {
    _Properties.Albedo = Albedo;
    return *this;
}

inline CelestialBody& CelestialBody::SetAge(double Age) {
    _Properties.Age = Age;
    return *this;
}

inline CelestialBody& CelestialBody::SetEscapeVelocity(double EscapeVelocity) {
    _Properties.EscapeVelocity = EscapeVelocity;
    return *this;
}

inline CelestialBody& CelestialBody::SetMagneticField(double MagneticField) {
    _Properties.MagneticField = MagneticField;
    return *this;
}

inline CelestialBody& CelestialBody::SetParentBody(const BaryCenter& ParentBody) {
    _Properties.Orbit.ParentBody = ParentBody;
    return *this;
}

inline CelestialBody& CelestialBody::SetEpoch(double Epoch) {
    _Properties.Orbit.Epoch = Epoch;
    return *this;
}

inline CelestialBody& CelestialBody::SetPeriod(double Period) {
    _Properties.Orbit.Period = Period;
    return *this;
}

inline CelestialBody& CelestialBody::SetSemiMajorAxis(double SemiMajorAxis) {
    _Properties.Orbit.SemiMajorAxis = SemiMajorAxis;
    return *this;
}

inline CelestialBody& CelestialBody::SetEccentricity(double Eccentricity) {
    _Properties.Orbit.Eccentricity = Eccentricity;
    return *this;
}

inline CelestialBody& CelestialBody::SetInclination(double Inclination) {
    _Properties.Orbit.Inclination = Inclination;
    return *this;
}

inline CelestialBody& CelestialBody::SetLongitudeOfAscendingNode(double LongitudeOfAscendingNode) {
    _Properties.Orbit.LongitudeOfAscendingNode = LongitudeOfAscendingNode;
    return *this;
}

inline CelestialBody& CelestialBody::SetArgumentOfPeriapsis(double ArgumentOfPeriapsis) {
    _Properties.Orbit.ArgumentOfPeriapsis = ArgumentOfPeriapsis;
    return *this;
}

inline CelestialBody& CelestialBody::SetMeanAnomaly(double MeanAnomaly) {
    _Properties.Orbit.MeanAnomaly = MeanAnomaly;
    return *this;
}

inline std::string CelestialBody::GetName() const {
    return _Properties.Name;
}

inline double CelestialBody::GetRadius() const {
    return _Properties.Radius;
}

inline double CelestialBody::GetMass() const {
    return _Properties.Mass;
}

inline double CelestialBody::GetRotationPeriod() const {
    return _Properties.RotationPeriod;
}

inline double CelestialBody::GetOblateness() const {
    return _Properties.Oblateness;
}

inline double CelestialBody::GetAxisTilt() const {
    return _Properties.AxisTilt;
}

inline double CelestialBody::GetAlbedo() const {
    return _Properties.Albedo;
}

inline double CelestialBody::GetAge() const {
    return _Properties.Age;
}

inline double CelestialBody::GetEscapeVelocity() const {
    return _Properties.EscapeVelocity;
}

inline double CelestialBody::GetMagneticField() const {
    return _Properties.MagneticField;
}

inline CelestialBody::BaryCenter CelestialBody::GetParentBody() const {
    return _Properties.Orbit.ParentBody;
}

inline double CelestialBody::GetEpoch() const {
    return _Properties.Orbit.Epoch;
}

inline double CelestialBody::GetPeriod() const {
    return _Properties.Orbit.Period;
}

inline double CelestialBody::GetSemiMajorAxis() const {
    return _Properties.Orbit.SemiMajorAxis;
}

inline double CelestialBody::GetEccentricity() const {
    return _Properties.Orbit.Eccentricity;
}

inline double CelestialBody::GetInclination() const {
    return _Properties.Orbit.Inclination;
}

inline double CelestialBody::GetLongitudeOfAscendingNode() const {
    return _Properties.Orbit.LongitudeOfAscendingNode;
}

inline double CelestialBody::GetArgumentOfPeriapsis() const {
    return _Properties.Orbit.ArgumentOfPeriapsis;
}

inline double CelestialBody::GetMeanAnomaly() const {
    return _Properties.Orbit.MeanAnomaly;
}

_ASTROOBJECT_END
_NPGS_END
