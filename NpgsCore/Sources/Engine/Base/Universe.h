#pragma once

#include <memory>
#include <random>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Engine/Base/AstroObject/Star.h"
#include "Engine/Base/Octree.h"
#include "Engine/Core/Modules/Stellar/StellarGenerator.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN

class NPGS_API Universe {
public:
    Universe(int Seed);
    Universe(std::random_device& RandomDevice);
    ~Universe() = default;

    void AddStar(const std::string& Name, AstroObject::CelestialBody::BaryCenter& StarSys, const AstroObject::Star& Star);

// private:
    void GenerateSlots(float PointRadius, int SampleLimit, int NumSample, float Density);

private:
    std::vector<AstroObject::CelestialBody::BaryCenter> _StarSystems;

    std::mt19937 _RandomEngine;
    UniformRealDistribution<float> _Dist;

public: // for debug
    std::unique_ptr<Octree> _StarOctree;
};

_NPGS_END
