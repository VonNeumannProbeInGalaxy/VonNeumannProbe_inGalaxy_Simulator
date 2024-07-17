#include "Planet.h"

Planet::Planet(const CelestialBody::BaseProperties& PlanetBaseProperties, const ExtendedProperties& PlanetExtraProperties) 
    : CelestialBody(PlanetBaseProperties), _PlanetExtraProperties(PlanetExtraProperties)
{}
