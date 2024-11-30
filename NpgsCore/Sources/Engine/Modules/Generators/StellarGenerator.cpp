#include "StellarGenerator.h"

#include <cmath>
#include <cstdint>
#include <algorithm>
#include <charconv>
#include <filesystem>
#include <format>
#include <iomanip>
#include <iterator>
#include <limits>
#include <print>
#include <sstream>
#include <stdexcept>
#include <string>

#include <glm/glm.hpp>

#define ENABLE_CONSOLE_LOGGER
#include "Engine/AssetLoader/AssetManager.h"
#include "Engine/Core/Base.h"
#include "Engine/Core/Constants.h"
#include "Engine/Utilities/Logger.h"
#include <Engine/Utilities/Utilities.h>

_NPGS_BEGIN
_MODULE_BEGIN

// Tool macros
// -----------
#define GenerateDeathStarPlaceholder(Lifetime) {                                                                        \
    Util::StellarClass::SpectralType DeathStarClass{                                                                    \
        Util::StellarClass::SpectralClass::kSpectral_Unknown,                                                           \
        Util::StellarClass::SpectralClass::kSpectral_Unknown,                                                           \
        Util::StellarClass::LuminosityClass::kLuminosity_Unknown,                                                       \
        0, 0.0f, 0.0f, false                                                                                            \
    };                                                                                                                  \
                                                                                                                        \
    Astro::Star DeathStar;                                                                                              \
    DeathStar.SetStellarClass(Util::StellarClass(Util::StellarClass::StarType::kDeathStarPlaceholder, DeathStarClass)); \
    DeathStar.SetLifetime(Lifetime);                                                                                    \
    throw DeathStar;                                                                                                    \
}

// Tool functions
// --------------
static float DefaultAgePdf(const glm::vec3&, float Age, float UniverseAge);
static float DefaultLogMassPdfSingleStar(float LogMassSol);
static float DefaultLogMassPdfBinaryStar(float LogMassSol);

// StellarGenerator implementations
// --------------------------------
StellarGenerator::StellarGenerator(
    const std::seed_seq& SeedSequence, GenerateOption Option, float UniverseAge,
    float MassLowerLimit, float MassUpperLimit, GenerateDistribution MassDistribution,
    float AgeLowerLimit,  float AgeUpperLimit,  GenerateDistribution AgeDistribution,
    float FeHLowerLimit,  float FeHUpperLimit,  GenerateDistribution FeHDistribution,
    float CoilTemperatureLimit, float dEpdM,
    const std::function<float(const glm::vec3&, float, float)>& AgePdf,
    const glm::vec2& AgeMaxPdf,
    const std::array<std::function<float(float)>, 2>& MassPdfs,
    const std::array<glm::vec2, 2>& MassMaxPdfs
)   :
    _RandomEngine(SeedSequence),
    _MagneticGenerators{
        Util::UniformRealDistribution<>(std::log10(500.0f), std::log10(3000.0f)),
        Util::UniformRealDistribution<>( 1.0f, 3.0f),
        Util::UniformRealDistribution<>( 0.0f, 1.0f),
        Util::UniformRealDistribution<>( 3.0f, 4.0f),
        Util::UniformRealDistribution<>(-1.0f, 0.0f),
        Util::UniformRealDistribution<>( 2.0f, 3.0f),
        Util::UniformRealDistribution<>( 0.5f, 4.5f),
        Util::UniformRealDistribution<>( 1e9f, 1e11f)
    },

    _FeHGenerators{
        std::make_shared<Util::LogNormalDistribution<>>(-0.3f, 0.5f),
        std::make_shared<Util::NormalDistribution<>>(-0.3f, 0.15f),
        std::make_shared<Util::NormalDistribution<>>(-0.08f, 0.12f),
        std::make_shared<Util::NormalDistribution<>>(0.05f, 0.16f)
    },

    _SpinGenerators{
        Util::UniformRealDistribution<>(3.0f, 5.0f),
        Util::UniformRealDistribution<>(0.001f, 0.998f)
    },

    _LogMassGenerator(
        Option == StellarGenerator::GenerateOption::kMergeStar ?
        std::make_shared<Util::UniformRealDistribution<>>(0.0f, 1.0f) :
        std::make_shared<Util::UniformRealDistribution<>>(std::log10(MassLowerLimit), std::log10(MassUpperLimit))
    ),
    _AgeGenerator(AgeLowerLimit, AgeUpperLimit),
    _CommonGenerator(0.0f, 1.0f),

    _MassPdfs(MassPdfs),
    _MassMaxPdfs(MassMaxPdfs),
    _AgeMaxPdf(AgeMaxPdf),
    _AgePdf(AgePdf),
    _UniverseAge(UniverseAge),
    _AgeLowerLimit(AgeLowerLimit), _AgeUpperLimit(AgeUpperLimit),
    _FeHLowerLimit(FeHLowerLimit), _FeHUpperLimit(FeHUpperLimit),
    _MassLowerLimit(MassLowerLimit), _MassUpperLimit(MassUpperLimit),
    _CoilTemperatureLimit(CoilTemperatureLimit), _dEpdM(dEpdM),

    _AgeDistribution(AgeDistribution), _FeHDistribution(FeHDistribution), _MassDistribution(MassDistribution), _Option(Option)
{
    InitMistData();
    InitPdfs();
}

StellarGenerator::BasicProperties StellarGenerator::GenerateBasicProperties() {
    return GenerateBasicProperties(0.0f, 0.0f);
}

StellarGenerator::BasicProperties StellarGenerator::GenerateBasicProperties(float Age, float FeH) {
    BasicProperties Properties{};
    Properties.TypeOption = _Option;

    if (Properties.TypeOption == GenerateOption::kBinarySecondStar) {
        Properties.TypeOption =  GenerateOption::kNormal;
    }

    // 生成 3 个基本参数
    if (Age == 0.0f) {
        switch (_AgeDistribution) {
        case GenerateDistribution::kFromPdf: {
            glm::vec2 MaxPdf = _AgeMaxPdf;
            if (!(_AgeLowerLimit < _UniverseAge - 1.38e10f + _AgeMaxPdf.x &&
                  _AgeUpperLimit > _UniverseAge - 1.38e10f + _AgeMaxPdf.x)) {
                if (_AgeLowerLimit > _UniverseAge - 1.38e10f + _AgeMaxPdf.x) {
                    MaxPdf.y = _AgePdf(glm::vec3(), _AgeLowerLimit, _UniverseAge / 1e9f);
                } else if (_AgeUpperLimit < _UniverseAge - 1.38e10f + _AgeMaxPdf.x) {
                    MaxPdf.y = _AgePdf(glm::vec3(), _AgeUpperLimit, _UniverseAge / 1e9f);
                }
            }
            Properties.Age = GenerateAge(MaxPdf.y);
            break;
        }
        case GenerateDistribution::kUniform: {
            Properties.Age = _AgeLowerLimit + _CommonGenerator(_RandomEngine) * (_AgeUpperLimit - _AgeLowerLimit);
            break;
        }
        case GenerateDistribution::kUniformByExponent: {
            float Random = _CommonGenerator(_RandomEngine);
            float LogAgeLower = std::log10(_AgeLowerLimit);
            float LogAgeUpper = std::log10(_AgeUpperLimit);
            Properties.Age = std::pow(10.0f, LogAgeLower + Random * (LogAgeUpper - LogAgeLower));
            break;
        }
        default:
            break;
        }
    } else {
        Properties.Age = Age;
    }

    if (FeH == 0.0f) {
        Util::Distribution<>* FeHGenerator = nullptr;

        float FeHLowerLimit = _FeHLowerLimit;
        float FeHUpperLimit = _FeHUpperLimit;

        // 不同的年龄使用不同的分布
        if (Properties.Age > _UniverseAge - 1.38e10f + 8e9f) {
            FeHGenerator = _FeHGenerators[0].get();
            FeHLowerLimit = -_FeHUpperLimit; // 对数分布，但是是反的
            FeHUpperLimit = -_FeHLowerLimit;
        } else if (Properties.Age > _UniverseAge - 1.38e10f + 6e9f) {
            FeHGenerator = _FeHGenerators[1].get();
        } else if (Properties.Age > _UniverseAge - 1.38e10f + 4e9f) {
            FeHGenerator = _FeHGenerators[2].get();
        } else {
            FeHGenerator = _FeHGenerators[3].get();
        }

        do {
            FeH = (*FeHGenerator)(_RandomEngine);
        } while (FeH > FeHUpperLimit || FeH < FeHLowerLimit);

        if (Properties.Age > _UniverseAge - 1.38e10 + 8e9) {
            FeH *= -1.0f; // 把对数分布反过来
        }
    }

    Properties.FeH = FeH;

    if (_Option != GenerateOption::kBinarySecondStar) {
        Util::BernoulliDistribution<> BinaryProbability(0.45 - 0.07 * std::pow(10, FeH));
        if (BinaryProbability(_RandomEngine)) {
            Properties.MultiOption = GenerateOption::kBinaryFirstStar;
            Properties.bIsSingleStar = false;
        }
    } else {
        Properties.MultiOption = GenerateOption::kBinarySecondStar;
        Properties.bIsSingleStar = false;
    }

    if (_MassLowerLimit == 0.0f && _MassUpperLimit == 0.0f) {
        Properties.InitialMassSol = 0.0f;
    } else {
        switch (_MassDistribution) {
        case GenerateDistribution::kFromPdf: {
            glm::vec2 MaxPdf{};
            float LogMassLower = std::log10(_MassLowerLimit);
            float LogMassUpper = std::log10(_MassUpperLimit);
            std::function<float(float)> LogMassPdf = nullptr;

            if (Properties.MultiOption != GenerateOption::kBinarySecondStar) {
                if (Properties.MultiOption == GenerateOption::kBinaryFirstStar) {
                    LogMassPdf = _MassPdfs[1];
                    MaxPdf = _MassMaxPdfs[1];
                } else {
                    LogMassPdf = _MassPdfs[0];
                    MaxPdf = _MassMaxPdfs[0];
                }
            } else {
                LogMassPdf = _MassPdfs[1];
                MaxPdf = _MassMaxPdfs[1];
            }

            if (!(LogMassLower < MaxPdf.x && LogMassUpper > MaxPdf.x)) { // 调整最大值，防止接受率过低
                if (LogMassLower > MaxPdf.x) {
                    MaxPdf.y = LogMassPdf(LogMassLower);
                } else if (LogMassUpper < MaxPdf.x) {
                    MaxPdf.y = LogMassPdf(LogMassUpper);
                }
            }

            Properties.InitialMassSol = GenerateMass(MaxPdf.y, LogMassPdf);
            break;
        }
        case GenerateDistribution::kUniform: {
            Properties.InitialMassSol = _MassLowerLimit + _CommonGenerator(_RandomEngine) * (_MassUpperLimit - _MassLowerLimit);
            break;
        }
        default:
            break;
        }
    }

    return Properties;
}

