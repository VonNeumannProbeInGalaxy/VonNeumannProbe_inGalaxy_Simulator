#pragma once

#include <array>
#include <memory>
#include <random>
#include <shared_mutex>
#include <unordered_map>
#include <utility>

#include "Engine/Base/NpgsObject/Astro/Star.h"
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

    enum class GenerateDistribution {
        kFromPdf,
        kUniform,
        kUniformByExponent
    };

    enum class GenerateOption {
        kNormal,
        kGiant,
        kDeathStar,
        kMergeStar
    };

    struct BasicProperties {
        float Age = 0.0f;
        float FeH = 0.0f;
        float InitialMass = 0.0f;

        GenerateOption Option; // 用于保存生成选项，类的生成选项仅影响该属性。生成的恒星完整信息也将根据该属性决定。该选项用于防止多线程生成恒星时属性和生成器胡乱匹配

        explicit operator Astro::Star() const {
            Astro::Star Star;
            Star.SetAge(Age);
            Star.SetFeH(FeH);
            Star.SetInitialMass(InitialMass);

            return Star;
        }
    };

public:
    StellarGenerator() = delete;
    explicit StellarGenerator(
        const std::seed_seq& SeedSequence, GenerateOption Option = GenerateOption::kNormal, float UniverseAge = 1.38e10f,
        float MassLowerLimit =  0.1f,      float MassUpperLimit = 300.0f,   GenerateDistribution MassDistribution = GenerateDistribution::kFromPdf,
        float AgeLowerLimit  =  0.0f,      float AgeUpperLimit  = 1.26e10f, GenerateDistribution AgeDistribution  = GenerateDistribution::kFromPdf,
        float FeHLowerLimit  = -4.0f,      float FeHUpperLimit  = 0.5f,     GenerateDistribution FeHDistribution  = GenerateDistribution::kFromPdf,
        float CoilTempLimit  =  1514.114f, float dEpdM          = 2e6f
    );

    ~StellarGenerator() = default;

public:
    BasicProperties GenerateBasicProperties();
    Astro::Star GenerateStar();
    Astro::Star GenerateStar(BasicProperties&  Properties);
    Astro::Star GenerateStar(BasicProperties&& Properties);

private:
    void InitMistData();

    // Processor functions, as member functions to access class members
    // ----------------------------------------------------------------
    float GenerateAge(float MaxPdf);
    float GenerateMass(float MaxPdf, bool bIsBinary);
    std::vector<double> GetActuallyMistData(const BasicProperties& Properties, bool bIsWhiteDwarf, bool bIsSingleWd);
    std::vector<double> InterpolateMistData(const std::pair<std::string, std::string>& Files, double TargetAge, double TargetMass, double MassCoefficient);
    std::vector<std::vector<double>> FindPhaseChanges(const std::shared_ptr<MistData>& DataCsv);
    void CalculateSpectralType(float FeH, Astro::Star& StarData);
    StellarClass::LuminosityClass CalculateLuminosityClass(const Astro::Star& StarData);
    void ProcessDeathStar(Astro::Star& DeathStar, double MergeStarProbability = 0.005);
    void GenerateMagnetic(Astro::Star& StarData);
    void GenerateSpin(Astro::Star& StarData);

    template <typename CsvType>
    static std::shared_ptr<CsvType> LoadCsvAsset(const std::string& Filename, const std::vector<std::string>& Headers);

private:
    std::mt19937 _RandomEngine;
    std::array<UniformRealDistribution<>, 8>       _MagneticGenerators;
    std::array<std::shared_ptr<Distribution<>>, 4> _FeHGenerators;
    std::array<UniformRealDistribution<>, 2>       _SpinGenerators;
    UniformRealDistribution<> _AgeGenerator;
    UniformRealDistribution<> _CommonGenerator;
    UniformRealDistribution<> _LogMassGenerator;

    float _UniverseAge;
    float _AgeLowerLimit;
    float _AgeUpperLimit;
    float _FeHLowerLimit;
    float _FeHUpperLimit;
    float _MassLowerLimit;
    float _MassUpperLimit;
    float _CoilTempLimit;
    float _dEpdM;

    GenerateDistribution _AgeDistribution;
    GenerateDistribution _FeHDistribution;
    GenerateDistribution _MassDistribution;
    GenerateOption       _Option;

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
    static const std::vector<std::string> _kMistHeaders;
    static const std::vector<std::string> _kWdMistHeaders;
    static const std::vector<std::string> _kHrDiagramHeaders;
    static std::unordered_map<std::string, std::vector<float>> _kMassFileCache;
    static std::unordered_map<std::shared_ptr<MistData>, std::vector<std::vector<double>>> _kPhaseChangesCache;
    static std::shared_mutex _kCacheMutex;
    static bool _kbMistDataInitiated;
};

_MODULES_END
_NPGS_END
