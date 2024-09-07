#include "Planet.h"

_NPGS_BEGIN
_ASTROOBJECT_BEGIN

Planet::Planet(const CelestialBody::BasicProperties& PlanetBasicProperties, const ExtendedProperties& PlanetExtraProperties)
    : CelestialBody(PlanetBasicProperties), _PlanetExtraProperties(PlanetExtraProperties)
{}

const float Planet::_kNull                             = 0.0f;
const float Planet::_kCarbonBasedUniversalIntelligence = 1.0f;
const float Planet::_kUrgesellschaft                   = 2.0f;
const float Planet::_kPrevIndustrielle                 = 3.0f;
const float Planet::_kSteamAge                         = 4.0f;
const float Planet::_kElectricAge                      = 5.0f;
const float Planet::_kAtomicAge                        = 6.0f;
const float Planet::_kDigitalAge                       = 7.0f;
const float Planet::_kPrevAsiAge                       = 8.0f;

_ASTROOBJECT_END
_NPGS_END