Astro::Star StellarGenerator::GenerateStar() {
    return GenerateStar(std::move(GenerateBasicProperties()));
}

Astro::Star StellarGenerator::GenerateStar(BasicProperties& Properties) {
    return GenerateStar(std::move(Properties));
}

Astro::Star StellarGenerator::GenerateStar(BasicProperties&& Properties) {
    if (Util::Equal(Properties.InitialMassSol, -1.0f)) {
        Properties = GenerateBasicProperties(Properties.Age, Properties.FeH);
    }
    
    Astro::Star Star(Properties);
    std::vector<double> StarData;

    switch (Properties.TypeOption) {
    case GenerateOption::kNormal: {
        try {
            StarData = GetActuallyMistData(Properties, false, true);
        } catch (Astro::Star& DeathStar) {
            DeathStar.SetAge(Properties.Age);
            DeathStar.SetFeH(Properties.FeH);
            DeathStar.SetInitialMass(Properties.InitialMassSol);
            DeathStar.SetIsSingleStar(Properties.bIsSingleStar);
            ProcessDeathStar(DeathStar);
            if (DeathStar.GetEvolutionPhase() == Astro::Star::EvolutionPhase::kNull) {
                // 对于双星，二者年龄和金属丰度要保持一致
                Properties.InitialMassSol /= 2;
                DeathStar = GenerateStar(Properties);
            }

            return DeathStar;
        }

        break;
    }
    case GenerateOption::kGiant: {
        Properties.Age = -1.0f; // 使用 -1.0，在计算年龄的时候根据寿命赋值一个濒死年龄
        StarData = GetActuallyMistData(Properties, false, true);
        break;
    }
    case GenerateOption::kDeathStar: {
        ProcessDeathStar(Star, GenerateOption::kDeathStar);
        Properties.Age = static_cast<float>(Star.GetAge());
        Properties.FeH = Star.GetFeH();
        Properties.InitialMassSol = Star.GetInitialMass() / kSolarMass;

        if (Star.GetEvolutionPhase() == Astro::Star::EvolutionPhase::kNull) { // 炸没了，再生成个新的
            Properties.InitialMassSol /= 2;
            Star = GenerateStar(Properties);
        }

        return Star;

        break;
    }
    case GenerateOption::kMergeStar: {
        ProcessDeathStar(Star, GenerateOption::kMergeStar);
        return Star;

        break;
    }
    default:
        break;
    }

    if (StarData.empty()) {
        return {};
    }

    double Lifetime          = StarData[_kLifetimeIndex];
    double EvolutionProgress = StarData[_kXIndex];
    float  Age               = static_cast<float>(StarData[_kStarAgeIndex]);
    float  RadiusSol         = static_cast<float>(std::pow(10.0, StarData[_kLogRIndex]));
    float  MassSol           = static_cast<float>(StarData[_kStarMassIndex]);
    float  Teff              = static_cast<float>(std::pow(10.0, StarData[_kLogTeffIndex]));
    float  SurfaceZ          = static_cast<float>(std::pow(10.0, StarData[_kLogSurfZIndex]));
    float  SurfaceH1         = static_cast<float>(StarData[_kSurfaceH1Index]);
    float  SurfaceHe3        = static_cast<float>(StarData[_kSurfaceHe3Index]);
    float  CoreTemp          = static_cast<float>(std::pow(10.0, StarData[_kLogCenterTIndex]));
    float  CoreDensity       = static_cast<float>(std::pow(10.0, StarData[_kLogCenterRhoIndex]));
    float  MassLossRate      = static_cast<float>(StarData[_kStarMdotIndex]);

    float LuminositySol  = std::pow(RadiusSol, 2.0f) * std::pow((Teff / kSolarTeff), 4.0f);
    float EscapeVelocity = std::sqrt((2.0f * kGravityConstant * MassSol * kSolarMass) / (RadiusSol * kSolarRadius));

    float LifeProgress         = static_cast<float>(Age / Lifetime);
    float WindSpeedCoefficient = 3.0f - LifeProgress;
    float StellarWindSpeed     = WindSpeedCoefficient * EscapeVelocity;

    float SurfaceEnergeticNuclide = (SurfaceH1 * 0.00002f + SurfaceHe3);
    float SurfaceVolatiles = 1.0f - SurfaceZ - SurfaceEnergeticNuclide;

    float Theta = _CommonGenerator(_RandomEngine) * 2.0f * kPi;
    float Phi   = _CommonGenerator(_RandomEngine) * kPi;

    Astro::Star::EvolutionPhase EvolutionPhase = static_cast<Astro::Star::EvolutionPhase>(StarData[_kPhaseIndex]);

    Star.SetInitialMass(Star.GetInitialMass() * kSolarMass);
    Star.SetIsSingleStar(Properties.bIsSingleStar);
    Star.SetAge(Age);
    Star.SetMass(MassSol * kSolarMass);
    Star.SetLifetime(Lifetime);
    Star.SetRadius(RadiusSol * kSolarRadius);
    Star.SetEscapeVelocity(EscapeVelocity);
    Star.SetLuminosity(LuminositySol * kSolarLuminosity);
    Star.SetTeff(Teff);
    Star.SetSurfaceH1(SurfaceH1);
    Star.SetSurfaceZ(SurfaceZ);
    Star.SetSurfaceEnergeticNuclide(SurfaceEnergeticNuclide);
    Star.SetSurfaceVolatiles(SurfaceVolatiles);
    Star.SetCoreTemp(CoreTemp);
    Star.SetCoreDensity(CoreDensity * 1000);
    Star.SetStellarWindSpeed(StellarWindSpeed);
    Star.SetStellarWindMassLossRate(-(MassLossRate * kSolarMass / kYearToSeconds));
    Star.SetEvolutionProgress(EvolutionProgress);
    Star.SetEvolutionPhase(EvolutionPhase);
    Star.SetNormal(glm::vec2(Theta, Phi));

    CalculateSpectralType(static_cast<float>(StarData.back()), Star);
    GenerateMagnetic(Star);
    GenerateSpin(Star);

    double Mass          = Star.GetMass();
    double Luminosity    = Star.GetLuminosity();
    float  Radius        = Star.GetRadius();
    float  MagneticField = Star.GetMagneticField();

    float MinCoilMass = static_cast<float>(std::max(
        6.6156e14  * std::pow(MagneticField, 2.0f) * std::pow(Luminosity, 1.5) * std::pow(_CoilTemperatureLimit, -6.0f) * std::pow(_dEpdM, -1.0f),
        2.34865e29 * std::pow(MagneticField, 2.0f) * std::pow(Luminosity, 2.0) * std::pow(_CoilTemperatureLimit, -8.0f) * std::pow(Mass,   -1.0)
    ));

    Star.SetMinCoilMass(MinCoilMass);

    return Star;
}

void StellarGenerator::InitMistData() {
    if (_kbMistDataInitiated) {
        return;
    }

    const std::array<std::string, 10> kPresetPrefix{
        Asset::GetAssetFilepath(Asset::AssetType::kModel, "MIST/[Fe_H]=-4.0"),
        Asset::GetAssetFilepath(Asset::AssetType::kModel, "MIST/[Fe_H]=-3.0"),
        Asset::GetAssetFilepath(Asset::AssetType::kModel, "MIST/[Fe_H]=-2.0"),
        Asset::GetAssetFilepath(Asset::AssetType::kModel, "MIST/[Fe_H]=-1.5"),
        Asset::GetAssetFilepath(Asset::AssetType::kModel, "MIST/[Fe_H]=-1.0"),
        Asset::GetAssetFilepath(Asset::AssetType::kModel, "MIST/[Fe_H]=-0.5"),
        Asset::GetAssetFilepath(Asset::AssetType::kModel, "MIST/[Fe_H]=+0.0"),
        Asset::GetAssetFilepath(Asset::AssetType::kModel, "MIST/[Fe_H]=+0.5"),
        Asset::GetAssetFilepath(Asset::AssetType::kModel, "MIST/WhiteDwarfs/Thin"),
        Asset::GetAssetFilepath(Asset::AssetType::kModel, "MIST/WhiteDwarfs/Thick")
    };

    std::vector<float> Masses;

    for (const auto& PrefixDirectory : kPresetPrefix) {
        for (const auto& Entry : std::filesystem::directory_iterator(PrefixDirectory)) {
            std::string Filename = Entry.path().filename().string();

            float Mass = 0.0f;
            std::from_chars(Filename.data(), Filename.data() + Filename.find("Ms_track.csv"), Mass);
            
            Masses.emplace_back(Mass);

            if (PrefixDirectory.find("WhiteDwarfs") != std::string::npos) {
                LoadCsvAsset<WdMistData>(PrefixDirectory + "/" + Filename, _kWdMistHeaders);
            } else {
                LoadCsvAsset<MistData>(PrefixDirectory + "/" + Filename, _kMistHeaders);
            }
        }

        _kMassFileCache.emplace(PrefixDirectory, Masses);
        Masses.clear();
    }

    _kbMistDataInitiated = true;
}

void StellarGenerator::InitPdfs() {
    if (_AgePdf == nullptr) {
        _AgePdf = DefaultAgePdf;
        _AgeMaxPdf = glm::vec2(8e9, 2.7f);
    }

    if (_MassPdfs[0] == nullptr) {
        _MassPdfs[0] = DefaultLogMassPdfSingleStar;
        _MassMaxPdfs[0] = glm::vec2(std::log10(0.1f), 0.158f);
    }

    if (_MassPdfs[1] == nullptr) {
        _MassPdfs[1] = DefaultLogMassPdfBinaryStar;
        _MassMaxPdfs[1] = glm::vec2(std::log10(0.22f), 0.086);
    }
}

float StellarGenerator::GenerateAge(float MaxPdf) {
    float Age = 0.0f;
    float Probability = 0.0f;
    do {
        Age = _AgeGenerator(_RandomEngine);
        Probability = DefaultAgePdf(glm::vec3(), Age / 1e9f, _UniverseAge / 1e9f);
    } while (_CommonGenerator(_RandomEngine) * MaxPdf > Probability);

    return Age;
}

float StellarGenerator::GenerateMass(float MaxPdf, auto& LogMassPdf) {
    float LogMass = 0.0f;
    float Probability = 0.0f;

    float LogMassLower = std::log10(_MassLowerLimit);
    float LogMassUpper = std::log10(_MassUpperLimit);

    if (LogMassUpper >= std::log10(300.0f)) {
        LogMassUpper =  std::log10(299.9f);
    }

    do {
        LogMass = (*_LogMassGenerator)(_RandomEngine);
        Probability = LogMassPdf(LogMass);
    } while ((LogMass < LogMassLower || LogMass > LogMassUpper) || _CommonGenerator(_RandomEngine) * MaxPdf > Probability);

    return std::pow(10.0f, LogMass);
}

