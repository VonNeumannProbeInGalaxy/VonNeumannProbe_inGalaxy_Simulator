#include "Star.h"

Star::Star(const CelestialBody::BaseProperties& StarBaseProperties, const ExtendedProperties& StarExtraProperties)
    : CelestialBody(StarBaseProperties), _StarBaseProperties(StarBaseProperties), _StarExtraProperties(StarExtraProperties)
{}
