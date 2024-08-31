#include "StellarSystem.h"

_NPGS_BEGIN

StellarSystem::StellarSystem(const BaryCenter& SystemBary, const std::vector<AstroObject::Star>& Stars, const std::vector<AstroObject::Planet>& Planets)
    : _SystemBary(SystemBary), _Stars(Stars), _Planets(Planets)
{}

_NPGS_END
