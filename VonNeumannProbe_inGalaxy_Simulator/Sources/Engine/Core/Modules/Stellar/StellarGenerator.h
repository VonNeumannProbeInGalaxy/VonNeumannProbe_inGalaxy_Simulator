#pragma once

#include <random>

#include <glm/glm.hpp>

#include "Engine/Base/AstroObject/Star.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN

namespace Modules {

class StellarGenerator {
public:
    using BaryCenter = AstroObject::CelestialBody::BaryCenter;

    struct BasicProperties {
        BaryCenter StarSys;
        double Age;
        double FeH;
        double Mass;
    };

public:
    StellarGenerator() = default;
    StellarGenerator(int Seed);
    ~StellarGenerator() = default;

public: // public for test
    double ProbabilityDensity(double Mass);
    double GenMass(double MaxPdf);
    BasicProperties GenBasicProperties();

private:
    std::mt19937 _RandomEngine;
    std::uniform_real_distribution<double> _UniformDistribution;
};

}

_NPGS_END
