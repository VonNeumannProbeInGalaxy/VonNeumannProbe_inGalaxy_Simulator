#pragma once

#include <string>
#include <vector>
#include "Engine/Core/Base.h"
#include "Engine/Core/Constants.h"

_NPGS_BEGIN

class CelestialBody {
public:
    struct OrbitProperties {
        std::string ParentBody;
        double Epoch;
        double Period;
        double SemiMajorAxis;
        double Eccentricity;
        double Inclination;
        double LongitudeOfAscendingNode;
        double ArgumentOfPeriapsis;
        double MeanAnomaly;
    };

    struct BaseProperties {
        std::string Name;
        double Radius;
        double Mass;
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
    CelestialBody() = delete;
    CelestialBody(const BaseProperties& Properties);

public:
    void SetBaseProperties(const BaseProperties& Properties) {
        _Properties = Properties;
    }

    BaseProperties GetBaseProperties() const {
        return _Properties;
    }

public:
    // Setters
    // Setters for BaseProperties
    // --------------------------
    void SetName(const std::string& Name) {
        _Properties.Name = Name;
    }

    void SetRadius(double Radius) {
        _Properties.Radius = Radius;
    }

    void SetMass(double Mass) {
        _Properties.Mass = Mass;
    }

    void SetRotateVelocity(double RotateVelocity) {
        _Properties.RotateVelocity = RotateVelocity;
    }

    void SetRotationPeriod(double RotationPeriod) {
        _Properties.RotationPeriod = RotationPeriod;
    }

    void SetOblateness(double Oblateness) {
        _Properties.Oblateness = Oblateness;
    }

    void SetAxisTilt(double AxisTilt) {
        _Properties.AxisTilt = AxisTilt;
    }

    void SetAlbedo(double Albedo) {
        _Properties.Albedo = Albedo;
    }

    void SetAge(double Age) {
        _Properties.Age = Age;
    }

    void SetEscapeVelocity(double EscapeVelocity) {
        _Properties.EscapeVelocity = EscapeVelocity;
    }

    void SetMagneticField(double MagneticField) {
        _Properties.MagneticField = MagneticField;
    }

    // Setters for OrbitProperties
    // ---------------------------
    void SetParentBody(const std::string& ParentBody) {
        _Properties.Orbit.ParentBody = ParentBody;
    }

    void SetEpoch(double Epoch) {
        _Properties.Orbit.Epoch = Epoch;
    }

    void SetPeriod(double Period) {
        _Properties.Orbit.Period = Period;
    }

    void SetSemiMajorAxis(double SemiMajorAxis) {
        _Properties.Orbit.SemiMajorAxis = SemiMajorAxis;
    }

    void SetEccentricity(double Eccentricity) {
        _Properties.Orbit.Eccentricity = Eccentricity;
    }

    void SetInclination(double Inclination) {
        _Properties.Orbit.Inclination = Inclination;
    }

    void SetLongitudeOfAscendingNode(double LongitudeOfAscendingNode) {
        _Properties.Orbit.LongitudeOfAscendingNode = LongitudeOfAscendingNode;
    }

    void SetArgumentOfPeriapsis(double ArgumentOfPeriapsis) {
        _Properties.Orbit.ArgumentOfPeriapsis = ArgumentOfPeriapsis;
    }

    void SetMeanAnomaly(double MeanAnomaly) {
        _Properties.Orbit.MeanAnomaly = MeanAnomaly;
    }

private:
    BaseProperties _Properties;
};

class MolecularCloud {
public:
    //struct CloudRegion {
    //    double Radius;
    //    double Density;
    //};

    struct BaseProperties {
        double Radius;
        double Mass;
        double Temperature;
        double Density;
        double MagneticField;
    };

public:
    MolecularCloud() = delete;
    MolecularCloud(const BaseProperties& Properties);

private:
    BaseProperties _Properties;
};

_NPGS_END