std::vector<double> StellarGenerator::GetActuallyMistData(const BasicProperties& Properties, bool bIsWhiteDwarf, bool bIsSingleWhiteDwarf) {
    float TargetAge  = Properties.Age;
    float TargetFeH  = Properties.FeH;
    float TargetMass = Properties.InitialMassSol;

    std::string PrefixDirectory;
    std::string MassString;
    std::stringstream MassStream;
    std::pair<std::string, std::string> Files;

    if (!bIsWhiteDwarf) {
        const std::array<float, 8> kPresetFeH{ -4.0f, -3.0f, -2.0f, -1.5f, -1.0f, -0.5f, 0.0f, 0.5f };

        float ClosestFeH = *std::min_element(kPresetFeH.begin(), kPresetFeH.end(), [TargetFeH](float Lhs, float Rhs) -> bool {
            return std::abs(Lhs - TargetFeH) < std::abs(Rhs - TargetFeH);
        });

        TargetFeH = ClosestFeH;

        MassStream << std::fixed << std::setfill('0') << std::setw(6) << std::setprecision(2) << TargetMass;
        MassString = MassStream.str() + "0";

        std::stringstream FeHStream;
        FeHStream << std::fixed << std::setprecision(1) << TargetFeH;
        PrefixDirectory = FeHStream.str();
        if (TargetFeH >= 0.0f) {
            PrefixDirectory.insert(PrefixDirectory.begin(), '+');
        }
        PrefixDirectory.insert(0, Asset::GetAssetFilepath(Asset::AssetType::kModel, "MIST/[Fe_H]="));
    } else {
        if (bIsSingleWhiteDwarf) {
            PrefixDirectory = Asset::GetAssetFilepath(Asset::AssetType::kModel, "MIST/WhiteDwarfs/Thin");
        } else {
            PrefixDirectory = Asset::GetAssetFilepath(Asset::AssetType::kModel, "MIST/WhiteDwarfs/Thick");
        }
    }

    std::vector<float> Masses;
    {
        std::shared_lock Lock(_kCacheMutex);
        Masses = _kMassFileCache[PrefixDirectory];
    }

    auto it = std::lower_bound(Masses.begin(), Masses.end(), TargetMass);
    if (it == Masses.end()) {
        if (!bIsWhiteDwarf) {
            throw std::out_of_range("Mass value out of range.");
        } else {
            it = std::prev(Masses.end(), 1);
        }
    }

    float LowerMass = 0.0f;
    float UpperMass = 0.0f;

    if (*it == TargetMass) {
        LowerMass = UpperMass = *it;
    } else {
        LowerMass = it == Masses.begin() ? *it : *(it - 1);
        UpperMass = *it;
    }

    float MassCoefficient = (TargetMass - LowerMass) / (UpperMass - LowerMass);

    MassString.clear();
    MassStream.str("");
    MassStream.clear();

    MassStream << std::fixed << std::setfill('0') << std::setw(6) << std::setprecision(2) << LowerMass;
    MassString = MassStream.str() + "0";
    std::string LowerMassFile = PrefixDirectory + "/" + MassString + "Ms_track.csv";

    MassString.clear();
    MassStream.str("");
    MassStream.clear();

    MassStream << std::fixed << std::setfill('0') << std::setw(6) << std::setprecision(2) << UpperMass;
    MassString = MassStream.str() + "0";
    std::string UpperMassFile = PrefixDirectory + "/" + MassString + "Ms_track.csv";

    Files.first  = LowerMassFile;
    Files.second = UpperMassFile;

    std::vector<double> Result = InterpolateMistData(Files, TargetAge, TargetMass, MassCoefficient);
    Result.emplace_back(TargetFeH); // 加入插值使用的金属丰度，用于计算光谱类型

    return Result;
}

std::vector<double> StellarGenerator::InterpolateMistData(const std::pair<std::string, std::string>& Files, double TargetAge, double TargetMass, double MassCoefficient) {
    std::vector<double> Result;

    if (Files.first.find("WhiteDwarfs") == std::string::npos) {
        if (Files.first != Files.second) [[likely]] {
            std::shared_ptr<MistData> LowerData = LoadCsvAsset<MistData>(Files.first,  _kMistHeaders);
            std::shared_ptr<MistData> UpperData = LoadCsvAsset<MistData>(Files.second, _kMistHeaders);

            auto LowerPhaseChanges = FindPhaseChanges(LowerData);
            auto UpperPhaseChanges = FindPhaseChanges(UpperData);

            if (Util::Equal(TargetAge, -1.0)) { // 年龄为 -1.0 代表要生成濒死恒星
                double LowerLifetime = LowerPhaseChanges.back()[_kStarAgeIndex];
                double UpperLifetime = UpperPhaseChanges.back()[_kStarAgeIndex];
                double Lifetime = LowerLifetime + (UpperLifetime - LowerLifetime) * MassCoefficient;
                TargetAge = Lifetime - 500000;
            }

            std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>> PhaseChangePair{
                LowerPhaseChanges,
                UpperPhaseChanges
            };

            double EvolutionProgress = CalculateEvolutionProgress(PhaseChangePair, TargetAge, MassCoefficient);

            double LowerLifetime = PhaseChangePair.first.back()[_kStarAgeIndex];
            double UpperLifetime = PhaseChangePair.second.back()[_kStarAgeIndex];

            std::vector<double> LowerRows = InterpolateStarData(LowerData, EvolutionProgress);
            std::vector<double> UpperRows = InterpolateStarData(UpperData, EvolutionProgress);

            LowerRows.emplace_back(LowerLifetime);
            UpperRows.emplace_back(UpperLifetime);

            Result = InterpolateFinalData({ LowerRows, UpperRows }, MassCoefficient, false);
        } else [[unlikely]] {
            std::shared_ptr<MistData> StarData = LoadCsvAsset<MistData>(Files.first, _kMistHeaders);
            auto PhaseChanges = FindPhaseChanges(StarData);

            if (Util::Equal(TargetAge, -1.0)) { // 年龄为 -1.0 代表要生成濒死恒星
                double Lifetime = PhaseChanges.back()[_kStarAgeIndex];
                TargetAge = Lifetime - 500000;
            }

            double EvolutionProgress = 0.0;
            double Lifetime = 0.0;
            if (TargetMass >= 0.1) {
                std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>> PhaseChangePair{ PhaseChanges, {} };
                EvolutionProgress = CalculateEvolutionProgress(PhaseChangePair, TargetAge, MassCoefficient);
                Lifetime = PhaseChanges.back()[_kStarAgeIndex];
                Result = InterpolateStarData(StarData, EvolutionProgress);
                Result.emplace_back(Lifetime);
            } else { // 外推小质量恒星的数据
                double OriginalLowerPhaseChangePoint = PhaseChanges[1][_kStarAgeIndex];
                double OriginalUpperPhaseChangePoint = PhaseChanges[2][_kStarAgeIndex];
                double LowerPhaseChangePoint = OriginalLowerPhaseChangePoint * std::pow(TargetMass / 0.1, -1.3);
                double UpperPhaseChangePoint = OriginalUpperPhaseChangePoint * std::pow(TargetMass / 0.1, -1.3);
                Lifetime = UpperPhaseChangePoint;
                if (TargetAge < LowerPhaseChangePoint) {
                    EvolutionProgress = TargetAge / LowerPhaseChangePoint - 1;
                } else if (LowerPhaseChangePoint <= TargetAge && TargetAge <= UpperPhaseChangePoint) {
                    EvolutionProgress = (TargetAge - LowerPhaseChangePoint) / (UpperPhaseChangePoint - LowerPhaseChangePoint);
                } else if (TargetAge > UpperPhaseChangePoint) {
                    GenerateDeathStarPlaceholder(Lifetime);
                }

                Result = InterpolateStarData(StarData, EvolutionProgress);
                Result.emplace_back(Lifetime);
                ExpandMistData(TargetMass, Result);
            }
        }
    } else {
        if (Files.first != Files.second) [[likely]] {
            std::shared_ptr<WdMistData> LowerData = LoadCsvAsset<WdMistData>(Files.first,  _kWdMistHeaders);
            std::shared_ptr<WdMistData> UpperData = LoadCsvAsset<WdMistData>(Files.second, _kWdMistHeaders);

            std::vector<double> LowerRows = InterpolateStarData(LowerData, TargetAge);
            std::vector<double> UpperRows = InterpolateStarData(UpperData, TargetAge);

            Result = InterpolateFinalData({ LowerRows, UpperRows }, MassCoefficient, true);
        } else [[unlikely]] {
            std::shared_ptr<WdMistData> StarData = LoadCsvAsset<WdMistData>(Files.first, _kWdMistHeaders);
            Result = InterpolateStarData(StarData, TargetAge);
        }
    }

    return Result;
}

std::vector<std::vector<double>> StellarGenerator::FindPhaseChanges(const std::shared_ptr<MistData>& DataCsv) {
    std::vector<std::vector<double>> Result;

    {
        std::shared_lock Lock(_kCacheMutex);
        if (_kPhaseChangesCache.contains(DataCsv)) {
            return _kPhaseChangesCache[DataCsv];
        }
    }

    const auto* const DataArray = DataCsv->Data();
    int CurrentPhase = -2;
    for (const auto& Row : *DataArray) {
        if (Row[_kPhaseIndex] != CurrentPhase || Row[_kXIndex] == 10.0) {
            CurrentPhase = static_cast<int>(Row[_kPhaseIndex]);
            Result.emplace_back(Row);
        }
    }

    {
        std::unique_lock Lock(_kCacheMutex);
        if (!_kPhaseChangesCache.contains(DataCsv)) {
            _kPhaseChangesCache.emplace(DataCsv, Result);
        } else {
            Result = _kPhaseChangesCache[DataCsv];
        }
    }

    return Result;
}

