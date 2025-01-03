#pragma once

#include <cstddef>
#include <array>
#include <functional>
#include <memory>
#include <random>
#include <shared_mutex>
#include <unordered_map>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

#include "Engine/Core/Base/Base.h"
#include "Engine/Core/Runtime/Assets/CommaSeparatedValues.hpp"
#include "Engine/Core/Types/Entries/Astro/Star.h"
#include "Engine/Core/Types/Properties/StellarClass.h"
#include "Engine/Utils/Random.hpp"

_NPGS_BEGIN
_SYSTEM_BEGIN
_GENERATOR_BEGIN

class FStellarGenerator
{
public:
    using FMistData   = Runtime::Asset::TCommaSeparatedValues<double, 12>;
    using FWdMistData = Runtime::Asset::TCommaSeparatedValues<double, 5>;
    using FHrDiagram  = Runtime::Asset::TCommaSeparatedValues<double, 7>;

    enum class EGenerateDistribution
    {
        kFromPdf,
        kUniform,
        kUniformByExponent
    };

    enum class EGenerateOption
    {
        kNormal,
        kGiant,
        kDeathStar,
        kMergeStar,
        kBinaryFirstStar,
        kBinarySecondStar,
    };

    struct FBasicProperties
    {
        float Age{};
        float FeH{};
        float InitialMassSol{};
        bool  bIsSingleStar{ true };

        // 用于保存生成选项，类的生成选项仅影响该属性。生成的恒星完整信息也将根据该属性决定。该选项用于防止多线程生成恒星时属性和生成器胡乱匹配
        EGenerateOption MultiOption;
        EGenerateOption TypeOption;

        explicit operator Astro::AStar() const
        {
            Astro::AStar Star;
            Star.SetAge(Age);
            Star.SetFeH(FeH);
            Star.SetInitialMass(InitialMassSol);
            Star.SetIsSingleStar(bIsSingleStar);

            return Star;
        }
    };

public:
    FStellarGenerator() = delete;
    explicit FStellarGenerator(const std::seed_seq& SeedSequence, EGenerateOption Option = EGenerateOption::kNormal,
                               float UniverseAge = 1.38e10f,
                               float MassLowerLimit = 0.1f, float MassUpperLimit = 300.0f,
                               EGenerateDistribution MassDistribution = EGenerateDistribution::kFromPdf,
                               float AgeLowerLimit = 0.0f, float AgeUpperLimit = 1.26e10f,
                               EGenerateDistribution AgeDistribution = EGenerateDistribution::kFromPdf,
                               float FeHLowerLimit = -4.0f, float FeHUpperLimit = 0.5f,
                               EGenerateDistribution FeHDistribution = EGenerateDistribution::kFromPdf,
                               float CoilTemperatureLimit = 1514.114f, float dEpdM = 2e6f,
                               const std::function<float(const glm::vec3&, float, float)>& AgePdf = nullptr,
                               const glm::vec2& AgeMaxPdf = glm::vec2(),
                               const std::array<std::function<float(float)>, 2>& MassPdfs = { nullptr, nullptr },
                               const std::array<glm::vec2, 2>& MassMaxPdfs = { glm::vec2(), glm::vec2() });

    FStellarGenerator(const FStellarGenerator&)     = delete;
    FStellarGenerator(FStellarGenerator&&) noexcept = default;
    ~FStellarGenerator()                            = default;

    FStellarGenerator& operator=(const FStellarGenerator&)     = delete;
    FStellarGenerator& operator=(FStellarGenerator&&) noexcept = default;

    FBasicProperties GenerateBasicProperties();
    FBasicProperties GenerateBasicProperties(float Age, float FeH);
    Astro::AStar GenerateStar();
    Astro::AStar GenerateStar(FBasicProperties& Properties);
    Astro::AStar GenerateStar(FBasicProperties&& Properties);

    FStellarGenerator& SetLogMassSuggestDistribution(std::unique_ptr<Util::TDistribution<>> Distribution);
    FStellarGenerator& SetUniverseAge(float Age);
    FStellarGenerator& SetAgeLowerLimit(float Limit);
    FStellarGenerator& SetAgeUpperLimit(float Limit);
    FStellarGenerator& SetFeHLowerLimit(float Limit);
    FStellarGenerator& SetFeHUpperLimit(float Limit);
    FStellarGenerator& SetMassLowerLimit(float Limit);
    FStellarGenerator& SetMassUpperLimit(float Limit);
    FStellarGenerator& SetCoilTempLimit(float Limit);
    FStellarGenerator& SetdEpdM(float dEpdM);
    FStellarGenerator& SetAgePdf(const std::function<float(const glm::vec3&, float, float)>& AgePdf);
    FStellarGenerator& SetAgeMaxPdf(const glm::vec2& MaxPdf);
    FStellarGenerator& SetMassPdfs(const std::array<std::function<float(float)>, 2>& MassPdfs);
    FStellarGenerator& SetMassMaxPdfs(const std::array<glm::vec2, 2>& MaxPdfs);
    FStellarGenerator& SetAgeDistribution(EGenerateDistribution Distribution);
    FStellarGenerator& SetFeHDistribution(EGenerateDistribution Distribution);
    FStellarGenerator& SetMassDistribution(EGenerateDistribution Distribution);
    FStellarGenerator& SetGenerateOption(EGenerateOption Option);

private:
    template <typename CsvType>
    CsvType* LoadCsvAsset(const std::string& Filename, const std::vector<std::string>& Headers);

