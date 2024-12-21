#include "CelestialObject.h"

#include <utility>

_NPGS_BEGIN
_ASTRO_BEGIN

FCelestialBody::FCelestialBody(const FBasicProperties& Properties)
	: _Properties(std::move(Properties))
{
}

_ASTRO_END
_NPGS_END
