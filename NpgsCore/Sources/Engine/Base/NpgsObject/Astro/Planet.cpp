#include "Planet.h"

_NPGS_BEGIN
_ASTRO_BEGIN

Planet::Planet(const CelestialBody::BasicProperties& PlanetBasicProperties, const ExtendedProperties& PlanetExtraProperties)
    : CelestialBody(PlanetBasicProperties), _PlanetExtraProperties(PlanetExtraProperties)
{}

AsteroidCluster::AsteroidCluster(const BasicProperties& Properties)
    : _Properties(Properties)
{}

_ASTRO_END
_NPGS_END
