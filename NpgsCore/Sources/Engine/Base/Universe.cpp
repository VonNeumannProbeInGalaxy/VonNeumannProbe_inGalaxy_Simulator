#include "Universe.h"

_NPGS_BEGIN

void Universe::AddStar(const std::string& Name, AstroObject::CelestialBody::BaryCenter& StarSys, const AstroObject::Star& Star) {}

const std::vector<std::pair<std::string, AstroObject::CelestialBody::BaryCenter>>& Universe::GetStarSystems() const {
    return _StarSystems;
}

_NPGS_END

