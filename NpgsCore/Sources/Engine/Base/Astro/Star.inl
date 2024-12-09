#pragma once

#include "Star.h"

_NPGS_BEGIN
_ASTRO_BEGIN

NPGS_INLINE Star& Star::SetExtendedProperties(const ExtendedProperties& StarExtraProperties)
{
	_StarExtraProperties = StarExtraProperties;
	return *this;
}

NPGS_INLINE const Star::ExtendedProperties& Star::GetExtendedProperties() const
{
	return _StarExtraProperties;
}

NPGS_INLINE Star& Star::SetMass(double Mass)
{
	_StarExtraProperties.Mass = Mass;
	return *this;
}

NPGS_INLINE Star& Star::SetLuminosity(double Luminosity)
{
	_StarExtraProperties.Luminosity = Luminosity;
	return *this;
}

NPGS_INLINE Star& Star::SetLifetime(double Lifetime)
{
	_StarExtraProperties.Lifetime = Lifetime;
	return *this;
}

NPGS_INLINE Star& Star::SetEvolutionProgress(double EvolutionProgress)
{
	_StarExtraProperties.EvolutionProgress = EvolutionProgress;
	return *this;
}

NPGS_INLINE Star& Star::SetFeH(float FeH)
{
	_StarExtraProperties.FeH = FeH;
	return *this;
}

NPGS_INLINE Star& Star::SetInitialMass(float InitialMass)
{
	_StarExtraProperties.InitialMass = InitialMass;
	return *this;
}

NPGS_INLINE Star& Star::SetSurfaceH1(float SurfaceH1)
{
	_StarExtraProperties.SurfaceH1 = SurfaceH1;
	return *this;
}

NPGS_INLINE Star& Star::SetSurfaceZ(float SurfaceZ)
{
	_StarExtraProperties.SurfaceZ = SurfaceZ;
	return *this;
}

NPGS_INLINE Star& Star::SetSurfaceEnergeticNuclide(float SurfaceEnergeticNuclide)
{
	_StarExtraProperties.SurfaceEnergeticNuclide = SurfaceEnergeticNuclide;
	return *this;
}

NPGS_INLINE Star& Star::SetSurfaceVolatiles(float SurfaceVolatiles)
{
	_StarExtraProperties.SurfaceVolatiles = SurfaceVolatiles;
	return *this;
}

NPGS_INLINE Star& Star::SetTeff(float Teff)
{
	_StarExtraProperties.Teff = Teff;
	return *this;
}

NPGS_INLINE Star& Star::SetCoreTemp(float CoreTemp)
{
	_StarExtraProperties.CoreTemp = CoreTemp;
	return *this;
}

NPGS_INLINE Star& Star::SetCoreDensity(float CoreDensity)
{
	_StarExtraProperties.CoreDensity = CoreDensity;
	return *this;
}

NPGS_INLINE Star& Star::SetStellarWindSpeed(float StellarWindSpeed)
{
	_StarExtraProperties.StellarWindSpeed = StellarWindSpeed;
	return *this;
}

NPGS_INLINE Star& Star::SetStellarWindMassLossRate(float StellarWindMassLossRate)
{
	_StarExtraProperties.StellarWindMassLossRate = StellarWindMassLossRate;
	return *this;
}

NPGS_INLINE Star& Star::SetMinCoilMass(float MinCoilMass)
{
	_StarExtraProperties.MinCoilMass = MinCoilMass;
	return *this;
}

NPGS_INLINE Star& Star::SetIsSingleStar(bool bIsSingleStar)
{
	_StarExtraProperties.bIsSingleStar = bIsSingleStar;
	return *this;
}

NPGS_INLINE Star& Star::SetHasPlanets(bool bHasPlanets)
{
	_StarExtraProperties.bHasPlanets = bHasPlanets;
	return *this;
}

NPGS_INLINE Star& Star::SetStarFrom(StarFrom From)
{
	_StarExtraProperties.From = From;
	return *this;
}

NPGS_INLINE Star& Star::SetEvolutionPhase(EvolutionPhase Phase)
{
	_StarExtraProperties.Phase = Phase;
	return *this;
}

NPGS_INLINE Star& Star::SetStellarClass(const Util::StellarClass& StellarClass)
{
	_StarExtraProperties.StellarClass = StellarClass;
	return *this;
}

NPGS_INLINE double Star::GetMass() const
{
	return _StarExtraProperties.Mass;
}

NPGS_INLINE double Star::GetLuminosity() const
{
	return _StarExtraProperties.Luminosity;
}

NPGS_INLINE double Star::GetLifetime() const
{
	return _StarExtraProperties.Lifetime;
}

NPGS_INLINE double Star::GetEvolutionProgress() const
{
	return _StarExtraProperties.EvolutionProgress;
}

NPGS_INLINE float Star::GetFeH() const
{
	return _StarExtraProperties.FeH;
}

NPGS_INLINE float Star::GetInitialMass() const
{
	return _StarExtraProperties.InitialMass;
}

NPGS_INLINE float Star::GetSurfaceH1() const
{
	return _StarExtraProperties.SurfaceH1;
}

NPGS_INLINE float Star::GetSurfaceZ() const
{
	return _StarExtraProperties.SurfaceZ;
}

NPGS_INLINE float Star::GetSurfaceEnergeticNuclide() const
{
	return _StarExtraProperties.SurfaceEnergeticNuclide;
}

NPGS_INLINE float Star::GetSurfaceVolatiles() const
{
	return _StarExtraProperties.SurfaceVolatiles;
}

NPGS_INLINE float Star::GetTeff() const
{
	return _StarExtraProperties.Teff;
}

NPGS_INLINE float Star::GetCoreTemp() const
{
	return _StarExtraProperties.CoreTemp;
}

NPGS_INLINE float Star::GetCoreDensity() const
{
	return _StarExtraProperties.CoreDensity;
}

NPGS_INLINE float Star::GetStellarWindSpeed() const
{
	return _StarExtraProperties.StellarWindSpeed;
}

NPGS_INLINE float Star::GetStellarWindMassLossRate() const
{
	return _StarExtraProperties.StellarWindMassLossRate;
}

NPGS_INLINE float Star::GetMinCoilMass() const
{
	return _StarExtraProperties.MinCoilMass;
}

NPGS_INLINE bool Star::GetIsSingleStar() const
{
	return _StarExtraProperties.bIsSingleStar;
}

NPGS_INLINE bool Star::GetHasPlanets() const
{
	return _StarExtraProperties.bHasPlanets;
}

NPGS_INLINE Star::StarFrom Star::GetStarFrom() const
{
	return _StarExtraProperties.From;
}

NPGS_INLINE Star::EvolutionPhase Star::GetEvolutionPhase() const
{
	return _StarExtraProperties.Phase;
}

NPGS_INLINE const Util::StellarClass& Star::GetStellarClass() const
{
	return _StarExtraProperties.StellarClass;
}

_ASTRO_END
_NPGS_END
