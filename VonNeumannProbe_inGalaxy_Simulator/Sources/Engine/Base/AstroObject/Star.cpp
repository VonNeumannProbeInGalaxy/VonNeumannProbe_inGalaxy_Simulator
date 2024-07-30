#include "Star.h"

_NPGS_BEGIN
_ASTROOBJECT_BEGIN

Star::Star(const CelestialBody::BasicProperties& StarBasicProperties, const ExtendedProperties& StarExtraProperties)
    : CelestialBody(StarBasicProperties), _StarExtraProperties(StarExtraProperties)
{}

_ASTROOBJECT_END
_NPGS_END
