#pragma once

#include <memory>
#include <random>
#include <vector>

#include <glm/glm.hpp>

#include "Engine/Base/AstroObject/Star.h"
#include "Engine/Base/Octree.hpp"
#include "Engine/Core/Modules/Stellar/StellarGenerator.h"
#include "Engine/Core/Base.h"
#include "Engine/Core/ThreadPool.h"
#include "Engine/Core/Random.hpp"

_NPGS_BEGIN

class NPGS_API Universe {
public:
    Universe(unsigned Seed, std::size_t NumStars, std::size_t NumExtraGiants = 0, std::size_t NumExtraMassiveStars = 0, std::size_t NumExtraNeutronStars = 0, std::size_t NumExtraBlackHoles = 0, std::size_t NumExtraMergeStars = 0, double UniverseAge = 1.38e10);
    ~Universe();

    const std::vector<AstroObject::Star>& FillUniverse();

private:
    void GenerateSlots(int SampleLimit, std::size_t NumSamples, float Density);
    void GenerateSlots(float DistMin, std::size_t NumSamples, float Density);
    void OctreeLinkToStars(std::vector<AstroObject::Star>& Stars, std::vector<glm::vec3>& Slots) const;

private:
    using NodeType = Octree<AstroObject::Star>::NodeType;

    std::mt19937                               _RandomEngine;
    std::unique_ptr<Octree<AstroObject::Star>> _StellarOctree;
    std::vector<AstroObject::Star>             _Stars;
    ThreadPool*                                _ThreadPool;
    UniformRealDistribution<float>             _CommonGenerator;
    UniformRealDistribution<float>             _SeedGenerator;

    std::size_t _NumStars;
    std::size_t _NumExtraGiants;
    std::size_t _NumExtraMassiveStars;
    std::size_t _NumExtraNeutronStars;
    std::size_t _NumExtraBlackHoles;
    std::size_t _NumExtraMergeStars;
    double      _UniverseAge;
};

_NPGS_END
