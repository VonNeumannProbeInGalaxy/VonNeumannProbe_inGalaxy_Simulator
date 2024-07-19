#include "Planet.h"

_NPGS_BEGIN

Planet::Planet(const CelestialBody::BaseProperties& PlanetBaseProperties, const ExtendedProperties& PlanetExtraProperties)
    : CelestialBody(PlanetBaseProperties), _PlanetExtraProperties(PlanetExtraProperties)
{}

_NPGS_END
