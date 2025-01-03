#pragma once

#include <cstddef>
#include <memory>
#include <random>
#include <vector>

#include <glm/glm.hpp>

#include "Engine/Core/Base/Base.h"
#include "Engine/Core/System/Generators/StellarGenerator.h"
#include "Engine/Core/System/Spatial/Octree.hpp"
#include "Engine/Core/Runtime/Threads/ThreadPool.h"
#include "Engine/Core/Types/Entries/Astro/Star.h"
#include "Engine/Core/Types/Entries/Astro/StellarSystem.h"
#include "Engine/Utils/Random.hpp"

_NPGS_BEGIN

class FUniverse
{
public:
    FUniverse() = delete;
    FUniverse(std::uint32_t Seed, std::size_t StarCount, std::size_t ExtraGiantCount = 0, std::size_t ExtraMassiveStarCount = 0,
              std::size_t ExtraNeutronStarCount = 0, std::size_t ExtraBlackHoleCount = 0, std::size_t ExtraMergeStarCount = 0,
              float UniverseAge = 1.38e10f);

    ~FUniverse();

    void FillUniverse();
    void ReplaceStar(std::size_t DistanceRank, const Astro::AStar& StarData);
    void CountStars();

private:
    template <typename AstroType, typename DataType>
    void MakeChunks(int MaxThread, std::vector<DataType>& Data, std::vector<std::vector<DataType>>& DataLists,
                    std::vector<std::promise<std::vector<AstroType>>>& Promises,
                    std::vector<std::future<std::vector<AstroType>>>& ChunkFutures);

    void GenerateStars(int MaxThread);
    void FillStellarSystem(int MaxThread);

    std::vector<Astro::AStar> InterpolateStars(int MaxThread, std::vector<System::Generator::FStellarGenerator>& Generators,
                                               std::vector<System::Generator::FStellarGenerator::FBasicProperties>& BasicProperties);

    void GenerateSlots(float MinDistance, std::size_t SampleCount, float Density);
    void OctreeLinkToStellarSystems(std::vector<Astro::AStar>& Stars, std::vector<glm::vec3>& Slots);
    void GenerateBinaryStars(int MaxThread);

private:
    using FNodeType = System::Spatial::TOctree<Astro::FStellarSystem>::FNodeType;

private:
    std::mt19937                                                     _RandomEngine;
    Util::TUniformIntDistribution<std::uint32_t>                     _SeedGenerator;
    Util::TUniformRealDistribution<>                                 _CommonGenerator;
    std::unique_ptr<System::Spatial::TOctree<Astro::FStellarSystem>> _Octree;
    Runtime::Thread::FThreadPool*                                    _ThreadPool;

    std::size_t _StarCount;
    std::size_t _ExtraGiantCount;
    std::size_t _ExtraMassiveStarCount;
    std::size_t _ExtraNeutronStarCount;
    std::size_t _ExtraBlackHoleCount;
    std::size_t _ExtraMergeStarCount;
    float       _UniverseAge;

    std::vector<Astro::FStellarSystem> _StellarSystems;
};

_NPGS_END
