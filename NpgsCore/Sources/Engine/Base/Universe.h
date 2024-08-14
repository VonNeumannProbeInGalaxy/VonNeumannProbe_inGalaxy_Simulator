#pragma once

#include <string>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

#include "Engine/Base/AstroObject/Star.h"
#include "Engine/Core/Modules/Stellar/StellarGenerator.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN

class NPGS_API Universe {
public:
    Universe() = default;
    ~Universe() = default;

    void AddStar(const std::string& Name, AstroObject::CelestialBody::BaryCenter& StarSys, const AstroObject::Star& Star);
    const std::vector<std::pair<std::string, AstroObject::CelestialBody::BaryCenter>>& GetStarSystems() const;

private:
    std::vector<std::pair<std::string, AstroObject::CelestialBody::BaryCenter>> _StarSystems;
};

_NPGS_END
