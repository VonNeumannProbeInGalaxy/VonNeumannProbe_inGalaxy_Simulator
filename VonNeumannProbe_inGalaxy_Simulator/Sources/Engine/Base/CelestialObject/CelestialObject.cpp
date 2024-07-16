#include "CelestialObject.h"

CelestialBody::CelestialBody(double Radius, double Mass, double Oblateness,
    double AxisTilt, double Age, double EscapeVelocity, double MagneticField) :

    _Radius(Radius), _Mass(Mass), _Oblateness(Oblateness), _AxisTilt(AxisTilt),
    _Age(Age), _EscapeVelocity(EscapeVelocity), _MagneticField(MagneticField) {}
