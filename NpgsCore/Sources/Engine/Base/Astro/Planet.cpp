#include "Planet.h"

_NPGS_BEGIN
_ASTRO_BEGIN

APlanet::APlanet(const FCelestialBody::FBasicProperties& BasicProperties, FExtendedProperties&& ExtraProperties)
	: FCelestialBody(BasicProperties), _ExtraProperties(std::move(ExtraProperties))
{
}

AAsteroidCluster::AAsteroidCluster(const FBasicProperties& Properties)
	: _Properties(Properties)
{
}

_ASTRO_END
_NPGS_END
