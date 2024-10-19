module;

#include <glm/glm.hpp>
#include "Engine/Core/Base.h"

export module Module.StellarGenerator;

import <array>;
import <functional>;
import <memory>;
import <random>;
import <shared_mutex>;
import <unordered_map>;
import <utility>;
import <vector>;

import Asset.Csv;
import Asset.Manager;
import Base.Astro.Star;
import Core.Random;
import Module.StellarClass;

_NPGS_BEGIN
_MODULES_BEGIN

export class StellarGenerator {
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
        kMergeStar,
        kBinaryFirstStar,
        kBinarySecondStar,
    };

    struct BasicProperties {
        float Age = 0.0f;
        float FeH = 0.0f;
        float InitialMassSol = 0.0f;
        bool  bIsSingleStar  = true;

        GenerateOption Option; // 用于保存生成选项，类的生成选项仅影响该属性。生成的恒星完整信息也将根据该属性决定。该选项用于防止多线程生成恒星时属性和生成器胡乱匹配

        explicit operator Astro::Star() const {
            Astro::Star Star;
            Star.SetAge(Age);
            Star.SetFeH(FeH);
            Star.SetInitialMass(InitialMassSol);
            Star.SetIsSingleStar(bIsSingleStar);

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
        float CoilTempLimit  =  1514.114f, float dEpdM          = 2e6f,
        const std::function<float(const glm::vec3&, float, float)>& AgePdf = nullptr,
        const glm::vec2& AgeMaxPdf = glm::vec2(),
        const std::array<std::function<float(float, std::function<float(float)>)>, 3>& MassPdfs = { nullptr, nullptr, nullptr },
        const std::array<glm::vec2, 3>& MassMaxPdfs = { glm::vec2(), glm::vec2(), glm::vec2() }
    );

    ~StellarGenerator() = default;

public:
    BasicProperties GenerateBasicProperties();
    BasicProperties GenerateBasicProperties(float Age, float FeH);
    Astro::Star GenerateStar();
    Astro::Star GenerateStar(BasicProperties&  Properties);
    Astro::Star GenerateStar(BasicProperties&& Properties);

    StellarGenerator& SetLogMassSuggestDistribution(UniformRealDistribution<>& Distribution);
    StellarGenerator& SetUniverseAge(float Age);
    StellarGenerator& SetAgeLowerLimit(float Limit);
    StellarGenerator& SetAgeUpperLimit(float Limit);
    StellarGenerator& SetFeHLowerLimit(float Limit);
    StellarGenerator& SetFeHUpperLimit(float Limit);
    StellarGenerator& SetMassLowerLimit(float Limit);
    StellarGenerator& SetMassUpperLimit(float Limit);
    StellarGenerator& SetCoilTempLimit(float Limit);
    StellarGenerator& SetdEpdM(float dEpdM);
    StellarGenerator& SetAgePdf(const std::function<float(const glm::vec3&, float, float)>& AgePdf);
    StellarGenerator& SetAgeMaxPdf(const glm::vec2& MaxPdf);
    StellarGenerator& SetMassPdfs(const std::array<std::function<float(float, std::function<float(float)>)>, 3>& MassPdfs);
    StellarGenerator& SetMassMaxPdfs(const std::array<glm::vec2, 3>& MaxPdfs);
    StellarGenerator& SetAgeDistribution(GenerateDistribution Distribution);
    StellarGenerator& SetFeHDistribution(GenerateDistribution Distribution);
    StellarGenerator& SetMassDistribution(GenerateDistribution Distribution);
    StellarGenerator& SetGenerateOption(GenerateOption Option);

private:
    void InitMistData();
    void InitPdfs();

    // Processor functions, as member functions to access class members
    // ----------------------------------------------------------------
    float GenerateAge(float MaxPdf);
    float GenerateMass(float MaxPdf, auto& LogMassPdf, GenerateOption Option);
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

    std::array<std::function<float(float, std::function<float(float)>)>, 3> _MassPdfs;
    std::array<glm::vec2, 3> _MassMaxPdfs;

    std::function<float(glm::vec3, float, float)> _AgePdf;
    glm::vec2 _AgeMaxPdf;

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

#include "StellarGenerator1.inl"
