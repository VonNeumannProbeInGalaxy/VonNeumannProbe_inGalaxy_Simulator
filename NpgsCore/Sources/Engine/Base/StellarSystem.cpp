#include "StellarSystem.h"

_NPGS_BEGIN

StellarSystem::StellarSystem(const BaryCenter& SystemBary, const std::vector<Astro::Star>& Stars, const std::vector<Astro::Planet>& Planets)
    : _SystemBary(SystemBary), _Stars(Stars), _Planets(Planets)
{}

_NPGS_END
