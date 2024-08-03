#pragma once

#include <random>
#include <unordered_map>

#include <glm/glm.hpp>

#include "Engine/Base/AstroObject/Star.h"
#include "Engine/Core/AssetLoader/Csv.hpp"
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_MODULES_BEGIN

class StellarGenerator {
public:
    using BaryCenter = AstroObject::CelestialBody::BaryCenter;
    using MesaData   = Assets::Csv<std::string, std::string, std::string, std::string, std::string, std::string,
                                   std::string, std::string, std::string, std::string, std::string, std::string>;

    struct BasicProperties {
        BaryCenter StarSys;
        double Age;
        double FeH;
        double Mass;

        explicit operator AstroObject::Star() const {
            AstroObject::Star Star;
            Star.SetParentBody(StarSys);
            Star.SetMass(Mass);
            Star.SetAge(Age);
            Star.SetFeH(FeH);

            return Star;
        }
    };

public:
    StellarGenerator() = default;
    StellarGenerator(int Seed);
    ~StellarGenerator() = default;

public:
    AstroObject::Star GenStar();
    AstroObject::Star operator=(const BasicProperties& Properties);

public: // public for test
    double DefaultPdf(double Mass);
    double GenMass(double MaxPdf);
    BasicProperties GenBasicProperties();
    std::vector<double> GetActuallyMesaData(const BasicProperties& Properties);
    bool BuildNumericMesaDataCache(const std::string& Filename);
    std::vector<double> InterpolateArrays(const std::vector<double>& Lower, const std::vector<double>& Upper, double Target);

private:
    std::mt19937 _RandomEngine;
    std::uniform_real_distribution<double> _UniformDistribution;
    std::unordered_map<std::string, std::vector<std::vector<double>>> _Cache;
};

_MODULES_END
_NPGS_END
