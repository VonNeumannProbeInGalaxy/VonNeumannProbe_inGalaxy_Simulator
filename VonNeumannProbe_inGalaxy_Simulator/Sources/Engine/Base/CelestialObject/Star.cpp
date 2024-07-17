#include "Star.h"

Star::Star(const CelestialBody::BaseProperties& StarBaseProperties, const ExtendedProperties& StarExtraProperties)
    : CelestialBody(StarBaseProperties), _StarExtraProperties(StarExtraProperties)
{}
