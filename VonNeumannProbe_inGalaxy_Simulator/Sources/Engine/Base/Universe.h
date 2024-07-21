#pragma once

#include <string>
#include <tuple>
#include <vector>

#include <glm/glm.hpp>

#include "Engine/Base/AstroObject/Star.h"
#include "Engine/Core/Modules/Stellar/StellarGenerator.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN

class Universe {
public:
    Universe() = default;
    ~Universe() = default;

public:
    void AddStar(const std::string& Name, AstroObject::CelestialBody::BaryCenter& StarSys, const AstroObject::Star& Star);

    const std::vector<std::tuple<std::string, AstroObject::CelestialBody::BaryCenter>>& GetStarSystems() const {
        return _StarSystems;
    }

private:
    std::vector<std::tuple<std::string, AstroObject::CelestialBody::BaryCenter>> _StarSystems;
};

_NPGS_END
