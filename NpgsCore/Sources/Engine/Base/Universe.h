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
#include "Engine/Core/ThreadPool.h"
#include "Engine/Core/Random.hpp"

_NPGS_BEGIN

class NPGS_API Universe {
public:
    Universe(int Seed);
    Universe(std::random_device& RandomDevice);
    ~Universe();

    void FillStar(int NumStars);

private:
    void GenerateSlots(int SampleLimit, std::size_t NumSamples, float Density);
    void GenerateSlots(float DistMin, std::size_t NumSamples, float Density);
    
    template <typename Ty>
    void CopyToVector(std::vector<Ty>& Stars) const;

private:
    int                            _Seed;
    std::mt19937                   _RandomEngine;
    std::unique_ptr<Octree>        _StellarOctree;
    ThreadPool*                    _ThreadPool;
    UniformRealDistribution<float> _Dist;
};

_NPGS_END
