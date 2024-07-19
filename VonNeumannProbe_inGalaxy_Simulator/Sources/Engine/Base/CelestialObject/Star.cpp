#include "Star.h"

_NPGS_BEGIN

Star::Star(const CelestialBody::BaseProperties& StarBaseProperties, const ExtendedProperties& StarExtraProperties)
    : CelestialBody(StarBaseProperties), _StarExtraProperties(StarExtraProperties)
{}

_NPGS_END
