#include "CelestialObject.h"

_NPGS_BEGIN
_ASTROOBJECT_BEGIN

// Implementations for CelestialBody
// ---------------------------------
CelestialBody::CelestialBody(const BasicProperties& Properties) : _Properties(Properties) {}

// Implementations for MolecularCloud
// ----------------------------------
MolecularCloud::MolecularCloud(const BasicProperties& Properties) : _Properties(Properties) {}

_ASTROOBJECT_END
_NPGS_END
