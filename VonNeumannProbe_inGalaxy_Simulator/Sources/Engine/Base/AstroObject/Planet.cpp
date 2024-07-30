#include "Planet.h"

_NPGS_BEGIN
_ASTROOBJECT_BEGIN

Planet::Planet(const CelestialBody::BasicProperties& PlanetBasicProperties, const ExtendedProperties& PlanetExtraProperties)
    : CelestialBody(PlanetBasicProperties), _PlanetExtraProperties(PlanetExtraProperties)
{}

_ASTROOBJECT_END
_NPGS_END
