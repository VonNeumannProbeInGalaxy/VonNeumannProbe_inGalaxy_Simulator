module;

#include <cstdint>
#include <glm/glm.hpp>
#include "Engine/Core/Base.h"

export module Universe;

import <future>;
import <memory>;
import <random>;
import <vector>;

import Base.Astro.Star;
import Base.Octree;
import Base.StellarSystem;
import Core.Random;
import Core.ThreadPool;

_NPGS_BEGIN

export class Universe {
public:
    Universe() = delete;
    Universe(
        unsigned    Seed,
        std::size_t NumStars,
        std::size_t NumExtraGiants       = 0,
        std::size_t NumExtraMassiveStars = 0,
        std::size_t NumExtraNeutronStars = 0,
        std::size_t NumExtraBlackHoles   = 0,
        std::size_t NumExtraMergeStars   = 0,
        float       UniverseAge          = 1.38e10f
    );

    ~Universe();

    void FillUniverse();
    void ReplaceStar(std::size_t DistanceRank, const Astro::Star& StarData);
    void CountStars();

private:
    void GenerateSlots(float MinDistance, std::size_t NumSamples, float Density);
    void OctreeLinkToStellarSystems(std::vector<Astro::Star>& Stars, std::vector<glm::vec3>& Slots);
    void GenerateBinaryStars(int MaxThread);

private:
    using NodeType = Octree<StellarSystem>::NodeType;

    std::mt19937                           _RandomEngine;
    UniformIntDistribution<std::uint32_t>  _SeedGenerator;
    UniformRealDistribution<>              _CommonGenerator;
    std::unique_ptr<Octree<StellarSystem>> _Octree;
    ThreadPool*                            _ThreadPool;

    std::size_t _NumStars;
    std::size_t _NumExtraGiants;
    std::size_t _NumExtraMassiveStars;
    std::size_t _NumExtraNeutronStars;
    std::size_t _NumExtraBlackHoles;
    std::size_t _NumExtraMergeStars;
    float       _UniverseAge;

    std::vector<StellarSystem> _StellarSystems;
};

_NPGS_END
