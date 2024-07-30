#pragma once

#include <string>
#include <glm/glm.hpp>
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_ASTROOBJECT_BEGIN

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

    CelestialBody& SetBasicProperties(const BasicProperties& Properties);
    CelestialBody& SetOrbitProperties(const OrbitProperties& Properties);
    const BasicProperties& GetBasicProperties() const;
    const OrbitProperties& GetOrbitProperties() const;

    // Setters
    // Setters for BasicProperties
    // ---------------------------
    CelestialBody& SetName(const std::string& Name);
    CelestialBody& SetRadius(double Radius);
    CelestialBody& SetMass(double Mass);
    CelestialBody& SetRotateVelocity(double RotateVelocity);
    CelestialBody& SetRotationPeriod(double RotationPeriod);
    CelestialBody& SetOblateness(double Oblateness);
    CelestialBody& SetAxisTilt(double AxisTilt);
    CelestialBody& SetAlbedo(double Albedo);
    CelestialBody& SetAge(double Age);
    CelestialBody& SetEscapeVelocity(double EscapeVelocity);
    CelestialBody& SetMagneticField(double MagneticField);

    // Setters for OrbitProperties
    // ---------------------------
    CelestialBody& SetParentBody(const BaryCenter& ParentBody);
    CelestialBody& SetEpoch(double Epoch);
    CelestialBody& SetPeriod(double Period);
    CelestialBody& SetSemiMajorAxis(double SemiMajorAxis);
    CelestialBody& SetEccentricity(double Eccentricity);
    CelestialBody& SetInclination(double Inclination);
    CelestialBody& SetLongitudeOfAscendingNode(double LongitudeOfAscendingNode);
    CelestialBody& SetArgumentOfPeriapsis(double ArgumentOfPeriapsis);
    CelestialBody& SetMeanAnomaly(double MeanAnomaly);

    // Getters
    // Getters for BasicProperties
    // ---------------------------
    std::string GetName() const;
    double GetRadius() const;
    double GetMass() const;
    double GetRotateVelocity() const;
    double GetRotationPeriod() const;
    double GetOblateness() const;
    double GetAxisTilt() const;
    double GetAlbedo() const;
    double GetAge() const;
    double GetEscapeVelocity() const;
    double GetMagneticField() const;

    // Getters for OrbitProperties
    // ---------------------------
    BaryCenter GetParentBody() const;
    double GetEpoch() const;
    double GetPeriod() const;
    double GetSemiMajorAxis() const;
    double GetEccentricity() const;
    double GetInclination() const;
    double GetLongitudeOfAscendingNode() const;
    double GetArgumentOfPeriapsis() const;
    double GetMeanAnomaly() const;

private:
    BasicProperties _Properties;
};

class MolecularCloud {
public:
    struct BasicProperties {
        double Radius;
        double Mass;
        double Temperature;
        double Density;
        double MagneticField;
    };

public:
    MolecularCloud() = default;
    MolecularCloud(const BasicProperties& Properties);
    ~MolecularCloud() = default;

private:
    BasicProperties _Properties;
};

_ASTROOBJECT_END
_NPGS_END

#include "CelestialObject.inl"