    void InitMistData();
    void InitPdfs();
    float GenerateAge(float MaxPdf);
    float GenerateMass(float MaxPdf, auto& LogMassPdf);
    std::vector<double> GetFullMistData(const FBasicProperties& Properties, bool bIsWhiteDwarf, bool bIsSingleWhiteDwarf);
    std::vector<double> InterpolateMistData(const std::pair<std::string, std::string>& Files, double TargetAge, double TargetMass, double MassCoefficient);
    std::vector<std::vector<double>> FindPhaseChanges(const FMistData* DataCsv);
    double CalculateEvolutionProgress(std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& PhaseChanges, double TargetAge, double MassCoefficient);
    std::pair<double, std::pair<double, double>> FindSurroundingTimePoints(const std::vector<std::vector<double>>& PhaseChanges, double TargetAge);
    std::pair<double, std::size_t> FindSurroundingTimePoints(const std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& PhaseChanges, double TargetAge, double MassCoefficient);
    void AlignArrays(std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& Arrays);
    std::vector<double> InterpolateHrDiagram(FHrDiagram* Data, double BvColorIndex);
    std::vector<double> InterpolateStarData(FMistData* Data, double EvolutionProgress);
    std::vector<double> InterpolateStarData(FWdMistData* Data, double TargetAge);
    std::vector<double> InterpolateStarData(auto* Data, double Target, const std::string& Header, int Index, bool bIsWhiteDwarf);
    std::vector<double> InterpolateArray(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Coefficient);
    std::vector<double> InterpolateFinalData(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Coefficient, bool bIsWhiteDwarf);
    void CalculateSpectralType(float FeH, Astro::AStar& StarData);
    Astro::FStellarClass::ELuminosityClass CalculateLuminosityClass(const Astro::AStar& StarData);
    void ProcessDeathStar(Astro::AStar& DeathStar, EGenerateOption Option = EGenerateOption::kNormal);
    void GenerateMagnetic(Astro::AStar& StarData);
    void GenerateSpin(Astro::AStar& StarData);
    void ExpandMistData(double TargetMass, std::vector<double>& StarData);

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
    std::mt19937                                          _RandomEngine;
    std::array<Util::TUniformRealDistribution<>, 8>       _MagneticGenerators;
    std::array<std::unique_ptr<Util::TDistribution<>>, 4> _FeHGenerators;
    std::array<Util::TUniformRealDistribution<>, 2>       _SpinGenerators;
    std::unique_ptr<Util::TDistribution<>>                _LogMassGenerator;
    Util::TUniformRealDistribution<>                      _AgeGenerator;
    Util::TUniformRealDistribution<>                      _CommonGenerator;

    std::array<std::function<float(float)>, 2>    _MassPdfs;
    std::array<glm::vec2, 2>                      _MassMaxPdfs;

    std::function<float(glm::vec3, float, float)> _AgePdf;
    glm::vec2                                     _AgeMaxPdf;

    float _UniverseAge;
    float _AgeLowerLimit;
    float _AgeUpperLimit;
    float _FeHLowerLimit;
    float _FeHUpperLimit;
    float _MassLowerLimit;
    float _MassUpperLimit;
    float _CoilTemperatureLimit;
    float _dEpdM;

    EGenerateDistribution _AgeDistribution;
    EGenerateDistribution _FeHDistribution;
    EGenerateDistribution _MassDistribution;
    EGenerateOption       _Option;

    static const std::vector<std::string> _kMistHeaders;
    static const std::vector<std::string> _kWdMistHeaders;
    static const std::vector<std::string> _kHrDiagramHeaders;
    static std::unordered_map<std::string, std::vector<float>> _kMassFilesCache;
    static std::unordered_map<const FMistData*, std::vector<std::vector<double>>> _kPhaseChangesCache;
    static std::shared_mutex _kCacheMutex;
    static bool _kbMistDataInitiated;
};

_GENERATOR_END
_SYSTEM_END
_NPGS_END

#include "StellarGenerator.inl"