double StellarGenerator::CalculateEvolutionProgress(std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& PhaseChanges, double TargetAge, double MassCoefficient) {
    double Result = 0.0;
    double Phase  = 0.0;

    if (PhaseChanges.second.empty()) [[unlikely]] {
        const auto& TimePointResults = FindSurroundingTimePoints(PhaseChanges.first, TargetAge);
        Phase = TimePointResults.first;
        const auto& TimePoints = TimePointResults.second;
        if (TargetAge > TimePoints.second) {
            GenerateDeathStarPlaceholder(TimePoints.second);
        }

        Result = (TargetAge - TimePoints.first) / (TimePoints.second - TimePoints.first) + Phase;
    } else [[likely]] {
        if (PhaseChanges.first.size() == PhaseChanges.second.size() &&
           (*std::prev(PhaseChanges.first.end(), 2))[_kPhaseIndex] == (*std::prev(PhaseChanges.second.end(), 2))[_kPhaseIndex]) {
            const auto& TimePointResults = FindSurroundingTimePoints(PhaseChanges, TargetAge, MassCoefficient);

            Phase = TimePointResults.first;
            std::size_t Index = TimePointResults.second;

            if (Index + 1 != PhaseChanges.first.size()) {
                std::pair<double, double> LowerTimePoints = {
                    PhaseChanges.first[Index][_kStarAgeIndex],
                    PhaseChanges.first[Index + 1][_kStarAgeIndex]
                };

                std::pair<double, double> UpperTimePoints = {
                    PhaseChanges.second[Index][_kStarAgeIndex],
                    PhaseChanges.second[Index + 1][_kStarAgeIndex]
                };

                const auto& [LowerLowerTimePoint, LowerUpperTimePoint] = LowerTimePoints;
                const auto& [UpperLowerTimePoint, UpperUpperTimePoint] = UpperTimePoints;

                double LowerTimePoint = LowerLowerTimePoint + (UpperLowerTimePoint - LowerLowerTimePoint) * MassCoefficient;
                double UpperTimePoint = LowerUpperTimePoint + (UpperUpperTimePoint - LowerUpperTimePoint) * MassCoefficient;

                Result = (TargetAge - LowerTimePoint) / (UpperTimePoint - LowerTimePoint) + Phase;

                if (Result > PhaseChanges.first.back()[_kPhaseIndex] + 1) {
                    return 0.0;
                }
            } else {
                Result = 0.0;
            }
        } else {
            if (PhaseChanges.first.back()[_kPhaseIndex] == PhaseChanges.second.back()[_kPhaseIndex]) {
                double FirstDiscardTimePoint = 0.0;
                double FirstCommonTimePoint  = (*std::prev(PhaseChanges.first.end(), 2))[_kStarAgeIndex];

                std::size_t MinSize = std::min(PhaseChanges.first.size(), PhaseChanges.second.size());
                for (std::size_t i = 0; i != MinSize - 1; ++i) {
                    if (PhaseChanges.first[i][_kPhaseIndex] != PhaseChanges.second[i][_kPhaseIndex]) {
                        FirstDiscardTimePoint = PhaseChanges.first[i][_kStarAgeIndex];
                        break;
                    }
                }

                double DeltaTimePoint = FirstCommonTimePoint - FirstDiscardTimePoint;
                (*std::prev(PhaseChanges.first.end(), 2))[_kStarAgeIndex] -= DeltaTimePoint;
                PhaseChanges.first.back()[_kStarAgeIndex] -= DeltaTimePoint;
            }

            AlignArrays(PhaseChanges);

            Result = CalculateEvolutionProgress(PhaseChanges, TargetAge, MassCoefficient);
            double IntegerPart    = 0.0;
            double FractionalPart = std::modf(Result, &IntegerPart);
            if (PhaseChanges.second.back()[_kPhaseIndex] == 9 && FractionalPart > 0.99 && Result < 9.0 &&
                IntegerPart >= (*std::prev(PhaseChanges.first.end(), 3))[_kPhaseIndex]) {
                Result = 9.0;
            }
        }
    }

    return Result;
}

std::pair<double, std::pair<double, double>> StellarGenerator::FindSurroundingTimePoints(const std::vector<std::vector<double>>& PhaseChanges, double TargetAge) {
    std::vector<std::vector<double>>::const_iterator LowerTimePoint;
    std::vector<std::vector<double>>::const_iterator UpperTimePoint;

    if (PhaseChanges.size() != 2 || PhaseChanges.front()[_kPhaseIndex] != PhaseChanges.back()[_kPhaseIndex]) {
        LowerTimePoint = std::lower_bound(PhaseChanges.begin(), PhaseChanges.end(), TargetAge,
            [](const std::vector<double>& Lhs, double Rhs) -> bool {
                return Lhs[0] < Rhs;
            }
        );

        UpperTimePoint = std::upper_bound(PhaseChanges.begin(), PhaseChanges.end(), TargetAge,
            [](double Lhs, const std::vector<double>& Rhs) -> bool {
                return Lhs < Rhs[0];
            }
        );

        if (LowerTimePoint == UpperTimePoint) {
            if (LowerTimePoint != PhaseChanges.begin()) {
                --LowerTimePoint;
            }
        }

        if (UpperTimePoint == PhaseChanges.end()) {
            --LowerTimePoint;
            --UpperTimePoint;
        }
    } else {
        LowerTimePoint = PhaseChanges.begin();
        UpperTimePoint = std::prev(PhaseChanges.end(), 1);
    }

    return { (*LowerTimePoint)[_kXIndex], { (*LowerTimePoint)[_kStarAgeIndex], (*UpperTimePoint)[_kStarAgeIndex] } };
}

std::pair<double, std::size_t> StellarGenerator::FindSurroundingTimePoints(const std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& PhaseChanges, double TargetAge, double MassCoefficient) {
    std::vector<double> LowerPhaseChangeTimePoints;
    std::vector<double> UpperPhaseChangeTimePoints;
    for (std::size_t i = 0; i != PhaseChanges.first.size(); ++i) {
        LowerPhaseChangeTimePoints.emplace_back(PhaseChanges.first[i][_kStarAgeIndex]);
        UpperPhaseChangeTimePoints.emplace_back(PhaseChanges.second[i][_kStarAgeIndex]);
    }

    std::vector<double> PhaseChangeTimePoints =
        InterpolateArray({ LowerPhaseChangeTimePoints, UpperPhaseChangeTimePoints }, MassCoefficient);

    if (TargetAge > PhaseChangeTimePoints.back()) {
        double Lifetime = LowerPhaseChangeTimePoints.back() +
            (UpperPhaseChangeTimePoints.back() - LowerPhaseChangeTimePoints.back()) * MassCoefficient;
        GenerateDeathStarPlaceholder(Lifetime);
    }

    std::vector<std::pair<double, double>> TimePointPairs;
    for (std::size_t i = 0; i != PhaseChanges.first.size(); ++i) {
        TimePointPairs.emplace_back(PhaseChanges.first[i][_kPhaseIndex], PhaseChangeTimePoints[i]);
    }

    std::pair<double, std::size_t> Result;
    for (std::size_t i = 0; i != TimePointPairs.size(); ++i) {
        if (TimePointPairs[i].second >= TargetAge) {
            Result.first = TimePointPairs[i == 0 ? 0 : i - 1].first;
            Result.second = i == 0 ? 0 : i - 1;
            break;
        }
    }

    return Result;
}

void StellarGenerator::AlignArrays(std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& Arrays) {
    if (Arrays.first.back()[_kPhaseIndex] != 9 && Arrays.second.back()[_kPhaseIndex] != 9) {
        std::size_t MinSize = std::min(Arrays.first.size(), Arrays.second.size());
        Arrays.first.resize(MinSize);
        Arrays.second.resize(MinSize);
    } else if (Arrays.first.back()[_kPhaseIndex] != 9 && Arrays.second.back()[_kPhaseIndex] == 9) {
        if (Arrays.first.size() + 1 == Arrays.second.size()) {
            Arrays.second.pop_back();
            Arrays.second.back()[_kPhaseIndex] = Arrays.first.back()[_kPhaseIndex];
            Arrays.second.back()[_kXIndex] = Arrays.first.back()[_kXIndex];
        } else {
            std::size_t MinSize = std::min(Arrays.first.size(), Arrays.second.size());
            Arrays.first.resize(MinSize - 1);
            Arrays.second.resize(MinSize - 1);
            Arrays.second.back()[_kPhaseIndex] = Arrays.first.back()[_kPhaseIndex];
            Arrays.second.back()[_kXIndex] = Arrays.first.back()[_kXIndex];
        }
    } else if (Arrays.first.back()[_kPhaseIndex] == 9 && Arrays.second.back()[_kPhaseIndex] == 9) {
        std::vector<double> LastArray1 = Arrays.first.back();
        std::vector<double> LastArray2 = Arrays.second.back();
        std::vector<double> SubLastArray1 = *std::prev(Arrays.first.end(), 2);
        std::vector<double> SubLastArray2 = *std::prev(Arrays.second.end(), 2);

        std::size_t MinSize = std::min(Arrays.first.size(), Arrays.second.size());

        Arrays.first.resize(MinSize - 2);
        Arrays.second.resize(MinSize - 2);
        Arrays.first.emplace_back(SubLastArray1);
        Arrays.first.emplace_back(LastArray1);
        Arrays.second.emplace_back(SubLastArray2);
        Arrays.second.emplace_back(LastArray2);
    } else {
        std::vector<double> LastArray1 = Arrays.first.back();
        std::vector<double> LastArray2 = Arrays.second.back();
        std::size_t MinSize = std::min(Arrays.first.size(), Arrays.second.size());
        Arrays.first.resize(MinSize - 1);
        Arrays.second.resize(MinSize - 1);
        Arrays.first.emplace_back(LastArray1);
        Arrays.second.emplace_back(LastArray2);
    }
}

std::vector<double> StellarGenerator::InterpolateHrDiagram(const std::shared_ptr<StellarGenerator::HrDiagram>& Data, double BvColorIndex) {
    std::vector<double> Result;

    std::pair<std::vector<double>, std::vector<double>> SurroundingRows;
    try {
        SurroundingRows = Data->FindSurroundingValues("B-V", BvColorIndex);
    } catch (std::out_of_range& e) {
        NpgsCoreError(std::string("H-R Diagram interpolation capture exception: ") + std::string(e.what()));
    }

    double Coefficient = (BvColorIndex - SurroundingRows.first[0]) / (SurroundingRows.second[0] - SurroundingRows.first[0]);

    auto& Array1 = SurroundingRows.first;
    auto& Array2 = SurroundingRows.second;

    while (!Array1.empty() && !Array2.empty() && (Array1.back() == -1 || Array2.back() == -1)) {
        Array1.pop_back();
        Array2.pop_back();
    }

    Result = InterpolateArray(SurroundingRows, Coefficient);

    return Result;
}

std::vector<double> StellarGenerator::InterpolateStarData(const std::shared_ptr<StellarGenerator::MistData>& Data, double EvolutionProgress) {
    return InterpolateStarData(Data, EvolutionProgress, "x", StellarGenerator::_kXIndex, false);
}

std::vector<double> StellarGenerator::InterpolateStarData(const std::shared_ptr<StellarGenerator::WdMistData>& Data, double TargetAge) {
    return InterpolateStarData(Data, TargetAge, "star_age", StellarGenerator::_kWdStarAgeIndex, true);
}

std::vector<double> StellarGenerator::InterpolateStarData(const auto& Data, double Target, const std::string& Header, int Index, bool bIsWhiteDwarf) {
    std::vector<double> Result;

    std::pair<std::vector<double>, std::vector<double>> SurroundingRows;
    try {
        SurroundingRows = Data->FindSurroundingValues(Header, Target);
    } catch (std::out_of_range& e) {
        if (!bIsWhiteDwarf) {
            NpgsCoreError(std::string("Stellar data interpolation capture exception: ") + std::string(e.what()));
            NpgsCoreError("Header: {}, Target: {}", Header, Target);
        } else {
            SurroundingRows.first = Data->Data()->back();
            SurroundingRows.second = Data->Data()->back();
        }
    }

    if (SurroundingRows.first != SurroundingRows.second) {
        if (!bIsWhiteDwarf) {
            int LowerPhase = static_cast<int>(SurroundingRows.first[Index]);
            int UpperPhase = static_cast<int>(SurroundingRows.second[Index]);
            if (LowerPhase != UpperPhase) {
                SurroundingRows.second[Index] = LowerPhase + 1;
            }
        }

        double Coefficient = (Target - SurroundingRows.first[Index]) /
            (SurroundingRows.second[Index] - SurroundingRows.first[Index]);
        Result = InterpolateFinalData(SurroundingRows, Coefficient, bIsWhiteDwarf);
    } else {
        Result = SurroundingRows.first;
    }

    return Result;
}

