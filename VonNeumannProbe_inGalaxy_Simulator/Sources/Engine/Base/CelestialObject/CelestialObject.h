#pragma once

#include <string>

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
        double Oblateness;
        double AxisTilt;
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

    void SetOblateness(double Oblateness) {
        _Properties.Oblateness = Oblateness;
    }

    void SetAxisTilt(double AxisTilt) {
        _Properties.AxisTilt = AxisTilt;
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
