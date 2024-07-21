#pragma once

#include <string>

#include <glm/glm.hpp>

#include "Engine/Core/Base.h"
#include "Engine/Core/Constants.h"

_NPGS_BEGIN

namespace AstroObject {

class CelestialBody {
public:
    struct BaryCenter {
        std::string Name;
        glm::dvec3 Position;
        double Distance;
    };

    struct OrbitProperties {
        BaryCenter ParentBody;
        double Epoch;
        double Period;
        double SemiMajorAxis;
        double Eccentricity;
        double Inclination;
        double LongitudeOfAscendingNode;
        double ArgumentOfPeriapsis;
        double MeanAnomaly;
    };

    struct BasicProperties {
        std::string Name;
        double Mass;
        double Radius;
        double RotateVelocity;
        double RotationPeriod;
        double Oblateness;
        double AxisTilt;
        double Albedo;
        double Age;
        double EscapeVelocity;
        double MagneticField;

        OrbitProperties Orbit;
    };

public:
    CelestialBody() = default;
    CelestialBody(const BasicProperties& Properties);
    ~CelestialBody() = default;

public:
    CelestialBody& SetBasicProperties(const BasicProperties& Properties) {
        _Properties = Properties;
        return *this;
    }

    CelestialBody& SetOrbitProperties(const OrbitProperties& Properties) {
        _Properties.Orbit = Properties;
        return *this;
    }

    BasicProperties GetBasicProperties() const {
        return _Properties;
    }

    OrbitProperties GetOrbitProperties() const {
        return _Properties.Orbit;
    }

    BasicProperties Data() const {
        return GetBasicProperties();
    }

public:
    // Setters
    // Setters for BasicProperties
    // --------------------------
    CelestialBody& SetName(const std::string& Name) {
        _Properties.Name = Name;
        return *this;
    }

    CelestialBody& SetRadius(double Radius) {
        _Properties.Radius = Radius;
        return *this;
    }

    CelestialBody& SetMass(double Mass) {
        _Properties.Mass = Mass;
        return *this;
    }

    CelestialBody& SetRotateVelocity(double RotateVelocity) {
        _Properties.RotateVelocity = RotateVelocity;
        return *this;
    }

    CelestialBody& SetRotationPeriod(double RotationPeriod) {
        _Properties.RotationPeriod = RotationPeriod;
        return *this;
    }

    CelestialBody& SetOblateness(double Oblateness) {
        _Properties.Oblateness = Oblateness;
        return *this;
    }

    CelestialBody& SetAxisTilt(double AxisTilt) {
        _Properties.AxisTilt = AxisTilt;
        return *this;
    }

    CelestialBody& SetAlbedo(double Albedo) {
        _Properties.Albedo = Albedo;
        return *this;
    }

    CelestialBody& SetAge(double Age) {
        _Properties.Age = Age;
        return *this;
    }

    CelestialBody& SetEscapeVelocity(double EscapeVelocity) {
        _Properties.EscapeVelocity = EscapeVelocity;
        return *this;
    }

    CelestialBody& SetMagneticField(double MagneticField) {
        _Properties.MagneticField = MagneticField;
        return *this;
    }

    // Setters for OrbitProperties
    // ---------------------------
    CelestialBody& SetParentBody(const BaryCenter& ParentBody) {
        _Properties.Orbit.ParentBody = ParentBody;
        return *this;
    }

    CelestialBody& SetEpoch(double Epoch) {
        _Properties.Orbit.Epoch = Epoch;
        return *this;
    }

    CelestialBody& SetPeriod(double Period) {
        _Properties.Orbit.Period = Period;
        return *this;
    }

    CelestialBody& SetSemiMajorAxis(double SemiMajorAxis) {
        _Properties.Orbit.SemiMajorAxis = SemiMajorAxis;
        return *this;
    }

    CelestialBody& SetEccentricity(double Eccentricity) {
        _Properties.Orbit.Eccentricity = Eccentricity;
        return *this;
    }

    CelestialBody& SetInclination(double Inclination) {
        _Properties.Orbit.Inclination = Inclination;
        return *this;
    }

    CelestialBody& SetLongitudeOfAscendingNode(double LongitudeOfAscendingNode) {
        _Properties.Orbit.LongitudeOfAscendingNode = LongitudeOfAscendingNode;
        return *this;
    }

    CelestialBody& SetArgumentOfPeriapsis(double ArgumentOfPeriapsis) {
        _Properties.Orbit.ArgumentOfPeriapsis = ArgumentOfPeriapsis;
        return *this;
    }

    CelestialBody& SetMeanAnomaly(double MeanAnomaly) {
        _Properties.Orbit.MeanAnomaly = MeanAnomaly;
        return *this;
    }

    // Getters
    // Getters for BasicProperties
    // --------------------------
    std::string GetName() const {
        return _Properties.Name;
    }

    double GetRadius() const {
        return _Properties.Radius;
    }

    double GetMass() const {
        return _Properties.Mass;
    }

    double GetRotateVelocity() const {
        return _Properties.RotateVelocity;
    }

    double GetRotationPeriod() const {
        return _Properties.RotationPeriod;
    }

    double GetOblateness() const {
        return _Properties.Oblateness;
    }

    double GetAxisTilt() const {
        return _Properties.AxisTilt;
    }

    double GetAlbedo() const {
        return _Properties.Albedo;
    }

    double GetAge() const {
        return _Properties.Age;
    }

    double GetEscapeVelocity() const {
        return _Properties.EscapeVelocity;
    }

    double GetMagneticField() const {
        return _Properties.MagneticField;
    }

    // Getters for OrbitProperties
    // ---------------------------
    BaryCenter GetParentBody() const {
        return _Properties.Orbit.ParentBody;
    }

    double GetEpoch() const {
        return _Properties.Orbit.Epoch;
    }

    double GetPeriod() const {
        return _Properties.Orbit.Period;
    }

    double GetSemiMajorAxis() const {
        return _Properties.Orbit.SemiMajorAxis;
    }

    double GetEccentricity() const {
        return _Properties.Orbit.Eccentricity;
    }

    double GetInclination() const {
        return _Properties.Orbit.Inclination;
    }

    double GetLongitudeOfAscendingNode() const {
        return _Properties.Orbit.LongitudeOfAscendingNode;
    }

    double GetArgumentOfPeriapsis() const {
        return _Properties.Orbit.ArgumentOfPeriapsis;
    }

    double GetMeanAnomaly() const {
        return _Properties.Orbit.MeanAnomaly;
    }

private:
    BasicProperties _Properties;
};

class MolecularCloud {
public:
    //struct CloudRegion {
    //    double Radius;
    //    double Density;
    //};

    struct BasicProperties {
        double Radius;
        double Mass;
        double Temperature;
        double Density;
        double MagneticField;
    };

public:
    MolecularCloud() = delete;
    MolecularCloud(const BasicProperties& Properties);

private:
    BasicProperties _Properties;
};

}

_NPGS_END