std::vector<double> StellarGenerator::InterpolateArray(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Coefficient) {
    if (DataArrays.first.size() != DataArrays.second.size()) {
        throw std::runtime_error("Data arrays size mismatch.");
    }

    std::size_t Size = DataArrays.first.size();
    std::vector<double> Result(Size);
    for (std::size_t i = 0; i != Size; ++i) {
        Result[i] = DataArrays.first[i] + (DataArrays.second[i] - DataArrays.first[i]) * Coefficient;
    }

    return Result;
}

std::vector<double> StellarGenerator::InterpolateFinalData(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Coefficient, bool bIsWhiteDwarf) {
    if (DataArrays.first.size() != DataArrays.second.size()) {
        throw std::runtime_error("Data arrays size mismatch.");
    }

    std::vector<double> Result = InterpolateArray(DataArrays, Coefficient);

    if (!bIsWhiteDwarf) {
        Result[StellarGenerator::_kPhaseIndex] = DataArrays.first[StellarGenerator::_kPhaseIndex];
    }

    return Result;
}

void StellarGenerator::CalculateSpectralType(float FeH, Astro::Star& StarData) {
    float Teff = StarData.GetTeff();
    auto EvolutionPhase = StarData.GetEvolutionPhase();

    Util::StellarClass::StarType StarType = StarData.GetStellarClass().GetStarType();
    Util::StellarClass::SpectralType SpectralType{};
    SpectralType.bIsAmStar = false;

    std::vector<std::pair<int, int>> SpectralSubclassMap;
    float Subclass = 0.0f;

    float SurfaceH1 = StarData.GetSurfaceH1();
    float MinSurfaceH1 = Astro::Star::_kFeHSurfaceH1Map.at(FeH) - 0.01f;

    std::function<void(Astro::Star::EvolutionPhase)> CalculateSpectralSubclass =
    [&](Astro::Star::EvolutionPhase BasePhase) -> void {
        std::uint32_t SpectralClass = BasePhase == Astro::Star::EvolutionPhase::kWolfRayet ? 11 : 0;

        if (BasePhase != Astro::Star::EvolutionPhase::kWolfRayet) {
            if (BasePhase == Astro::Star::EvolutionPhase::kMainSequence) {
                // 如果表面氢质量分数低于 0.5 并且还是主序星阶段，转为 WR 星
                // 该情况只有 O 型星会出现
                if (SurfaceH1 < 0.5f) {
                    EvolutionPhase = Astro::Star::EvolutionPhase::kWolfRayet;
                    StarData.SetEvolutionPhase(EvolutionPhase);
                    CalculateSpectralSubclass(EvolutionPhase);
                    return;
                }
            }

            const auto& InitialMap = Astro::Star::_kInitialCommonMap;
            for (auto it = InitialMap.begin(); it != InitialMap.end() - 1; ++it) {
                ++SpectralClass;
                if (it->first >= Teff && (it + 1)->first < Teff) {
                    SpectralSubclassMap = it->second;
                    break;
                }
            }
        } else {
            if (Teff >= 200000) { // 温度超过 20 万 K，直接赋值为 WO2
                SpectralType.HSpectralClass = Util::StellarClass::SpectralClass::kSpectral_WO;
                SpectralType.Subclass = 2.0f;
                return;
            } else {
                if (SurfaceH1 >= 0.2f) { // 根据表面氢质量分数来判断处于的 WR 阶段
                    SpectralSubclassMap = Astro::Star::_kSpectralSubclassMap_WNxh;
                    SpectralClass = 13;
                    SpectralType.SpecialMark = std::to_underlying(Util::StellarClass::SpecialPeculiarities::kCode_h);
                } else if (SurfaceH1 >= 0.1f) {
                    SpectralSubclassMap = Astro::Star::_kSpectralSubclassMap_WN;
                    SpectralClass = 13;
                } else if (SurfaceH1 < 0.1f && SurfaceH1 > 0.05f) {
                    SpectralSubclassMap = Astro::Star::_kSpectralSubclassMap_WC;
                    SpectralClass = 12;
                } else {
                    SpectralSubclassMap = Astro::Star::_kSpectralSubclassMap_WO;
                    SpectralClass = 14;
                }
            }
        }

        SpectralType.HSpectralClass = static_cast<Util::StellarClass::SpectralClass>(SpectralClass);

        if (SpectralSubclassMap.empty()) {
            std::println("Age: {}, FeH: {}, Mass: {}, Teff: {}",
                StarData.GetAge(), StarData.GetFeH(), StarData.GetMass() / kSolarMass, StarData.GetTeff());
        }

        for (auto it = SpectralSubclassMap.begin(); it != SpectralSubclassMap.end() - 1; ++it) {
            if (it->first >= Teff && (it + 1)->first < Teff) {
                Subclass = static_cast<float>(it->second);
                break;
            }
        }

        if (SpectralType.HSpectralClass == Util::StellarClass::SpectralClass::kSpectral_WN &&
            SpectralType.SpecialMark & std::to_underlying(Util::StellarClass::SpecialPeculiarities::kCode_h)) {
            if (Subclass < 5) {
                Subclass = 5.0f;
            }
        }

        if (SpectralType.HSpectralClass == Util::StellarClass::SpectralClass::kSpectral_WO) {
            if (Subclass > 4) {
                Subclass = 4.0f;
            }
        }

        SpectralType.Subclass = Subclass;
    };

    if (EvolutionPhase != Astro::Star::EvolutionPhase::kWolfRayet) {
        switch (StarType) {
        case Util::StellarClass::StarType::kNormalStar: {
            if (Teff < 54000) {
                CalculateSpectralSubclass(EvolutionPhase);

                if (EvolutionPhase != Astro::Star::EvolutionPhase::kWolfRayet) {
                    if (EvolutionPhase == Astro::Star::EvolutionPhase::kPrevMainSequence) {
                        SpectralType.LuminosityClass = CalculateLuminosityClass(StarData);
                    } else if (EvolutionPhase == Astro::Star::EvolutionPhase::kMainSequence) {
                        if (SpectralType.HSpectralClass == Util::StellarClass::SpectralClass::kSpectral_O && SurfaceH1 < MinSurfaceH1) {
                            SpectralType.LuminosityClass = CalculateLuminosityClass(StarData);
                        } else {
                            SpectralType.LuminosityClass = Util::StellarClass::LuminosityClass::kLuminosity_V;
                        }
                    } else {
                        SpectralType.LuminosityClass = CalculateLuminosityClass(StarData);
                    }
                } else {
                    SpectralType.LuminosityClass = Util::StellarClass::LuminosityClass::kLuminosity_Unknown;
                }
            } else { // 高于 O2 上限，转到通过表面氢质量分数判断阶段
                if (SurfaceH1 > MinSurfaceH1) {
                    SpectralType.HSpectralClass = Util::StellarClass::SpectralClass::kSpectral_O;
                    SpectralType.Subclass = 2.0f;
                    SpectralType.LuminosityClass = Util::StellarClass::LuminosityClass::kLuminosity_V;
                } else if (SurfaceH1 > 0.5f) {
                    SpectralType.HSpectralClass = Util::StellarClass::SpectralClass::kSpectral_O;
                    SpectralType.Subclass = 2.0f;
                    SpectralType.LuminosityClass = CalculateLuminosityClass(StarData);
                } else {
                    CalculateSpectralSubclass(Astro::Star::EvolutionPhase::kWolfRayet);
                }
            }

            break;
        }
        case Util::StellarClass::StarType::kWhiteDwarf: {
            double MassSol = StarData.GetMass() / kSolarMass;

            if (Teff >= 12000) {
                if (MassSol <= 0.5) {
                    SpectralType.HSpectralClass = Util::StellarClass::SpectralClass::kSpectral_DA;
                } else {
                    if (Teff > 45000) {
                        SpectralType.HSpectralClass = Util::StellarClass::SpectralClass::kSpectral_DO;
                    } else {
                        SpectralType.HSpectralClass = Util::StellarClass::SpectralClass::kSpectral_DB;
                    }
                }
            } else {
                SpectralType.HSpectralClass = Util::StellarClass::SpectralClass::kSpectral_DC;
            }

            Subclass = 50400.0f / Teff;
            if (Subclass > 9.5f) {
                Subclass = 9.5f;
            }

            SpectralType.Subclass = std::round(Subclass * 2.0f) / 2.0f;

            break;
        }
        case Util::StellarClass::StarType::kNeutronStar: {
            SpectralType.HSpectralClass = Util::StellarClass::SpectralClass::kSpectral_Q;
            break;
        }
        case Util::StellarClass::StarType::kBlackHole: {
            SpectralType.HSpectralClass = Util::StellarClass::SpectralClass::kSpectral_X;
            break;
        }
        case Util::StellarClass::StarType::kDeathStarPlaceholder: {
            SpectralType.HSpectralClass = Util::StellarClass::SpectralClass::kSpectral_Unknown;
            break;
        }
        default: {
            SpectralType.HSpectralClass = Util::StellarClass::SpectralClass::kSpectral_Unknown;
            break;
        }
        }
    } else {
        CalculateSpectralSubclass(Astro::Star::EvolutionPhase::kWolfRayet);
        SpectralType.LuminosityClass = Util::StellarClass::LuminosityClass::kLuminosity_Unknown;
    }

    Util::StellarClass Class(StarType, SpectralType);
    StarData.SetStellarClass(Class);
}

