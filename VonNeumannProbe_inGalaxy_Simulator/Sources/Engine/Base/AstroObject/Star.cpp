#include "Star.h"

_NPGS_BEGIN

namespace AstroObject {

Star::Star(const CelestialBody::BasicProperties& StarBasicProperties, const ExtendedProperties& StarExtraProperties)
    : CelestialBody(StarBasicProperties), _StarExtraProperties(StarExtraProperties)
{}

}

_NPGS_END
