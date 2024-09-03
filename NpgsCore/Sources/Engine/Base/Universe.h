#pragma once

#include <memory>
#include <random>
#include <vector>

#include <glm/glm.hpp>

#include "Engine/Base/StellarSystem.h"
#include "Engine/Base/Octree.hpp"
#include "Engine/Core/Base.h"
#include "Engine/Core/ThreadPool.h"
#include "Engine/Core/Random.hpp"

_NPGS_BEGIN

class NPGS_API Universe {
public:
    Universe(unsigned Seed, std::size_t NumStars, std::size_t NumExtraGiants = 0, std::size_t NumExtraMassiveStars = 0, std::size_t NumExtraNeutronStars = 0, std::size_t NumExtraBlackHoles = 0, std::size_t NumExtraMergeStars = 0, float UniverseAge = 1.38e10f);
    ~Universe();

    void FillUniverse();
    void ReplaceStar(std::size_t DistanceRank, const AstroObject::Star& StarData);
    void CountStars() const;

private:
    void GenerateSlots(float DistMin, std::size_t NumSamples, float Density);
    void OctreeLinkToStars(std::vector<AstroObject::Star>& Stars, std::vector<glm::vec3>& Slots);

private:
    using NodeType = Octree<StellarSystem>::NodeType;

    std::mt19937                               _RandomEngine;
    std::unique_ptr<Octree<StellarSystem>>     _Octree;
    std::vector<AstroObject::Star>             _Stars;
    std::vector<StellarSystem>                 _StellarSystems;
    ThreadPool*                                _ThreadPool;
    UniformRealDistribution<float>             _CommonGenerator;
    UniformRealDistribution<float>             _SeedGenerator;

    std::size_t _NumStars;
    std::size_t _NumExtraGiants;
    std::size_t _NumExtraMassiveStars;
    std::size_t _NumExtraNeutronStars;
    std::size_t _NumExtraBlackHoles;
    std::size_t _NumExtraMergeStars;
    float       _UniverseAge;
};

_NPGS_END