Util::StellarClass::LuminosityClass StellarGenerator::CalculateLuminosityClass(const Astro::Star& StarData) {
    float MassLossRateSolPerYear = StarData.GetStellarWindMassLossRate() * kYearToSeconds / kSolarMass;
    double MassSol = StarData.GetMass() / kSolarMass;
    Util::StellarClass::LuminosityClass LuminosityClass = Util::StellarClass::LuminosityClass::kLuminosity_Unknown;

    double LuminositySol = StarData.GetLuminosity() / kSolarLuminosity;
    if (LuminositySol > 650000) { // 光度高于 650000 Lsun
        LuminosityClass = Util::StellarClass::LuminosityClass::kLuminosity_0;
    }

    if (MassLossRateSolPerYear > 1e-4f && MassSol >= 15) { // 表面物质流失率大于 1e-4 Msun/yr 并且质量大于等于 15 Msun
        LuminosityClass = Util::StellarClass::LuminosityClass::kLuminosity_IaPlus;
    }

    if (LuminosityClass != Util::StellarClass::LuminosityClass::kLuminosity_Unknown) { // 如果判断为特超巨星，直接返回
        return LuminosityClass;
    }

    std::string HrDiagramDataFilepath = Asset::GetAssetFilepath(Asset::AssetType::kModel, "H-R Diagram/H-R Diagram.csv");
    std::shared_ptr<HrDiagram> HrDiagramData = LoadCsvAsset<HrDiagram>(HrDiagramDataFilepath, _kHrDiagramHeaders);

    float Teff = StarData.GetTeff();
    float BvColorIndex = 0.0f;
    if (std::log10(Teff) < 3.691f) {
        BvColorIndex = -3.684f * std::log10(Teff) + 14.551f;
    } else {
        BvColorIndex = 0.344f * std::pow(std::log10(Teff), 2.0f) - 3.402f * std::log10(Teff) + 8.037f;
    }

    if (BvColorIndex < -0.3f || BvColorIndex > 1.9727273f) { // 超过 HR 表的范围，使用光度判断
        if (LuminositySol > 100000) {
            return Util::StellarClass::LuminosityClass::kLuminosity_Ia;
        } else if (LuminositySol > 50000) {
            return Util::StellarClass::LuminosityClass::kLuminosity_Iab;
        } else if (LuminositySol > 10000) {
            return Util::StellarClass::LuminosityClass::kLuminosity_Ib;
        } else if (LuminositySol > 1000) {
            return Util::StellarClass::LuminosityClass::kLuminosity_II;
        } else if (LuminositySol > 100) {
            return Util::StellarClass::LuminosityClass::kLuminosity_III;
        } else if (LuminositySol > 10) {
            return Util::StellarClass::LuminosityClass::kLuminosity_IV;
        } else if (LuminositySol > 0.05) {
            return Util::StellarClass::LuminosityClass::kLuminosity_V;
        } else {
            return Util::StellarClass::LuminosityClass::kLuminosity_VI;
        }
    }

    std::vector<double> LuminosityData = InterpolateHrDiagram(HrDiagramData, BvColorIndex);
    if (LuminositySol > LuminosityData[1]) {
        return Util::StellarClass::LuminosityClass::kLuminosity_Ia;
    }

    double ClosestValue = *std::min_element(LuminosityData.begin() + 1, LuminosityData.end(),
        [LuminositySol](double Lhs, double Rhs) -> bool {
            return std::abs(Lhs - LuminositySol) < std::abs(Rhs - LuminositySol);
        }
    );

    while (LuminosityData.size() < 7) {
        LuminosityData.emplace_back(-1);
    }

    if (LuminositySol <= LuminosityData[1] && LuminositySol >= LuminosityData[2] &&
        (ClosestValue == LuminosityData[1] || ClosestValue == LuminosityData[2])) {
        LuminosityClass = Util::StellarClass::LuminosityClass::kLuminosity_Iab;
    } else {
        if (ClosestValue == LuminosityData[2]) {
            LuminosityClass = Util::StellarClass::LuminosityClass::kLuminosity_Ib;
        } else if (ClosestValue == LuminosityData[3]) {
            LuminosityClass = Util::StellarClass::LuminosityClass::kLuminosity_II;
        } else if (ClosestValue == LuminosityData[4]) {
            LuminosityClass = Util::StellarClass::LuminosityClass::kLuminosity_III;
        } else if (ClosestValue == LuminosityData[5]) {
            LuminosityClass = Util::StellarClass::LuminosityClass::kLuminosity_IV;
        } else if (ClosestValue == LuminosityData[6]) {
            LuminosityClass = Util::StellarClass::LuminosityClass::kLuminosity_V;
        }
    }

    return LuminosityClass;
}

