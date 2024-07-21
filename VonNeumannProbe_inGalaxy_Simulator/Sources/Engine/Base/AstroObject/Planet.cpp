#include "Planet.h"

_NPGS_BEGIN

namespace AstroObject {

Planet::Planet(const CelestialBody::BasicProperties& PlanetBasicProperties, const ExtendedProperties& PlanetExtraProperties)
    : CelestialBody(PlanetBasicProperties), _PlanetExtraProperties(PlanetExtraProperties)
{}

}

_NPGS_END
