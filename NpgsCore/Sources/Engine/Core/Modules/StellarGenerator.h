#pragma once

#include <array>
#include <memory>
#include <random>
#include <shared_mutex>
#include <unordered_map>
#include <utility>

#include "Engine/Base/AstroObject/Star.h"
#include "Engine/Core/AssetLoader/AssetManager.h"
#include "Engine/Core/Base.h"
#include "Engine/Core/Random.hpp"

_NPGS_BEGIN
_MODULES_BEGIN

class NPGS_API StellarGenerator {
public:
    using MistData   = Assets::Csv<double, 12>;
    using WdMistData = Assets::Csv<double, 5>;
    using HrDiagram  = Assets::Csv<double, 7>;

    enum class GenDistribution {
        kFromPdf,
        kUniform,
        kUniformByExponent
    };

    enum class GenOption {
        kNormal,
        kGiant,
        kDeathStar,
        kMergeStar
    };

    struct BasicProperties {
        float Age;
        float FeH;
        float InitialMass;

        GenOption Option;

        explicit operator AstroObject::Star() const {
            AstroObject::Star Star;
            Star.SetAge(Age);
            Star.SetFeH(FeH);
            Star.SetInitialMass(InitialMass);

            return Star;
        }
    };

public:
    StellarGenerator() = default;
    StellarGenerator(const std::seed_seq& SeedSeq, GenOption Option = GenOption::kNormal, float UniverseAge = 1.38e10f,
        float MassLowerLimit =  0.1f,     float MassUpperLimit = 300.0f,   GenDistribution MassDistribution = GenDistribution::kFromPdf,
        float AgeLowerLimit  =  0.0f,     float AgeUpperLimit  = 1.26e10f, GenDistribution AgeDistribution  = GenDistribution::kFromPdf,
        float FeHLowerLimit  = -4.0f,     float FeHUpperLimit  = 0.5f,     GenDistribution FeHDistribution  = GenDistribution::kFromPdf,
        float CoilTempLimit  = 1514.114f, float dEpdM          = 2e6f);
    ~StellarGenerator() = default;

public:
    BasicProperties GenBasicProperties();
    AstroObject::Star GenerateStar();
    AstroObject::Star GenerateStar(BasicProperties& Properties);

private:
    template <typename CsvType>
    static std::shared_ptr<CsvType> LoadCsvAsset(const std::string& Filename, const std::vector<std::string>& Headers);

    void InitMistData();
    float GenerateAge(float MaxPdf);
    float GenerateMass(float MaxPdf, bool bIsBinary);
    std::vector<double> GetActuallyMistData(const BasicProperties& Properties, bool bIsWhiteDwarf, bool bIsSingleWd);
    std::vector<double> InterpolateMistData(const std::pair<std::string, std::string>& Files, double TargetAge, double TargetMass, double MassFactor);
    std::vector<std::vector<double>> FindPhaseChanges(const std::shared_ptr<MistData>& DataCsv);
    void CalcSpectralType(AstroObject::Star& StarData, float FeH);
    StellarClass::LuminosityClass CalcLuminosityClass(const AstroObject::Star& StarData);
    void ProcessDeathStar(AstroObject::Star& DeathStar, double MergeStarProbability = 0.005);
    void GenerateMagnetic(AstroObject::Star& StarData);
    void GenerateSpin(AstroObject::Star& StarData);

public:
    static const int _kStarAgeIndex;
    static const int _kStarMassIndex;
    static const int _kStarMdotIndex;
    static const int _kLogTeffIndex;
    static const int _kLogRIndex;
    static const int _kLogSurfZIndex;
    static const int _kSurfaceH1Index;
    static const int _kSurfaceHe3Index;
    static const int _kLogCenterTIndex;
    static const int _kLogCenterRhoIndex;
    static const int _kPhaseIndex;
    static const int _kXIndex;
    static const int _kLifetimeIndex;

    static const int _kWdStarAgeIndex;
    static const int _kWdLogRIndex;
    static const int _kWdLogTeffIndex;
    static const int _kWdLogCenterTIndex;
    static const int _kWdLogCenterRhoIndex;

private:
    std::mt19937 _RandomEngine;
    UniformRealDistribution<float> _AgeGenerator;
    UniformRealDistribution<float> _LogMassGenerator;
    UniformRealDistribution<float> _CommonGenerator;
    std::array<std::shared_ptr<Distribution<float>>, 4> _FeHGenerators;
    std::array<std::shared_ptr<Distribution<float>>, 8> _MagneticGenerators;
    std::array<std::shared_ptr<Distribution<float>>, 2> _SpinGenerators;

    float _UniverseAge;
    float _AgeLowerLimit;
    float _AgeUpperLimit;
    float _FeHLowerLimit;
    float _FeHUpperLimit;
    float _MassLowerLimit;
    float _MassUpperLimit;
    float _CoilTempLimit;
    float _dEpdM;

    GenDistribution _MassDistribution;
    GenDistribution _AgeDistribution;
    GenDistribution _FeHDistribution;
    GenOption       _Option;

    static const std::vector<std::string> _kMistHeaders;
    static const std::vector<std::string> _kWdMistHeaders;
    static const std::vector<std::string> _kHrDiagramHeaders;
    static std::unordered_map<std::string, std::vector<float>>                             _kMassFileCache;
    static std::unordered_map<std::shared_ptr<MistData>, std::vector<std::vector<double>>> _kPhaseChangesCache;
    static std::shared_mutex                                                               _kCacheMutex;
    static bool _kbMistDataInitiated;
};

_MODULES_END
_NPGS_END