void StellarGenerator::ProcessDeathStar(Astro::Star& DeathStar, GenerateOption Option) {
    double InputAge     = DeathStar.GetAge();
    float  InputFeH     = DeathStar.GetFeH();
    float  InputMassSol = DeathStar.GetInitialMass();

    Astro::Star::EvolutionPhase EvolutionPhase{};
    Astro::Star::StarFrom       DeathStarFrom{};
    Util::StellarClass::StarType      DeathStarType{};
    Util::StellarClass::SpectralType  DeathStarClass{};

    double DeathStarAge = InputAge - static_cast<float>(DeathStar.GetLifetime());
    float DeathStarMassSol = 0.0f;

    auto CalculateBlackHoleMass = [&, this]() -> float {
        BasicProperties Properties{};
        Properties.Age = static_cast<float>(DeathStar.GetLifetime() - 100);
        Properties.FeH = InputFeH;
        Properties.InitialMassSol = InputMassSol;
        Properties.TypeOption = GenerateOption::kNormal;

        auto GiantStar = GenerateStar(Properties);

        return static_cast<float>(GiantStar.GetMass() / kSolarMass * 0.8);
    };

    if (InputFeH <= -2.0f && InputMassSol >= 140 && InputMassSol < 250) {
        EvolutionPhase = Astro::Star::EvolutionPhase::kNull;
        DeathStarFrom  = Astro::Star::StarFrom::kPairInstabilitySupernova;
        DeathStarType  = Util::StellarClass::StarType::kDeathStarPlaceholder;
        DeathStarClass = {
            Util::StellarClass::SpectralClass::kSpectral_Unknown,
            Util::StellarClass::SpectralClass::kSpectral_Unknown,
            Util::StellarClass::LuminosityClass::kLuminosity_Unknown,
            0, 0.0f, 0.0f, false
        };
    } else if (InputFeH <= -2.0f && InputMassSol >= 250) {
        EvolutionPhase   = Astro::Star::EvolutionPhase::kStellarBlackHole;
        DeathStarFrom    = Astro::Star::StarFrom::kPhotondisintegration;
        DeathStarType    = Util::StellarClass::StarType::kBlackHole;
        DeathStarClass   = {
            Util::StellarClass::SpectralClass::kSpectral_X,
            Util::StellarClass::SpectralClass::kSpectral_Unknown,
            Util::StellarClass::LuminosityClass::kLuminosity_Unknown,
            0, 0.0f, 0.0f, false
        };
        DeathStarMassSol = CalculateBlackHoleMass();
    } else {
        if (InputMassSol > -0.75f && InputMassSol < 0.8f) {
            DeathStarMassSol = (0.9795f - 0.393f * InputMassSol) * InputMassSol;
        } else if (InputMassSol >= 0.8f && InputMassSol < 7.9f) {
            DeathStarMassSol = -0.00012336f * std::pow(InputMassSol, 6.0f) + 0.003160f * std::pow(InputMassSol, 5.0f) - 0.02960f * std::pow(InputMassSol, 4.0f) + 0.12350f * std::pow(InputMassSol, 3.0f) - 0.21550f * std::pow(InputMassSol, 2.0f) + 0.19022f * InputMassSol + 0.46575f;
        } else if (InputMassSol >= 7.9f && InputMassSol < 10.0f) {
            DeathStarMassSol = 1.301f + 0.008095f * InputMassSol;
        } else if (InputMassSol >= 10.0f && InputMassSol < 21.0f) {
            DeathStarMassSol = 1.246f + 0.0136f * InputMassSol;
        } else if (InputMassSol >= 21.0f && InputMassSol < 23.3537f) {
            DeathStarMassSol = std::pow(10.0f, (1.334f - 0.009987f * InputMassSol));
        } else if (InputMassSol >= 23.3537f && InputMassSol < 33.75f) {
            DeathStarMassSol = 12.1f - 0.763f * InputMassSol + 0.0137f * std::pow(InputMassSol, 2.0f);
        } else {
            DeathStarMassSol = CalculateBlackHoleMass();
        }

        if (InputMassSol >= 0.075f && InputMassSol < 0.5f) {
            EvolutionPhase = Astro::Star::EvolutionPhase::kHeliumWhiteDwarf;
            DeathStarFrom  = Astro::Star::StarFrom::kSlowColdingDown;
            DeathStarType  = Util::StellarClass::StarType::kWhiteDwarf;
            DeathStarClass = {
                Util::StellarClass::SpectralClass::kSpectral_Unknown,
                Util::StellarClass::SpectralClass::kSpectral_Unknown,
                Util::StellarClass::LuminosityClass::kLuminosity_Unknown,
                0, 0.0f, 0.0f, false
            };
        } else if (InputMassSol >= 0.5f && InputMassSol < 8.0f) {
            EvolutionPhase = Astro::Star::EvolutionPhase::kCarbonOxygenWhiteDwarf;
            DeathStarFrom  = Astro::Star::StarFrom::kEnvelopeDisperse;
            DeathStarType  = Util::StellarClass::StarType::kWhiteDwarf;
            DeathStarClass = {
                Util::StellarClass::SpectralClass::kSpectral_Unknown,
                Util::StellarClass::SpectralClass::kSpectral_Unknown,
                Util::StellarClass::LuminosityClass::kLuminosity_Unknown,
                0, 0.0f, 0.0f, false
            };
        } else if (InputMassSol >= 8.0f && InputMassSol < 9.759f) {
            EvolutionPhase = Astro::Star::EvolutionPhase::kOxygenNeonMagnWhiteDwarf;
            DeathStarFrom  = Astro::Star::StarFrom::kEnvelopeDisperse;
            DeathStarType  = Util::StellarClass::StarType::kWhiteDwarf;
            DeathStarClass = {
                Util::StellarClass::SpectralClass::kSpectral_Unknown,
                Util::StellarClass::SpectralClass::kSpectral_Unknown,
                Util::StellarClass::LuminosityClass::kLuminosity_Unknown,
                0, 0.0f, 0.0f, false
            };
        } else if (InputMassSol >= 9.759f && InputMassSol < 10.0f) {
            EvolutionPhase = Astro::Star::EvolutionPhase::kNeutronStar;
            DeathStarFrom  = Astro::Star::StarFrom::kElectronCaptureSupernova;
            DeathStarType  = Util::StellarClass::StarType::kNeutronStar;
            DeathStarClass = {
                Util::StellarClass::SpectralClass::kSpectral_Q,
                Util::StellarClass::SpectralClass::kSpectral_Unknown,
                Util::StellarClass::LuminosityClass::kLuminosity_Unknown,
                0, 0.0f, 0.0f, false
            };
        } else if (InputMassSol >= 10.0f && InputMassSol < 21.0f) {
            EvolutionPhase = Astro::Star::EvolutionPhase::kNeutronStar;
            DeathStarFrom  = Astro::Star::StarFrom::kIronCoreCollapseSupernova;
            DeathStarType  = Util::StellarClass::StarType::kNeutronStar;
            DeathStarClass = {
                Util::StellarClass::SpectralClass::kSpectral_Q,
                Util::StellarClass::SpectralClass::kSpectral_Unknown,
                Util::StellarClass::LuminosityClass::kLuminosity_Unknown,
                0, 0.0f, 0.0f, false
            };
        } else if (InputMassSol >= 21.0f && InputMassSol < 23.3537f) {
            EvolutionPhase = Astro::Star::EvolutionPhase::kStellarBlackHole;
            DeathStarFrom  = Astro::Star::StarFrom::kIronCoreCollapseSupernova;
            DeathStarType  = Util::StellarClass::StarType::kBlackHole;
            DeathStarClass = {
                Util::StellarClass::SpectralClass::kSpectral_X,
                Util::StellarClass::SpectralClass::kSpectral_Unknown,
                Util::StellarClass::LuminosityClass::kLuminosity_Unknown,
                0, 0.0f, 0.0f, false
            };
        } else if (InputMassSol >= 23.3537f && InputMassSol < 33.75f) {
            EvolutionPhase = Astro::Star::EvolutionPhase::kNeutronStar;
            DeathStarFrom  = Astro::Star::StarFrom::kIronCoreCollapseSupernova;
            DeathStarType  = Util::StellarClass::StarType::kNeutronStar;
            DeathStarClass = {
                Util::StellarClass::SpectralClass::kSpectral_Q,
                Util::StellarClass::SpectralClass::kSpectral_Unknown,
                Util::StellarClass::LuminosityClass::kLuminosity_Unknown,
                0, 0.0f, 0.0f, false
            };
        } else {
            EvolutionPhase = Astro::Star::EvolutionPhase::kStellarBlackHole;
            DeathStarFrom  = Astro::Star::StarFrom::kRelativisticJetHypernova;
            DeathStarType  = Util::StellarClass::StarType::kBlackHole;
            DeathStarClass = {
                Util::StellarClass::SpectralClass::kSpectral_X,
                Util::StellarClass::SpectralClass::kSpectral_Unknown,
                Util::StellarClass::LuminosityClass::kLuminosity_Unknown,
                0, 0.0f, 0.0f, false
            };
        }
    }

    if (Option == GenerateOption::kMergeStar || DeathStarType == Util::StellarClass::StarType::kNeutronStar) {
        float MergeStarProbability = 0.1f * static_cast<int>(DeathStar.GetIsSingleStar());
        MergeStarProbability *= static_cast<int>(Option != GenerateOption::kDeathStar);
        Util::BernoulliDistribution MergeProbability(MergeStarProbability);
        if (Option == GenerateOption::kMergeStar || MergeProbability(_RandomEngine)) {
            DeathStar.SetIsSingleStar(true);
            DeathStarFrom = Astro::Star::StarFrom::kWhiteDwarfMerge;
            Util::BernoulliDistribution BlackHoleProbability(0.114514);
            float MassSol = 0.0f;
            if (BlackHoleProbability(_RandomEngine)) {
                Util::UniformRealDistribution<> MassDistribution(2.6f, 2.76f);
                MassSol        = MassDistribution(_RandomEngine);
                EvolutionPhase = Astro::Star::EvolutionPhase::kStellarBlackHole;
                DeathStarType  = Util::StellarClass::StarType::kBlackHole;
                DeathStarClass = {
                    Util::StellarClass::SpectralClass::kSpectral_X,
                    Util::StellarClass::SpectralClass::kSpectral_Unknown,
                    Util::StellarClass::LuminosityClass::kLuminosity_Unknown,
                    0, 0.0f, 0.0f, false
                };
            } else {
                Util::UniformRealDistribution<> MassDistribution(1.38f, 2.18072f);
                MassSol        = MassDistribution(_RandomEngine);
                EvolutionPhase = Astro::Star::EvolutionPhase::kNeutronStar;
                DeathStarType  = Util::StellarClass::StarType::kNeutronStar;
                DeathStarClass = {
                    Util::StellarClass::SpectralClass::kSpectral_Q,
                    Util::StellarClass::SpectralClass::kSpectral_Unknown,
                    Util::StellarClass::LuminosityClass::kLuminosity_Unknown,
                    0, 0.0f, 0.0f, false
                };
            }

            DeathStarMassSol = MassSol;
        }
    }

    float StarAge                 = 0.0f;
    float LogR                    = 0.0f;
    float LogTeff                 = 0.0f;
    float LogCenterT              = 0.0f;
    float LogCenterRho            = 0.0f;
    float SurfaceZ                = 0.0f;
    float SurfaceEnergeticNuclide = 0.0f;
    float SurfaceVolatiles        = 0.0f;

    switch (DeathStarType) {
    case Util::StellarClass::StarType::kWhiteDwarf: {
        std::vector<double> WhiteDwarfData =
            GetActuallyMistData({ static_cast<float>(DeathStarAge), 0.0f, DeathStarMassSol }, true, true);

        StarAge      = static_cast<float>(WhiteDwarfData[_kWdStarAgeIndex]);
        LogR         = static_cast<float>(WhiteDwarfData[_kWdLogRIndex]);
        LogTeff      = static_cast<float>(WhiteDwarfData[_kWdLogTeffIndex]);
        LogCenterT   = static_cast<float>(WhiteDwarfData[_kWdLogCenterTIndex]);
        LogCenterRho = static_cast<float>(WhiteDwarfData[_kWdLogCenterRhoIndex]);

        if (DeathStarMassSol < 0.2f || DeathStarMassSol > 1.3f) {
            LogR         = std::log10(0.0323f - 0.021384f * DeathStarMassSol);
            LogCenterT   = std::numeric_limits<float>::min();
            LogCenterRho = std::numeric_limits<float>::min();
        }

        if (DeathStarAge > StarAge) {
            LogTeff    = static_cast<float>(std::log10(std::pow(10.0, LogTeff) *
                std::pow((20.0 * StarAge) / (DeathStarAge + 19.0 * StarAge), 7.0 / 4.0)));
            LogCenterT = std::numeric_limits<float>::min();
        }

        SurfaceZ                = 0.0f;
        SurfaceEnergeticNuclide = 0.0f;
        SurfaceVolatiles        = 1.0f;

        break;
    }
    case Util::StellarClass::StarType::kNeutronStar: {
        if (DeathStarAge < 1e5f) {
            DeathStarAge += 1e5f;
        }

        float Radius = 0.0f;
        if (DeathStarMassSol <= 0.77711f) {
            Radius = -4.783f + 2.565f / DeathStarMassSol + 42.0f * DeathStarMassSol - 55.4f * std::pow(DeathStarMassSol, 2.0f) +
                34.93f * std::pow(DeathStarMassSol, 3.0f) - 8.4f * std::pow(DeathStarMassSol, 4.0f);
        } else if (DeathStarMassSol <= 2.0181f) {
            Radius = 11.302f - 0.35184f * DeathStarMassSol;
        } else {
            Radius = -31951.1f + 63121.8f * DeathStarMassSol - 46717.8f * std::pow(DeathStarMassSol, 2.0f) +
                15358.4f * std::pow(DeathStarMassSol, 3.0f) - 1892.365f * std::pow(DeathStarMassSol, 4.0f);
        }

        LogR    = std::log10(Radius * 1000 / kSolarRadius);
        LogTeff = static_cast<float>(std::log10(1.5e8 * std::pow((DeathStarAge - 1e5) + 22000, -0.5)));

        SurfaceZ                = std::numeric_limits<float>::quiet_NaN();
        SurfaceEnergeticNuclide = std::numeric_limits<float>::quiet_NaN();
        SurfaceVolatiles        = std::numeric_limits<float>::quiet_NaN();

        break;
    }
    case Util::StellarClass::StarType::kBlackHole: {
        LogR                    = std::numeric_limits<float>::quiet_NaN();
        LogTeff                 = std::numeric_limits<float>::quiet_NaN();
        LogCenterT              = std::numeric_limits<float>::quiet_NaN();
        LogCenterRho            = std::numeric_limits<float>::quiet_NaN();
        SurfaceZ                = std::numeric_limits<float>::quiet_NaN();
        SurfaceEnergeticNuclide = std::numeric_limits<float>::quiet_NaN();
        SurfaceVolatiles        = std::numeric_limits<float>::quiet_NaN();

        break;
    }
    default: {
        break;
    }
    }

    double EvolutionProgress = static_cast<double>(EvolutionPhase);
    double Age               = DeathStarAge;
    float  MassSol           = DeathStarMassSol;
    float  RadiusSol         = std::pow(10.0f, LogR);
    float  Teff              = std::pow(10.0f, LogTeff);
    float  CoreTemp          = std::pow(10.0f, LogCenterT);
    float  CoreDensity       = std::pow(10.0f, LogCenterRho);

    float LuminositySol  = std::pow(RadiusSol, 2.0f) * std::pow((Teff / kSolarTeff), 4.0f);
    float EscapeVelocity = std::sqrt((2.0f * kGravityConstant * MassSol * kSolarMass) / (RadiusSol * kSolarRadius));

    float Theta = _CommonGenerator(_RandomEngine) * 2.0f * kPi;
    float Phi   = _CommonGenerator(_RandomEngine) * kPi;

    DeathStar.SetInitialMass(InputMassSol * kSolarMass);
    DeathStar.SetAge(Age);
    DeathStar.SetMass(MassSol * kSolarMass);
    DeathStar.SetLifetime(-DeathStar.GetLifetime());
    DeathStar.SetEvolutionProgress(EvolutionProgress);
    DeathStar.SetRadius(RadiusSol * kSolarRadius);
    DeathStar.SetEscapeVelocity(EscapeVelocity);
    DeathStar.SetLuminosity(LuminositySol * kSolarLuminosity);
    DeathStar.SetTeff(Teff);
    DeathStar.SetSurfaceZ(SurfaceZ);
    DeathStar.SetSurfaceEnergeticNuclide(SurfaceEnergeticNuclide);
    DeathStar.SetSurfaceVolatiles(SurfaceVolatiles);
    DeathStar.SetCoreTemp(CoreTemp);
    DeathStar.SetCoreDensity(CoreDensity * 1000);
    DeathStar.SetEvolutionPhase(EvolutionPhase);
    DeathStar.SetNormal(glm::vec2(Theta, Phi));
    DeathStar.SetStarFrom(DeathStarFrom);
    DeathStar.SetStellarClass(Util::StellarClass(DeathStarType, DeathStarClass));

    CalculateSpectralType(0.0, DeathStar);
    GenerateMagnetic(DeathStar);
    GenerateSpin(DeathStar);
}

