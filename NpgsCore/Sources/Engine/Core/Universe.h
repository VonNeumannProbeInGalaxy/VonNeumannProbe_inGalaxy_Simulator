#pragma once

#include <memory>
#include <random>
#include <vector>

#include <glm/glm.hpp>

#include "Engine/Base/Astro/Star.h"
#include "Engine/Base/Astro/StellarSystem.h"
#include "Engine/Core/Base.h"
#include "Engine/Core/Octree.hpp"
#include "Engine/Core/ThreadPool.h"
#include "Engine/Modules/Generators/StellarGenerator.h"
#include "Engine/Utilities/Random.hpp"

_NPGS_BEGIN

class Universe {
public:
    Universe() = delete;
    Universe(
        std::uint32_t Seed,
        std::size_t   StarCount,
        std::size_t   ExtraGiantCount       = 0,
        std::size_t   ExtraMassiveStarCount = 0,
        std::size_t   ExtraNeutronStarCount = 0,
        std::size_t   ExtraBlackHoleCount   = 0,
        std::size_t   ExtraMergeStarCount   = 0,
        float         UniverseAge           = 1.38e10f
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

    std::vector<Astro::Star> InterpolateStars(
        int MaxThread,
        std::vector<Module::StellarGenerator>& Generators,
        std::vector<Module::StellarGenerator::BasicProperties>& BasicProperties
    );

    void GenerateSlots(float MinDistance, std::size_t SampleCount, float Density);
    void OctreeLinkToStellarSystems(std::vector<Astro::Star>& Stars, std::vector<glm::vec3>& Slots);
    void GenerateBinaryStars(int MaxThread);

private:
    using NodeType = Octree<Astro::StellarSystem>::NodeType;

private:
    std::mt19937                                  _RandomEngine;
    Util::UniformIntDistribution<std::uint32_t>   _SeedGenerator;
    Util::UniformRealDistribution<>               _CommonGenerator;
    std::unique_ptr<Octree<Astro::StellarSystem>> _Octree;
    ThreadPool*                                   _ThreadPool;

    std::size_t _StarCount;
    std::size_t _ExtraGiantCount;
    std::size_t _ExtraMassiveStarCount;
    std::size_t _ExtraNeutronStarCount;
    std::size_t _ExtraBlackHoleCount;
    std::size_t _ExtraMergeStarCount;
    float       _UniverseAge;

    std::vector<Astro::StellarSystem> _StellarSystems;
};

_NPGS_END