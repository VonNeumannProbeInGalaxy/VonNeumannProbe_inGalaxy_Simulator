#pragma once

#include <memory>
#include <random>
#include <unordered_map>
#include <utility>

#include <glm/glm.hpp>

//#define BENCHMARK
#include "Engine/Base/AstroObject/Star.h"
#include "Engine/Core/AssetLoader/AssetManager.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_MODULES_BEGIN

class StellarGenerator {
public:
    using BaryCenter = AstroObject::CelestialBody::BaryCenter;
    using MistData   = Assets::Csv<double, 12>;

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

private:
    double DefaultPdf(double Mass);
    double GenMass(double MaxPdf);
    BasicProperties GenBasicProperties();
    std::vector<double> GetActuallyMistData(const BasicProperties& Properties);
    std::shared_ptr<MistData> LoadMistData(const std::string& Filename);
    std::vector<double> InterpolateMistData(const std::pair<std::string, std::string>& Files, double TargetAge, double MassFactor);
    std::vector<std::vector<double>> FindPhaseChanges(std::shared_ptr<MistData> DataCsv);
    std::pair<double, std::pair<double, double>> FindSurroundingTimePoints(const std::vector<std::vector<double>>& PhaseChanges, double TargetAge);
    std::pair<double, std::size_t> FindSurroundingTimePoints(const std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& PhaseChanges, double TargetAge, double MassFactor);
    double CalcEvolutionProgress(std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& PhaseChanges, double TargetAge, double MassFactor);
    std::vector<double> InterpolateRows(const std::shared_ptr<MistData>& Data, double EvolutionProgress);
    void AlignArrays(std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& Arrays);
    std::vector<double> InterpolateArray(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Factor);
    std::vector<double> InterpolateFinalData(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Factor);

private:
    std::mt19937 _RandomEngine;
    std::uniform_real_distribution<double> _UniformDistribution;
    std::vector<std::string> _MistHeaders{ "star_age", "star_mass", "star_mdot", "log_L", "log_Teff", "log_R", "log_surf_z", "v_wind_Km_per_s", "log_center_T", "log_center_Rho", "phase", "x" };
    std::unordered_map<std::shared_ptr<MistData>, std::vector<std::vector<double>>> _PhaseChangesCache;
};

_MODULES_END
_NPGS_END