void StellarGenerator::GenerateMagnetic(Astro::Star& StarData) {
    Util::Distribution<>* MagneticGenerator = nullptr;

    Util::StellarClass::StarType StarType = StarData.GetStellarClass().GetStarType();
    float MassSol = static_cast<float>(StarData.GetMass() / kSolarMass);
    Astro::Star::EvolutionPhase EvolutionPhase = StarData.GetEvolutionPhase();

    float MagneticField = 0.0f;

    switch (StarType) {
    case Util::StellarClass::StarType::kNormalStar: {
        if (MassSol >= 0.075f && MassSol < 0.33f) {
            MagneticGenerator = &_MagneticGenerators[0];
        } else if (MassSol >= 0.33f && MassSol < 0.6f) {
            MagneticGenerator = &_MagneticGenerators[1];
        } else if (MassSol >= 0.6f && MassSol < 1.5f) {
            MagneticGenerator = &_MagneticGenerators[2];
        } else if (MassSol >= 1.5f && MassSol < 20.0f) {
            auto SpectralType = StarData.GetStellarClass().Data();
            if (EvolutionPhase == Astro::Star::EvolutionPhase::kMainSequence &&
                (SpectralType.HSpectralClass == Util::StellarClass::SpectralClass::kSpectral_A ||
                    SpectralType.HSpectralClass == Util::StellarClass::SpectralClass::kSpectral_B)) {
                Util::BernoulliDistribution ProbabilityGenerator(0.15); //  p 星的概率
                if (ProbabilityGenerator(_RandomEngine)) {
                    MagneticGenerator = &_MagneticGenerators[3];
                    SpectralType.SpecialMark |= std::to_underlying(Util::StellarClass::SpecialPeculiarities::kCode_p);
                    StarData.SetStellarClass(Util::StellarClass(Util::StellarClass::StarType::kNormalStar, SpectralType));
                } else {
                    MagneticGenerator = &_MagneticGenerators[4];
                }
            } else {
                MagneticGenerator = &_MagneticGenerators[4];
            }
        } else {
            MagneticGenerator = &_MagneticGenerators[5];
        }

        MagneticField = std::pow(10.0f, (*MagneticGenerator)(_RandomEngine)) / 10000;

        break;
    }
    case Util::StellarClass::StarType::kWhiteDwarf: {
        MagneticGenerator = &_MagneticGenerators[6];
        MagneticField = std::pow(10.0f, (*MagneticGenerator)(_RandomEngine));
        break;
    }
    case Util::StellarClass::StarType::kNeutronStar: {
        MagneticGenerator = &_MagneticGenerators[7];
        MagneticField = (*MagneticGenerator)(_RandomEngine) / static_cast<float>(std::pow((0.034f * StarData.GetAge() / 1e4f), 1.17f) + 0.84f);
        break;
    }
    case Util::StellarClass::StarType::kBlackHole: {
        MagneticField = 0.0f;
        break;
    }
    case Util::StellarClass::StarType::kDeathStarPlaceholder: {
        break;
    }
    default: {
        break;
    }
    }

    StarData.SetMagneticField(MagneticField);
}

void StellarGenerator::GenerateSpin(Astro::Star& StarData) {
    Util::StellarClass::StarType StarType = StarData.GetStellarClass().GetStarType();
    float StarAge   = static_cast<float>(StarData.GetAge());
    float MassSol   = static_cast<float>(StarData.GetMass() / kSolarMass);
    float RadiusSol = StarData.GetRadius() / kSolarRadius;
    float Spin      = 0.0f;

    Util::Distribution<float>* SpinGenerator = nullptr;

    switch (StarType) {
    case Util::StellarClass::StarType::kNormalStar: {
        float Base = 1.0f + _CommonGenerator(_RandomEngine);
        if (StarData.GetStellarClass().Data().SpecialMark & std::to_underlying(Util::StellarClass::SpecialPeculiarities::kCode_p)) {
            Base *= 10;
        }

        float LgMass = std::log10(MassSol);
        float Term1  = 0.0f;
        float Term2  = 0.0f;
        float Term3  = std::pow(2.0f, std::sqrt(Base * (StarAge + 1e6f) * 1e-9f));

        if (MassSol <= 1.4f) {
            Term1 = std::pow(10.0f, 30.893f - 25.34303f * std::exp(LgMass) + 21.7577f * LgMass +
                7.34205f * std::pow(LgMass, 2.0f) + 0.12951f * std::pow(LgMass, 3.0f));
            Term2 = std::pow(RadiusSol / std::pow(MassSol, 0.9f), 2.5f);
        } else {
            Term1 = std::pow(10.0f, 28.0784f - 22.15753f * std::exp(LgMass) + 12.55134f * LgMass +
                30.9045f * std::pow(LgMass, 2.0f) - 10.1479f * std::pow(LgMass, 3.0f) + 4.6894f * std::pow(LgMass, 4.0f));
            Term2 = std::pow(RadiusSol / (1.1062f * std::pow(MassSol, 0.6f)), 2.5f);
        }

        Spin = Term1 * Term2 * Term3;

        break;
    }
    case Util::StellarClass::StarType::kWhiteDwarf: {
        SpinGenerator = &_SpinGenerators[0];
        Spin = std::pow(10.0f, (*SpinGenerator)(_RandomEngine));
        break;
    }
    case Util::StellarClass::StarType::kNeutronStar: {
        Spin = (StarAge * 3 * 1e-9f) + 1e-3f;
        break;
    }
    case Util::StellarClass::StarType::kBlackHole: { // 此处表示无量纲自旋参数，而非自转时间
        SpinGenerator = &_SpinGenerators[1];
        Spin = (*SpinGenerator)(_RandomEngine);
        break;
    }
    default: {
        break;
    }
    }

    if (StarType != Util::StellarClass::StarType::kBlackHole) {
        float Oblateness = 4.0f * std::pow(kPi, 2.0f) * std::pow(StarData.GetRadius(), 3.0f);
        Oblateness /= (std::pow(Spin, 2.0f) * kGravityConstant * static_cast<float>(StarData.GetMass()));
        StarData.SetOblateness(Oblateness);
    }

    StarData.SetSpin(Spin);
}

void StellarGenerator::ExpandMistData(double TargetMass, std::vector<double>& StarData) {
    double RadiusSol     = std::pow(10.0, StarData[_kLogRIndex]);
    double Teff          = std::pow(10.0, StarData[_kLogTeffIndex]);
    double LuminositySol = std::pow(RadiusSol, 2.0) * std::pow((Teff / kSolarTeff), 4.0);

    double& StarMass = StarData[_kStarMassIndex];
    double& StarMdot = StarData[_kStarMdotIndex];
    double& LogR     = StarData[_kLogRIndex];
    double& LogTeff  = StarData[_kLogTeffIndex];

    double LogL = std::log10(LuminositySol);

    StarMass = TargetMass * (StarMass / 0.1);
    StarMdot = TargetMass * (StarMdot / 0.1);

    RadiusSol = std::pow(10.0, LogR) * std::pow(TargetMass / 0.1, 2.3);
    LuminositySol = std::pow(10.0, LogL) * std::pow(TargetMass / 0.1, 2.3);

    Teff = kSolarTeff * std::pow((LuminositySol / std::pow(RadiusSol, 2.0)), 0.25);
    LogTeff = std::log10(Teff);

    LogR = std::log10(RadiusSol);
}

template<typename CsvType>
std::shared_ptr<CsvType> StellarGenerator::LoadCsvAsset(const std::string& Filename, const std::vector<std::string>& Headers) {
    {
        std::shared_lock Lock(_kCacheMutex);
        auto Asset = Asset::AssetManager::GetAsset<CsvType>(Filename);
        if (Asset != nullptr) {
            return Asset;
        }
    }

    std::unique_lock Lock(_kCacheMutex);
    auto CsvAsset = std::make_shared<CsvType>(Filename, Headers);
    Asset::AssetManager::AddAsset<CsvType>(Filename, CsvAsset);

    return CsvAsset;
}

const int StellarGenerator::_kStarAgeIndex        = 0;
const int StellarGenerator::_kStarMassIndex       = 1;
const int StellarGenerator::_kStarMdotIndex       = 2;
const int StellarGenerator::_kLogTeffIndex        = 3;
const int StellarGenerator::_kLogRIndex           = 4;
const int StellarGenerator::_kLogSurfZIndex       = 5;
const int StellarGenerator::_kSurfaceH1Index      = 6;
const int StellarGenerator::_kSurfaceHe3Index     = 7;
const int StellarGenerator::_kLogCenterTIndex     = 8;
const int StellarGenerator::_kLogCenterRhoIndex   = 9;
const int StellarGenerator::_kPhaseIndex          = 10;
const int StellarGenerator::_kXIndex              = 11;
const int StellarGenerator::_kLifetimeIndex       = 12;

const int StellarGenerator::_kWdStarAgeIndex      = 0;
const int StellarGenerator::_kWdLogRIndex         = 1;
const int StellarGenerator::_kWdLogTeffIndex      = 2;
const int StellarGenerator::_kWdLogCenterTIndex   = 3;
const int StellarGenerator::_kWdLogCenterRhoIndex = 4;

const std::vector<std::string> StellarGenerator::_kMistHeaders{
    "star_age", "star_mass", "star_mdot", "log_Teff", "log_R", "log_surf_z",
    "surface_h1", "surface_he3", "log_center_T", "log_center_Rho", "phase", "x"
};

const std::vector<std::string> StellarGenerator::_kWdMistHeaders{
    "star_age", "log_R", "log_Teff", "log_center_T", "log_center_Rho"
};

const std::vector<std::string> StellarGenerator::_kHrDiagramHeaders{ "B-V", "Ia", "Ib", "II", "III", "IV", "V" };
std::unordered_map<std::string, std::vector<float>> StellarGenerator::_kMassFileCache;
std::unordered_map<std::shared_ptr<StellarGenerator::MistData>, std::vector<std::vector<double>>> StellarGenerator::_kPhaseChangesCache;
std::shared_mutex StellarGenerator::_kCacheMutex;
bool StellarGenerator::_kbMistDataInitiated = false;

// Tool functions implementations
// ------------------------------
float DefaultAgePdf(const glm::vec3&, float Age, float UniverseAge) {
    float Probability = 0.0f;
    if (Age - (UniverseAge - 13.8f) < 8.0f) {
        Probability = std::exp((Age - (UniverseAge - 13.8f) / 8.4f));
    } else {
        Probability = 2.6f * std::exp((-0.5f * std::pow((Age - (UniverseAge - 13.8f)) - 8.0f, 2.0f)) / (std::pow(1.5f, 2.0f)));
    }

    return static_cast<float>(Probability);
}

float DefaultLogMassPdfSingleStar(float LogMassSol) {
    float Probability = 0.0f;

    if (std::pow(10.0f, LogMassSol) <= 1.0f) {
        Probability = 0.158f * std::exp(-1.0f * std::pow(LogMassSol + 1.0f, 2.0f) / 1.101128f);
    } else {
        Probability = 0.06371598f * std::pow(std::pow(10.0f, LogMassSol), -0.8f);
    }

    return Probability;
}

float DefaultLogMassPdfBinaryStar(float LogMassSol) {
    float Probability = 0.0f;
    
    if (std::pow(10.0, LogMassSol) <= 1.0f) {
        Probability = 0.086f * std::exp(-1.0f * std::pow(LogMassSol + 0.65757734f, 2.0f) / 1.101128f);
    } else {
        Probability = 0.058070157f * std::pow(std::pow(10.0f, LogMassSol), -0.65f);
    }

    return Probability;
}

_MODULE_END
_NPGS_END
