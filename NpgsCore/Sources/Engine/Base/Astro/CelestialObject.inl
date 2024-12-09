#pragma once

#include "CelestialObject.h"

_NPGS_BEGIN
_ASTRO_BEGIN

NPGS_INLINE CelestialBody& CelestialBody::SetBasicProperties(const BasicProperties& Properties)
{
	_Properties = Properties;
	return *this;
}

NPGS_INLINE const CelestialBody::BasicProperties& CelestialBody::GetBasicProperties() const
{
	return _Properties;
}

NPGS_INLINE CelestialBody& CelestialBody::SetNormal(const glm::vec2& Normal)
{
	_Properties.Normal = Normal;
	return *this;
}

NPGS_INLINE CelestialBody& CelestialBody::SetName(const std::string& Name)
{
	_Properties.Name = Name;
	return *this;
}

NPGS_INLINE CelestialBody& CelestialBody::SetAge(double Age)
{
	_Properties.Age = Age;
	return *this;
}

NPGS_INLINE CelestialBody& CelestialBody::SetRadius(float Radius)
{
	_Properties.Radius = Radius;
	return *this;
}

NPGS_INLINE CelestialBody& CelestialBody::SetSpin(float Spin)
{
	_Properties.Spin = Spin;
	return *this;
}

NPGS_INLINE CelestialBody& CelestialBody::SetOblateness(float Oblateness)
{
	_Properties.Oblateness = Oblateness;
	return *this;
}

NPGS_INLINE CelestialBody& CelestialBody::SetEscapeVelocity(float EscapeVelocity)
{
	_Properties.EscapeVelocity = EscapeVelocity;
	return *this;
}

NPGS_INLINE CelestialBody& CelestialBody::SetMagneticField(float MagneticField)
{
	_Properties.MagneticField = MagneticField;
	return *this;
}

NPGS_INLINE const glm::vec2& CelestialBody::GetNormal() const
{
	return _Properties.Normal;
}

NPGS_INLINE const std::string& CelestialBody::GetName() const
{
	return _Properties.Name;
}

NPGS_INLINE double CelestialBody::GetAge() const
{
	return _Properties.Age;
}

NPGS_INLINE float CelestialBody::GetRadius() const
{
	return _Properties.Radius;
}

NPGS_INLINE float CelestialBody::GetSpin() const
{
	return _Properties.Spin;
}

NPGS_INLINE float CelestialBody::GetOblateness() const
{
	return _Properties.Oblateness;
}

NPGS_INLINE float CelestialBody::GetEscapeVelocity() const
{
	return _Properties.EscapeVelocity;
}

NPGS_INLINE float CelestialBody::GetMagneticField() const
{
	return _Properties.MagneticField;
}

_ASTRO_END
_NPGS_END
