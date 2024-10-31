#pragma once

#include <memory>
#include <random>
#include <vector>

#include <glm/glm.hpp>

#include "Engine/Base/NpgsObject/Astro/Star.h"
#include "Engine/Base/Octree.hpp"
#include "Engine/Base/StellarSystem.h"
#include "Engine/Core/Modules/StellarGenerator.h"
#include "Engine/Core/Base.h"
#include "Engine/Core/Random.hpp"
#include "Engine/Core/ThreadPool.h"

_NPGS_BEGIN

class Universe {
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
    template <typename AstroType, typename DataType>
    void MakeChunks(
        int MaxThread,
        std::vector<DataType>& Data,
        std::vector<std::vector<DataType>>& DataLists,
        std::vector<std::promise<std::vector<AstroType>>>& Promises,
        std::vector<std::future<std::vector<AstroType>>>& ChunkFutures
    );

    void GenerateStars(int MaxThread);
    void FillStellarSystem(int MaxThread);
    std::vector<Astro::Star> InterpolateStars(int MaxThread, std::vector<Module::StellarGenerator>& Generators, std::vector<Module::StellarGenerator::BasicProperties>& BasicProperties);
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
