#pragma once

class CelestialBody {
public:
    CelestialBody() = delete;
    CelestialBody(double Radius, double Mass, double Oblateness, double AxisTilt,
                  double Age, double EscapeVelocity, double MagneticField);

public:
    void SetRadius(double Radius) {
        _Radius = Radius;
    }

    void SetMass(double Mass) {
        _Mass = Mass;
    }

    void SetOblateness(double Oblateness) {
        _Oblateness = Oblateness;
    }

    void SetAxisTilt(double AxisTilt) {
        _AxisTilt = AxisTilt;
    }

    void SetAge(double Age) {
        _Age = Age;
    }

    void SetEscapeVelocity(double EscapeVelocity) {
        _EscapeVelocity = EscapeVelocity;
    }

    void SetMagneticField(double MagneticField) {
        _MagneticField = MagneticField;
    }

public:
    double GetRadius() const {
        return _Radius;
    }

    double GetMass() const {
        return _Mass;
    }

    double GetOblateness() const {
        return _Oblateness;
    }

    double GetAxisTilt() const {
        return _AxisTilt;
    }

    double GetAge() const {
        return _Age;
    }

    double GetEscapeVelocity() const {
        return _EscapeVelocity;
    }

    double GetMagneticField() const {
        return _MagneticField;
    }

private:
    double _Radius;
    double _Mass;
    double _Oblateness;
    double _AxisTilt;
    double _Age;
    double _EscapeVelocity;
    double _MagneticField;
};
