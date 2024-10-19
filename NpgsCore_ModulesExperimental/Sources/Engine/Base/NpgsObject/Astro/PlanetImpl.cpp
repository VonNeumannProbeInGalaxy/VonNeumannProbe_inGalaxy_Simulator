module;

#include "Engine/Core/Base.h"

module Base.Astro.Planet;

_NPGS_BEGIN
_ASTRO_BEGIN

Planet::Planet(const CelestialBody::BasicProperties& PlanetBasicProperties, ExtendedProperties&& PlanetExtraProperties)
    : CelestialBody(PlanetBasicProperties), _PlanetExtraProperties(std::move(PlanetExtraProperties))
{}

AsteroidCluster::AsteroidCluster(const BasicProperties& Properties)
    : _Properties(Properties)
{}

_ASTRO_END
_NPGS_END
