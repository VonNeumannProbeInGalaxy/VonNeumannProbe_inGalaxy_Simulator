#include "StellarGenerator.h"

#include <cmath>

#include <algorithm>
#include <filesystem>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#define ENABLE_LOGGER
#include "Engine/Core/Constants.h"
#include "Engine/Core/Logger.h"

_NPGS_BEGIN
_MODULES_BEGIN

// Tool macros
// -----------
#define GenerateDeathStarPlaceholder(Lifetime) {                                                                                                                                                                           \
    StellarClass::SpectralType DeathStarClass{ StellarClass::SpectralClass::kSpectral_Unknown, StellarClass::SpectralClass::kSpectral_Unknown, StellarClass::LuminosityClass::kLuminosity_Unknown, 0, 0.0f, 0.0f, false }; \
    Astro::Star DeathStar;                                                                                                                                                                                           \
    DeathStar.SetStellarClass(StellarClass(StellarClass::StarType::kDeathStarPlaceholder, DeathStarClass));                                                                                                                \
    DeathStar.SetLifetime(Lifetime);                                                                                                                                                                                       \
    throw DeathStar;                                                                                                                                                                                                       \
}

// Processor functions
// -------------------
static float DefaultAgePdf(float Age, float UniverseAge);
static float DefaultLogMassPdf(float Mass, bool bIsBinary);
double CalcEvolutionProgress(std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& PhaseChanges, double TargetAge, double MassCoefficient);
std::pair<double, std::pair<double, double>> FindSurroundingTimePoints(const std::vector<std::vector<double>>& PhaseChanges, double TargetAge);
std::pair<double, std::size_t> FindSurroundingTimePoints(const std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& PhaseChanges, double TargetAge, double MassCoefficient);
void AlignArrays(std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& Arrays);
std::vector<double> InterpolateRows(const std::shared_ptr<StellarGenerator::HrDiagram>& Data, double BvColorIndex);
std::vector<double> InterpolateRows(const std::shared_ptr<StellarGenerator::MistData>& Data, double EvolutionProgress);
std::vector<double> InterpolateRows(const std::shared_ptr<StellarGenerator::WdMistData>& Data, double TargetAge);
std::vector<double> InterpolateRows(const auto& Data, double Target, const std::string& Header, int Index, bool bIsWhiteDwarf);
std::vector<double> InterpolateArray(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Coefficient);
std::vector<double> InterpolateFinalData(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Coefficient, bool bIsWhiteDwarf);
void ExpandMistData(std::vector<double>& StarData, double TargetMass);

// StellarGenerator implementations
// --------------------------------
StellarGenerator::StellarGenerator(const std::seed_seq& SeedSequence, GenOption Option, float UniverseAge, float MassLowerLimit, float MassUpperLimit, GenDistribution MassDistribution, float AgeLowerLimit, float AgeUpperLimit, GenDistribution AgeDistribution, float FeHLowerLimit, float FeHUpperLimit, GenDistribution FeHDistribution, float CoilTempLimit, float dEpdM)
    :
    _RandomEngine(SeedSequence),
    _MagneticGenerators({
        UniformRealDistribution<>(std::log10(500.0f), std::log10(3000.0f)),
        UniformRealDistribution<>(1.0f, 3.0f),
        UniformRealDistribution<>(0.0f, 1.0f),
        UniformRealDistribution<>(3.0f, 4.0f),
        UniformRealDistribution<>(-1.0f, 0.0f),
        UniformRealDistribution<>(2.0f, 3.0f),
        UniformRealDistribution<>(0.5f, 4.5f),
        UniformRealDistribution<>(1e9f, 1e11f)
        }),

    _FeHGenerators({
        std::make_shared<LogNormalDistribution<>>(-0.3f, 0.5f),
        std::make_shared<NormalDistribution<>>(-0.3f, 0.15f),
        std::make_shared<NormalDistribution<>>(-0.08f, 0.12f),
        std::make_shared<NormalDistribution<>>(0.05f, 0.16f)
        }),

    _SpinGenerators({
        UniformRealDistribution<>(3.0f, 5.0f),
        UniformRealDistribution<>(0.001f, 0.998f)
        }),

    _AgeGenerator(AgeLowerLimit, AgeUpperLimit),
    _CommonGenerator(0.0f, 1.0f),
    _LogMassGenerator(Option == StellarGenerator::GenOption::kMergeStar ? (0.0f, 1.0f) : std::log10(MassLowerLimit), std::log10(MassUpperLimit)),

    _UniverseAge(UniverseAge),
    _AgeLowerLimit(AgeLowerLimit), _AgeUpperLimit(AgeUpperLimit),
    _FeHLowerLimit(FeHLowerLimit), _FeHUpperLimit(FeHUpperLimit),
    _MassLowerLimit(MassLowerLimit), _MassUpperLimit(MassUpperLimit),
    _CoilTempLimit(CoilTempLimit), _dEpdM(dEpdM),

    _AgeDistribution(AgeDistribution), _FeHDistribution(FeHDistribution), _MassDistribution(MassDistribution), _Option(Option)
{
    InitMistData();
}

StellarGenerator::BasicProperties StellarGenerator::GenBasicProperties() {
    BasicProperties Properties{};

    // 生成 3 个基本参数
    if (_MassLowerLimit == 0.0f && _MassUpperLimit == 0.0f) {
        Properties.InitialMass = 0.0f;
    } else {
        switch (_MassDistribution) {
        case GenDistribution::kFromPdf: {
            // TODO: 单星质量 MaxPdf
            float MaxProbability = 0.086f;
            float LogMassLower = std::log10(_MassLowerLimit);
            float LogMassUpper = std::log10(_MassUpperLimit);
            if (!(LogMassLower < 0.22f && LogMassUpper > 0.22f)) { //  调整最大值，防止接受率过低
                if (LogMassLower > 0.22f) {
                    MaxProbability = DefaultLogMassPdf(LogMassLower, true);
                } else if (LogMassUpper < 0.22f) {
                    MaxProbability = DefaultLogMassPdf(LogMassUpper, true);
                }
            }
            Properties.InitialMass = GenerateMass(MaxProbability, true);
            break;
        }
        case GenDistribution::kUniform: {
            Properties.InitialMass = _MassLowerLimit + _CommonGenerator(_RandomEngine) * (_MassUpperLimit - _MassLowerLimit);
            break;
        }
        default:
            break;
        }
    }

    switch (_AgeDistribution) {
    case GenDistribution::kFromPdf: {
        float MaxProbability = 2.6f;
        if (!(_AgeLowerLimit < _UniverseAge - 1.38e10f + 8e9f && _AgeUpperLimit > _UniverseAge - 1.38e10f + 8e9f)) {
            if (_AgeLowerLimit > _UniverseAge - 1.38e10f + 8e9f) {
                MaxProbability = DefaultAgePdf(_AgeLowerLimit, _UniverseAge / 1e9f);
            } else if (_AgeUpperLimit < _UniverseAge - 1.38e10f + 8e9f) {
                MaxProbability = DefaultAgePdf(_AgeUpperLimit, _UniverseAge / 1e9f);
            }
        }
        Properties.Age = GenerateAge(MaxProbability);
        break;
    }
    case GenDistribution::kUniform: {
        Properties.Age = _AgeLowerLimit + _CommonGenerator(_RandomEngine) * (_AgeUpperLimit - _AgeLowerLimit);
        break;
    }
    case GenDistribution::kUniformByExponent: {
        float Random = _CommonGenerator(_RandomEngine);
        float LogAgeLower = std::log10(_AgeLowerLimit);
        float LogAgeUpper = std::log10(_AgeUpperLimit);
        Properties.Age = std::pow(10.0f, LogAgeLower + Random * (LogAgeUpper - LogAgeLower));
        break;
    }
    default:
        break;
    }

    Distribution<>* FeHGenerator = nullptr;

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

    float FeH = 0.0f;
    do {
        FeH = FeHGenerator->Generate(_RandomEngine);
    } while (FeH > FeHUpperLimit || FeH < FeHLowerLimit);

    if (Properties.Age > _UniverseAge - 1.38e10 + 8e9) {
        FeH *= -1.0f; // 把对数分布反过来
    }

    Properties.FeH    = FeH;
    Properties.Option = _Option;

    return Properties;
}

Astro::Star StellarGenerator::GenerateStar() {
    BasicProperties BasicData = GenBasicProperties();
    return GenerateStar(std::move(BasicData));
}

Astro::Star StellarGenerator::GenerateStar(BasicProperties& Properties) {
    return GenerateStar(std::move(Properties));
}

Astro::Star StellarGenerator::GenerateStar(BasicProperties&& Properties) {
    Astro::Star Star(Properties);
    std::vector<double> StarData;

    switch (Properties.Option) {
    case GenOption::kNormal: {
        try {
            StarData = GetActuallyMistData(Properties, false, true);
        } catch (Astro::Star& DeathStar) {
            DeathStar.SetAge(Properties.Age);
            DeathStar.SetFeH(Properties.FeH);
            DeathStar.SetInitialMass(Properties.InitialMass);
            ProcessDeathStar(DeathStar);
            if (DeathStar.GetEvolutionPhase() == Astro::Star::Phase::kNull) {
                DeathStar = GenerateStar();
            }

            return DeathStar;
        }

        break;
    }
    case GenOption::kGiant: {
        Properties.Age = -1.0f; // 使用 -1.0，在计算年龄的时候根据寿命赋值一个濒死年龄
        try {
            StarData = GetActuallyMistData(Properties, false, true);
        } catch (Astro::Star&) {
            return GenerateStar(); // 生成的东西超过了寿命，递归再生成一个新的
        }

        break;
    }
    case GenOption::kDeathStar: {
        ProcessDeathStar(Star);
        if (Star.GetEvolutionPhase() == Astro::Star::Phase::kNull) { // 炸没了，再生成个新的
            Star = GenerateStar();
        }

        return Star;

        break;
    }
    case GenOption::kMergeStar: {
        ProcessDeathStar(Star, 1.0);
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

    Astro::Star::Phase EvolutionPhase = static_cast<Astro::Star::Phase>(StarData[_kPhaseIndex]);

    Star.SetInitialMass(Star.GetInitialMass() * kSolarMass);
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
    Star.SetStellarWindMassLossRate(-(MassLossRate * kSolarMass / kYearInSeconds));
    Star.SetEvolutionProgress(EvolutionProgress);
    Star.SetEvolutionPhase(EvolutionPhase);
    Star.SetNormal(glm::vec2(Theta, Phi));

    CalcSpectralType(Star, static_cast<float>(StarData.back()));
    GenerateMagnetic(Star);
    GenerateSpin(Star);

    double Mass          = Star.GetMass();
    double Luminosity    = Star.GetLuminosity();
    float  Radius        = Star.GetRadius();
    float  MagneticField = Star.GetMagneticField();

    float MinCoilMass = static_cast<float>(std::max(
        6.6156e14  * std::pow(MagneticField, 2.0f) * std::pow(Luminosity, 1.5) * std::pow(_CoilTempLimit, -6.0f) * std::pow(_dEpdM, -1.0f),
        2.34865e29 * std::pow(MagneticField, 2.0f) * std::pow(Luminosity, 2.0) * std::pow(_CoilTempLimit, -8.0f) * std::pow(Mass,   -1.0)
    ));

    Star.SetMinCoilMass(MinCoilMass);

    return Star;
}

void StellarGenerator::InitMistData() {
    if (_kbMistDataInitiated) {
        return;
    }

    const std::array<std::string, 10> kPresetPrefix{
        Assets::GetAssetFilepath(Assets::AssetType::kModel, "MIST/[Fe_H]=-4.0"),
        Assets::GetAssetFilepath(Assets::AssetType::kModel, "MIST/[Fe_H]=-3.0"),
        Assets::GetAssetFilepath(Assets::AssetType::kModel, "MIST/[Fe_H]=-2.0"),
        Assets::GetAssetFilepath(Assets::AssetType::kModel, "MIST/[Fe_H]=-1.5"),
        Assets::GetAssetFilepath(Assets::AssetType::kModel, "MIST/[Fe_H]=-1.0"),
        Assets::GetAssetFilepath(Assets::AssetType::kModel, "MIST/[Fe_H]=-0.5"),
        Assets::GetAssetFilepath(Assets::AssetType::kModel, "MIST/[Fe_H]=+0.0"),
        Assets::GetAssetFilepath(Assets::AssetType::kModel, "MIST/[Fe_H]=+0.5"),
        Assets::GetAssetFilepath(Assets::AssetType::kModel, "MIST/WhiteDwarfs/Thin"),
        Assets::GetAssetFilepath(Assets::AssetType::kModel, "MIST/WhiteDwarfs/Thick")
    };

    std::vector<float> Masses;

    for (const auto& PrefixDir : kPresetPrefix) {
        for (const auto& Entry : std::filesystem::directory_iterator(PrefixDir)) {
            std::string Filename = Entry.path().filename().string();
            float Mass = std::stof(Filename.substr(0, Filename.find("Ms_track.csv")));
            Masses.emplace_back(Mass);

            if (PrefixDir.find("WhiteDwarfs") != std::string::npos) {
                LoadCsvAsset<WdMistData>(PrefixDir + "/" + Filename, _kWdMistHeaders);
            } else {
                LoadCsvAsset<MistData>(PrefixDir + "/" + Filename, _kMistHeaders);
            }
        }

        _kMassFileCache.emplace(PrefixDir, Masses);
        Masses.clear();
    }

    _kbMistDataInitiated = true;
}

float StellarGenerator::GenerateAge(float MaxPdf) {
    float Age = 0.0f;
    float Probability = 0.0f;
    do {
        Age = _AgeGenerator(_RandomEngine);
        Probability = DefaultAgePdf(Age / 1e9f, _UniverseAge / 1e9f);
    } while (_CommonGenerator(_RandomEngine) * MaxPdf > Probability);

    return Age;
}

float StellarGenerator::GenerateMass(float MaxPdf, bool bIsBinary) {
    float LogMass = 0.0f;
    float Probability = 0.0f;
    do {
        LogMass = _LogMassGenerator(_RandomEngine);
        Probability = DefaultLogMassPdf(LogMass, bIsBinary);
    } while (_CommonGenerator(_RandomEngine) * MaxPdf > Probability);

    return std::pow(10.0f, LogMass);
}

std::vector<double> StellarGenerator::GetActuallyMistData(const BasicProperties& Properties, bool bIsWhiteDwarf, bool bIsSingleWd) {
    float TargetAge = Properties.Age;
    float TargetFeH = Properties.FeH;
    float TargetMass = Properties.InitialMass;

    std::string PrefixDir;
    std::string MassStr;
    std::stringstream MassStream;
    std::pair<std::string, std::string> Files;

    if (!bIsWhiteDwarf) {
        const std::array<float, 8> kPresetFeH{ -4.0f, -3.0f, -2.0f, -1.5f, -1.0f, -0.5f, 0.0f, 0.5f };

        float ClosestFeH = *std::min_element(kPresetFeH.begin(), kPresetFeH.end(), [TargetFeH](float Lhs, float Rhs) -> bool {
            return std::abs(Lhs - TargetFeH) < std::abs(Rhs - TargetFeH);
        });

        TargetFeH = ClosestFeH;

        MassStream << std::fixed << std::setfill('0') << std::setw(6) << std::setprecision(2) << TargetMass;
        MassStr = MassStream.str() + "0";

        std::stringstream FeHStream;
        FeHStream << std::fixed << std::setprecision(1) << TargetFeH;
        PrefixDir = FeHStream.str();
        if (TargetFeH >= 0.0f) {
            PrefixDir.insert(PrefixDir.begin(), '+');
        }
        PrefixDir.insert(0, Assets::GetAssetFilepath(Assets::AssetType::kModel, "MIST/[Fe_H]="));
    } else {
        if (bIsSingleWd) {
            PrefixDir = Assets::GetAssetFilepath(Assets::AssetType::kModel, "MIST/WhiteDwarfs/Thin");
        } else {
            PrefixDir = Assets::GetAssetFilepath(Assets::AssetType::kModel, "MIST/WhiteDwarfs/Thick");
        }
    }

    std::vector<float> Masses;
    {
        std::shared_lock Lock(_kCacheMutex);
        Masses = _kMassFileCache[PrefixDir];
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

    MassStr.clear();
    MassStream.str("");
    MassStream.clear();

    MassStream << std::fixed << std::setfill('0') << std::setw(6) << std::setprecision(2) << LowerMass;
    MassStr = MassStream.str() + "0";
    std::string LowerMassFile = PrefixDir + "/" + MassStr + "Ms_track.csv";

    MassStr.clear();
    MassStream.str("");
    MassStream.clear();

    MassStream << std::fixed << std::setfill('0') << std::setw(6) << std::setprecision(2) << UpperMass;
    MassStr = MassStream.str() + "0";
    std::string UpperMassFile = PrefixDir + "/" + MassStr + "Ms_track.csv";

    Files.first = LowerMassFile;
    Files.second = UpperMassFile;

    std::vector<double> Result = InterpolateMistData(Files, TargetAge, TargetMass, MassCoefficient);
    Result.emplace_back(TargetFeH);

    return Result;
}

std::vector<double> StellarGenerator::InterpolateMistData(const std::pair<std::string, std::string>& Files, double TargetAge, double TargetMass, double MassCoefficient) {
    std::vector<double> Result;

    if (Files.first.find("WhiteDwarfs") == std::string::npos) {
        if (Files.first != Files.second) [[likely]] {
            std::shared_ptr<MistData> LowerData = LoadCsvAsset<MistData>(Files.first, _kMistHeaders);
            std::shared_ptr<MistData> UpperData = LoadCsvAsset<MistData>(Files.second, _kMistHeaders);

            auto LowerPhaseChanges = FindPhaseChanges(LowerData);
            auto UpperPhaseChanges = FindPhaseChanges(UpperData);

            while (TargetAge == -1.0) { // 年龄为 -1.0 代表要生成濒死恒星
                double LowerLifetime = LowerPhaseChanges.back()[_kStarAgeIndex];
                double UpperLifetime = UpperPhaseChanges.back()[_kStarAgeIndex];
                double Lifetime = LowerLifetime + (UpperLifetime - LowerLifetime) * MassCoefficient;
                TargetAge = Lifetime - 500000;
            }

            std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>> PhaseChangePair{ LowerPhaseChanges, UpperPhaseChanges };
            double EvolutionProgress = CalcEvolutionProgress(PhaseChangePair, TargetAge, MassCoefficient);

            double LowerLifetime = PhaseChangePair.first.back()[_kStarAgeIndex];
            double UpperLifetime = PhaseChangePair.second.back()[_kStarAgeIndex];

            std::vector<double> LowerRows = InterpolateRows(LowerData, EvolutionProgress);
            std::vector<double> UpperRows = InterpolateRows(UpperData, EvolutionProgress);

            LowerRows.emplace_back(LowerLifetime);
            UpperRows.emplace_back(UpperLifetime);

            Result = InterpolateFinalData({ LowerRows, UpperRows }, MassCoefficient, false);
        } else [[unlikely]] {
            std::shared_ptr<MistData> StarData = LoadCsvAsset<MistData>(Files.first, _kMistHeaders);
            auto   PhaseChanges = FindPhaseChanges(StarData);
            double EvolutionProgress = 0.0;
            double Lifetime = 0.0;
            if (TargetMass >= 0.1) {
                std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>> PhaseChangePair{ PhaseChanges, {} };
                EvolutionProgress = CalcEvolutionProgress(PhaseChangePair, TargetAge, MassCoefficient);
                Lifetime = PhaseChanges.back()[_kStarAgeIndex];
                Result = InterpolateRows(StarData, EvolutionProgress);
                Result.emplace_back(Lifetime);
            } else {
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

                Result = InterpolateRows(StarData, EvolutionProgress);
                Result.emplace_back(Lifetime);
                ExpandMistData(Result, TargetMass);
            }
        }
    } else {
        if (Files.first != Files.second) [[likely]] {
            std::shared_ptr<WdMistData> LowerData = LoadCsvAsset<WdMistData>(Files.first, _kWdMistHeaders);
            std::shared_ptr<WdMistData> UpperData = LoadCsvAsset<WdMistData>(Files.second, _kWdMistHeaders);

            std::vector<double> LowerRows = InterpolateRows(LowerData, TargetAge);
            std::vector<double> UpperRows = InterpolateRows(UpperData, TargetAge);

            Result = InterpolateFinalData({ LowerRows, UpperRows }, MassCoefficient, true);
        } else [[unlikely]] {
            std::shared_ptr<WdMistData> StarData = LoadCsvAsset<WdMistData>(Files.first, _kWdMistHeaders);
            Result = InterpolateRows(StarData, TargetAge);
        }
    }

    return Result;
}

std::vector<std::vector<double>> StellarGenerator::FindPhaseChanges(const std::shared_ptr<MistData>& DataCsv) {
    std::vector<std::vector<double>> Result;

    _kCacheMutex.lock();
    if (_kPhaseChangesCache.contains(DataCsv)) {
        Result = _kPhaseChangesCache[DataCsv];
    } else {
        _kCacheMutex.unlock();
        auto* DataArray = DataCsv->Data();
        int CurrentPhase = -2;
        for (const auto& Row : *DataArray) {
            if (Row[_kPhaseIndex] != CurrentPhase || Row[_kXIndex] == 10.0) {
                CurrentPhase = static_cast<int>(Row[_kPhaseIndex]);
                Result.emplace_back(Row);
            }
        }

        _kCacheMutex.lock();
        if (!_kPhaseChangesCache.contains(DataCsv)) {
            _kPhaseChangesCache.emplace(DataCsv, Result);
        }
    }
    _kCacheMutex.unlock();

    return Result;
}

void StellarGenerator::CalcSpectralType(Astro::Star& StarData, float FeH) {
    float Teff = StarData.GetTeff();
    auto EvolutionPhase = StarData.GetEvolutionPhase();

    StellarClass::StarType StarType = StarData.GetStellarClass().GetStarType();
    StellarClass::SpectralType SpectralType{};
    SpectralType.bIsAmStar = false;

    std::vector<std::pair<int, int>> SpectralSubclassMap;
    float Subclass = 0.0f;

    float SurfaceH1 = StarData.GetSurfaceH1();
    float MinSurfaceH1 = Astro::Star::_kFeHSurfaceH1Map.at(FeH) - 0.01f;

    std::function<void(Astro::Star::Phase, float)> CalcSpectralSubclass = [&](Astro::Star::Phase Phase, float SurfaceH1) -> void {
        std::uint32_t SpectralClass = Phase == Astro::Star::Phase::kWolfRayet ? 11 : 0;

        if (Phase != Astro::Star::Phase::kWolfRayet) {
            if (Phase == Astro::Star::Phase::kMainSequence) {
                if (SurfaceH1 < 0.5f) { // 如果表面氢质量分数低于 0.5 并且还是主序星阶段，转为 WR 星
                    EvolutionPhase = Astro::Star::Phase::kWolfRayet;
                    CalcSpectralSubclass(EvolutionPhase, SurfaceH1);
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
                SpectralType.HSpectralClass = StellarClass::SpectralClass::kSpectral_WO;
                SpectralType.Subclass = 2.0f;
                return;
            } else {
                if (SurfaceH1 >= 0.2f) { // 根据表面氢质量分数来判断处于的 WR 阶段
                    SpectralSubclassMap = Astro::Star::_kSpectralSubclassMap_WNxh;
                    SpectralClass = 13;
                    SpectralType.SpecialMark = std::to_underlying(StellarClass::SpecialPeculiarities::kCode_h);
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

        SpectralType.HSpectralClass = static_cast<StellarClass::SpectralClass>(SpectralClass);

        if (SpectralSubclassMap.empty()) {
            std::println("Age: {}, FeH: {}, Mass: {}, Teff: {}", StarData.GetAge(), StarData.GetFeH(), StarData.GetMass() / kSolarMass, StarData.GetTeff());
        }

        for (auto it = SpectralSubclassMap.begin(); it != SpectralSubclassMap.end() - 1; ++it) {
            if (it->first >= Teff && (it + 1)->first < Teff) {
                Subclass = static_cast<float>(it->second);
                break;
            }
        }

        if (SpectralType.HSpectralClass == StellarClass::SpectralClass::kSpectral_WN &&
            SpectralType.SpecialMark & std::to_underlying(StellarClass::SpecialPeculiarities::kCode_h)) {
            if (Subclass < 5) {
                Subclass = 5.0f;
            }
        }

        if (SpectralType.HSpectralClass == StellarClass::SpectralClass::kSpectral_WO) {
            if (Subclass > 4) {
                Subclass = 4.0f;
            }
        }

        SpectralType.Subclass = Subclass;
    };

    if (EvolutionPhase != Astro::Star::Phase::kWolfRayet) {
        switch (StarType) {
        case StellarClass::StarType::kNormalStar: {
            if (Teff < 54000) { // 高于 O0 上限，转到通过表面氢质量分数判断阶段
                CalcSpectralSubclass(EvolutionPhase, SurfaceH1);

                if (EvolutionPhase != Astro::Star::Phase::kWolfRayet) {
                    if (EvolutionPhase == Astro::Star::Phase::kPrevMainSequence) {
                        SpectralType.LuminosityClass = CalcLuminosityClass(StarData);
                    } else if (EvolutionPhase == Astro::Star::Phase::kMainSequence) {
                        if (SpectralType.HSpectralClass == StellarClass::SpectralClass::kSpectral_O && SurfaceH1 < MinSurfaceH1) {
                            SpectralType.LuminosityClass = CalcLuminosityClass(StarData);
                        } else {
                            SpectralType.LuminosityClass = StellarClass::LuminosityClass::kLuminosity_V;
                        }
                    } else {
                        SpectralType.LuminosityClass = CalcLuminosityClass(StarData);
                    }
                } else {
                    SpectralType.LuminosityClass = StellarClass::LuminosityClass::kLuminosity_Unknown;
                }
            } else {
                if (SurfaceH1 > MinSurfaceH1) {
                    SpectralType.HSpectralClass = StellarClass::SpectralClass::kSpectral_O;
                    SpectralType.Subclass = 2.0f;
                    SpectralType.LuminosityClass = StellarClass::LuminosityClass::kLuminosity_V;
                } else if (SurfaceH1 > 0.5f) {
                    SpectralType.HSpectralClass = StellarClass::SpectralClass::kSpectral_O;
                    SpectralType.Subclass = 2.0f;
                    SpectralType.LuminosityClass = CalcLuminosityClass(StarData);
                } else {
                    CalcSpectralSubclass(Astro::Star::Phase::kWolfRayet, SurfaceH1);
                }
            }

            break;
        }
        case StellarClass::StarType::kWhiteDwarf: {
            double MassSol = StarData.GetMass() / kSolarMass;

            if (Teff >= 12000) {
                if (MassSol <= 0.5) {
                    SpectralType.HSpectralClass = StellarClass::SpectralClass::kSpectral_DA;
                } else {
                    if (Teff > 45000) {
                        SpectralType.HSpectralClass = StellarClass::SpectralClass::kSpectral_DO;
                    } else {
                        SpectralType.HSpectralClass = StellarClass::SpectralClass::kSpectral_DB;
                    }
                }
            } else {
                SpectralType.HSpectralClass = StellarClass::SpectralClass::kSpectral_DC;
            }

            Subclass = 50400.0f / Teff;
            if (Subclass > 9.5f) {
                Subclass = 9.5f;
            }

            SpectralType.Subclass = std::round(Subclass * 2.0f) / 2.0f;

            break;
        }
        case StellarClass::StarType::kNeutronStar: {
            SpectralType.HSpectralClass = StellarClass::SpectralClass::kSpectral_Q;
            break;
        }
        case StellarClass::StarType::kBlackHole: {
            SpectralType.HSpectralClass = StellarClass::SpectralClass::kSpectral_X;
            break;
        }
        case StellarClass::StarType::kDeathStarPlaceholder: {
            SpectralType.HSpectralClass = StellarClass::SpectralClass::kSpectral_Unknown;
            break;
        }
        default: {
            SpectralType.HSpectralClass = StellarClass::SpectralClass::kSpectral_Unknown;
            break;
        }
        }
    } else {
        CalcSpectralSubclass(Astro::Star::Phase::kWolfRayet, SurfaceH1);
        SpectralType.LuminosityClass = StellarClass::LuminosityClass::kLuminosity_Unknown;
    }

    StellarClass Class(StarType, SpectralType);
    StarData.SetStellarClass(Class);
}

StellarClass::LuminosityClass StellarGenerator::CalcLuminosityClass(const Astro::Star& StarData) {
    float MassLossRateSolPerYear = StarData.GetStellarWindMassLossRate() * kYearInSeconds / kSolarMass;
    double MassSol = StarData.GetMass() / kSolarMass;
    StellarClass::LuminosityClass LuminosityClass = StellarClass::LuminosityClass::kLuminosity_Unknown;

    double LuminositySol = StarData.GetLuminosity() / kSolarLuminosity;
    if (LuminositySol > 650000) { // 光度高于 650000 Lsun
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_0;
    }

    if (MassLossRateSolPerYear > 1e-4f && MassSol >= 15) { // 表面物质流失率大于 1e-4 Msun/yr 并且质量大于等于 15 Msun
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_IaPlus;
    }

    if (LuminosityClass != StellarClass::LuminosityClass::kLuminosity_Unknown) { // 如果判断为特超巨星，直接返回
        return LuminosityClass;
    }

    std::shared_ptr<HrDiagram> HrDiagramData = LoadCsvAsset<HrDiagram>(Assets::GetAssetFilepath(Assets::AssetType::kModel, "H-R Diagram/H-R Diagram.csv"), _kHrDiagramHeaders);

    float Teff = StarData.GetTeff();
    float BvColorIndex = 0.0f;
    if (std::log10(Teff) < 3.691f) {
        BvColorIndex = -3.684f * std::log10(Teff) + 14.551f;
    } else {
        BvColorIndex = 0.344f * std::pow(std::log10(Teff), 2.0f) - 3.402f * std::log10(Teff) + 8.037f;
    }

    if (BvColorIndex < -0.3f || BvColorIndex > 1.9727273f) { // 超过 HR 表的范围，使用光度判断
        if (LuminositySol > 100000) {
            return StellarClass::LuminosityClass::kLuminosity_Ia;
        } else if (LuminositySol > 50000) {
            return StellarClass::LuminosityClass::kLuminosity_Iab;
        } else if (LuminositySol > 10000) {
            return StellarClass::LuminosityClass::kLuminosity_Ib;
        } else if (LuminositySol > 1000) {
            return StellarClass::LuminosityClass::kLuminosity_II;
        } else if (LuminositySol > 100) {
            return StellarClass::LuminosityClass::kLuminosity_III;
        } else if (LuminositySol > 10) {
            return StellarClass::LuminosityClass::kLuminosity_IV;
        } else if (LuminositySol > 0.05) {
            return StellarClass::LuminosityClass::kLuminosity_V;
        } else {
            return StellarClass::LuminosityClass::kLuminosity_VI;
        }
    }

    std::vector<double> LuminosityData = InterpolateRows(HrDiagramData, BvColorIndex);
    if (LuminositySol > LuminosityData[1]) {
        return StellarClass::LuminosityClass::kLuminosity_Ia;
    }

    double ClosestValue = *std::min_element(LuminosityData.begin() + 1, LuminosityData.end(), [LuminositySol](double Lhs, double Rhs) -> bool {
        return std::abs(Lhs - LuminositySol) < std::abs(Rhs - LuminositySol);
    });

    while (LuminosityData.size() < 7) {
        LuminosityData.emplace_back(-1);
    }

    if (LuminositySol <= LuminosityData[1] && LuminositySol >= LuminosityData[2] &&
        (ClosestValue == LuminosityData[1] || ClosestValue == LuminosityData[2])) {
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_Iab;
    } else {
        if (ClosestValue == LuminosityData[2]) {
            LuminosityClass = StellarClass::LuminosityClass::kLuminosity_Ib;
        } else if (ClosestValue == LuminosityData[3]) {
            LuminosityClass = StellarClass::LuminosityClass::kLuminosity_II;
        } else if (ClosestValue == LuminosityData[4]) {
            LuminosityClass = StellarClass::LuminosityClass::kLuminosity_III;
        } else if (ClosestValue == LuminosityData[5]) {
            LuminosityClass = StellarClass::LuminosityClass::kLuminosity_IV;
        } else if (ClosestValue == LuminosityData[6]) {
            LuminosityClass = StellarClass::LuminosityClass::kLuminosity_V;
        }
    }

    return LuminosityClass;
}

void StellarGenerator::ProcessDeathStar(Astro::Star& DeathStar, double MergeStarProbability) {
    float InputAge  = DeathStar.GetAge();
    float InputFeH  = DeathStar.GetFeH();
    float InputMass = DeathStar.GetInitialMass();

    Astro::Star::Phase         EvolutionPhase{};
    Astro::Star::Death         EvolutionEnding{};
    StellarClass::StarType     DeathStarType{};
    StellarClass::SpectralType DeathStarClass{};

    float DeathStarAge  = InputAge - static_cast<float>(DeathStar.GetLifetime());
    float DeathStarMass = 0.0f;

    if (InputFeH <= -2.0f && InputMass >= 140 && InputMass < 250) {
        EvolutionPhase  = Astro::Star::Phase::kNull;
        EvolutionEnding = Astro::Star::Death::kPairInstabilitySupernova;
        DeathStarType   = StellarClass::StarType::kDeathStarPlaceholder;
        DeathStarClass  = { StellarClass::SpectralClass::kSpectral_Unknown, StellarClass::SpectralClass::kSpectral_Unknown, StellarClass::LuminosityClass::kLuminosity_Unknown, 0, 0.0f, 0.0f, false };
    } else if (InputFeH <= -2.0f && InputMass >= 250) {
        EvolutionPhase  = Astro::Star::Phase::kStellarBlackHole;
        EvolutionEnding = Astro::Star::Death::kPhotondisintegration;
        DeathStarType   = StellarClass::StarType::kBlackHole;
        DeathStarClass  = { StellarClass::SpectralClass::kSpectral_X, StellarClass::SpectralClass::kSpectral_Unknown, StellarClass::LuminosityClass::kLuminosity_Unknown, 0, 0.0f, 0.0f, false };
        DeathStarMass   = 0.5f * InputMass;
    } else {
        if (InputMass > -0.75f && InputMass < 0.8f) {
            DeathStarMass = (0.9795f - 0.393f * InputMass) * InputMass;
        } else if (InputMass >= 0.8f && InputMass < 7.9f) {
            DeathStarMass = -0.00012336f * std::pow(InputMass, 6.0f) + 0.003160f * std::pow(InputMass, 5.0f) - 0.02960f * std::pow(InputMass, 4.0f) + 0.12350f * std::pow(InputMass, 3.0f) - 0.21550f * std::pow(InputMass, 2.0f) + 0.19022f * InputMass + 0.46575f;
        } else if (InputMass >= 7.9f && InputMass < 10.0f) {
            DeathStarMass = 1.301f + 0.008095f * InputMass;
        } else if (InputMass >= 10.0f && InputMass < 21.0f) {
            DeathStarMass = 1.246f + 0.0136f * InputMass;
        } else if (InputMass >= 21.0f && InputMass < 23.3537f) {
            DeathStarMass = std::pow(10.0f, (1.334f - 0.009987f * InputMass));
        } else if (InputMass >= 23.3537f && InputMass < 33.75f) {
            DeathStarMass = 12.1f - 0.763f * InputMass + 0.0137f * std::pow(InputMass, 2.0f);
        } else if (InputMass >= 33.75f && InputMass < 40.0f) {
            DeathStarMass = std::pow(10.0f, (0.882f + 0.0105f * InputMass));
        } else {
            DeathStarMass = 0.5f * InputMass;
        }

        if (InputMass >= 0.075f && InputMass < 0.5f) {
            EvolutionPhase  = Astro::Star::Phase::kHeliumWhiteDwarf;
            EvolutionEnding = Astro::Star::Death::kSlowColdingDown;
            DeathStarType   = StellarClass::StarType::kWhiteDwarf;
            DeathStarClass  = { StellarClass::SpectralClass::kSpectral_Unknown, StellarClass::SpectralClass::kSpectral_Unknown, StellarClass::LuminosityClass::kLuminosity_Unknown, 0, 0.0f, 0.0f, false };
        } else if (InputMass >= 0.5f && InputMass < 8.0f) {
            EvolutionPhase  = Astro::Star::Phase::kCarbonOxygenWhiteDwarf;
            EvolutionEnding = Astro::Star::Death::kEnvelopeDisperse;
            DeathStarType   = StellarClass::StarType::kWhiteDwarf;
            DeathStarClass  = { StellarClass::SpectralClass::kSpectral_Unknown, StellarClass::SpectralClass::kSpectral_Unknown, StellarClass::LuminosityClass::kLuminosity_Unknown, 0, 0.0f, 0.0f, false };
        } else if (InputMass >= 8.0f && InputMass < 9.759f) {
            EvolutionPhase  = Astro::Star::Phase::kOxygenNeonMagnWhiteDwarf;
            EvolutionEnding = Astro::Star::Death::kEnvelopeDisperse;
            DeathStarType   = StellarClass::StarType::kWhiteDwarf;
            DeathStarClass  = { StellarClass::SpectralClass::kSpectral_Unknown, StellarClass::SpectralClass::kSpectral_Unknown, StellarClass::LuminosityClass::kLuminosity_Unknown, 0, 0.0f, 0.0f, false };
        } else if (InputMass >= 9.759f && InputMass < 10.0f) {
            EvolutionPhase  = Astro::Star::Phase::kNeutronStar;
            EvolutionEnding = Astro::Star::Death::kElectronCaptureSupernova;
            DeathStarType   = StellarClass::StarType::kNeutronStar;
            DeathStarClass  = { StellarClass::SpectralClass::kSpectral_Q, StellarClass::SpectralClass::kSpectral_Unknown, StellarClass::LuminosityClass::kLuminosity_Unknown, 0, 0.0f, 0.0f, false };
        } else if (InputMass >= 10.0f && InputMass < 21.0f) {
            EvolutionPhase  = Astro::Star::Phase::kNeutronStar;
            EvolutionEnding = Astro::Star::Death::kIronCoreCollapseSupernova;
            DeathStarType   = StellarClass::StarType::kNeutronStar;
            DeathStarClass  = { StellarClass::SpectralClass::kSpectral_Q, StellarClass::SpectralClass::kSpectral_Unknown, StellarClass::LuminosityClass::kLuminosity_Unknown, 0, 0.0f, 0.0f, false };
        } else if (InputMass >= 21.0f && InputMass < 23.3537f) {
            EvolutionPhase  = Astro::Star::Phase::kStellarBlackHole;
            EvolutionEnding = Astro::Star::Death::kIronCoreCollapseSupernova;
            DeathStarType   = StellarClass::StarType::kBlackHole;
            DeathStarClass  = { StellarClass::SpectralClass::kSpectral_X, StellarClass::SpectralClass::kSpectral_Unknown, StellarClass::LuminosityClass::kLuminosity_Unknown, 0, 0.0f, 0.0f, false };
        } else if (InputMass >= 23.3537f && InputMass < 33.75f) {
            EvolutionPhase  = Astro::Star::Phase::kNeutronStar;
            EvolutionEnding = Astro::Star::Death::kIronCoreCollapseSupernova;
            DeathStarType   = StellarClass::StarType::kNeutronStar;
            DeathStarClass  = { StellarClass::SpectralClass::kSpectral_Q, StellarClass::SpectralClass::kSpectral_Unknown, StellarClass::LuminosityClass::kLuminosity_Unknown, 0, 0.0f, 0.0f, false };
        } else {
            EvolutionPhase  = Astro::Star::Phase::kStellarBlackHole;
            EvolutionEnding = Astro::Star::Death::kRelativisticJetHypernova;
            DeathStarType   = StellarClass::StarType::kBlackHole;
            DeathStarClass  = { StellarClass::SpectralClass::kSpectral_X, StellarClass::SpectralClass::kSpectral_Unknown, StellarClass::LuminosityClass::kLuminosity_Unknown, 0, 0.0f, 0.0f, false };
        }
    }

    if (DeathStarType == StellarClass::StarType::kNeutronStar || MergeStarProbability == 1.0) {
        BernoulliDistribution MergeProbability(MergeStarProbability);
        if (MergeProbability(_RandomEngine)) {
            EvolutionEnding = Astro::Star::Death::kWhiteDwarfMerge;
            BernoulliDistribution BlackHoleProbability(0.114514);
            float MassSol = 0.0f;
            if (BlackHoleProbability(_RandomEngine)) {
                UniformRealDistribution<float> MassDist(2.6f, 2.76f);
                MassSol = MassDist(_RandomEngine);
                EvolutionPhase = Astro::Star::Phase::kStellarBlackHole;
                DeathStarType = StellarClass::StarType::kBlackHole;
                DeathStarClass = { StellarClass::SpectralClass::kSpectral_X, StellarClass::SpectralClass::kSpectral_Unknown, StellarClass::LuminosityClass::kLuminosity_Unknown, 0, 0.0f, 0.0f, false };
            } else {
                UniformRealDistribution<float> MassDist(1.38f, 2.18072f);
                MassSol = MassDist(_RandomEngine);
                EvolutionPhase = Astro::Star::Phase::kNeutronStar;
                DeathStarType = StellarClass::StarType::kNeutronStar;
                DeathStarClass = { StellarClass::SpectralClass::kSpectral_Q, StellarClass::SpectralClass::kSpectral_Unknown, StellarClass::LuminosityClass::kLuminosity_Unknown, 0, 0.0f, 0.0f, false };
            }

            DeathStarMass = MassSol;
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
    case StellarClass::StarType::kWhiteDwarf: {
        std::vector<double> WhiteDwarfData = GetActuallyMistData({ DeathStarAge, 0.0f, DeathStarMass }, true, true);

        StarAge      = static_cast<float>(WhiteDwarfData[_kWdStarAgeIndex]);
        LogR         = static_cast<float>(WhiteDwarfData[_kWdLogRIndex]);
        LogTeff      = static_cast<float>(WhiteDwarfData[_kWdLogTeffIndex]);
        LogCenterT   = static_cast<float>(WhiteDwarfData[_kWdLogCenterTIndex]);
        LogCenterRho = static_cast<float>(WhiteDwarfData[_kWdLogCenterRhoIndex]);

        if (DeathStarMass < 0.2 || DeathStarMass > 1.3) {
            LogR         = std::log10(0.0323f - 0.021384f * DeathStarMass);
            LogCenterT   = std::numeric_limits<float>::min();
            LogCenterRho = std::numeric_limits<float>::min();
        }

        if (DeathStarAge > StarAge) {
            float T1   = std::pow(10.0f, LogTeff);
            LogTeff    = std::log10(T1 * std::pow((20.0f * StarAge) / (DeathStarAge + 19.0f * StarAge), 7.0f / 4.0f));
            LogCenterT = std::numeric_limits<float>::min();
        }

        SurfaceZ = 0.0f;
        SurfaceEnergeticNuclide = 0.0f;
        SurfaceVolatiles = 1.0f;

        break;
    }
    case StellarClass::StarType::kNeutronStar: {
        if (DeathStarAge < 1e5f) {
            DeathStarAge += 1e5f;
        }

        float Radius = 0.0f;
        if (DeathStarMass <= 0.77711f) {
            Radius = -4.783f + 2.565f / DeathStarMass + 42.0f * DeathStarMass - 55.4f * std::pow(DeathStarMass, 2.0f) + 34.93f * std::pow(DeathStarMass, 3.0f) - 8.4f * std::pow(DeathStarMass, 4.0f);
        } else if (DeathStarMass <= 2.0181f) {
            Radius = 11.302f - 0.35184f * DeathStarMass;
        } else {
            Radius = -31951.1f + 63121.8f * DeathStarMass - 46717.8f * std::pow(DeathStarMass, 2.0f) + 15358.4f * std::pow(DeathStarMass, 3.0f) - 1892.365f * std::pow(DeathStarMass, 4.0f);
        }

        LogR    = std::log10(Radius * 1000 / kSolarRadius);
        LogTeff = std::log10(1.5e8f * std::pow((DeathStarAge - 1e5f) + 22000, -0.5f));

        SurfaceZ                = std::numeric_limits<float>::quiet_NaN();
        SurfaceEnergeticNuclide = std::numeric_limits<float>::quiet_NaN();
        SurfaceVolatiles        = std::numeric_limits<float>::quiet_NaN();

        break;
    }
    case StellarClass::StarType::kBlackHole: {
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
    float  Age               = DeathStarAge;
    float  MassSol           = DeathStarMass;
    float  RadiusSol         = std::pow(10.0f, LogR);
    float  Teff              = std::pow(10.0f, LogTeff);
    float  CoreTemp          = std::pow(10.0f, LogCenterT);
    float  CoreDensity       = std::pow(10.0f, LogCenterRho);

    float LuminositySol  = std::pow(RadiusSol, 2.0f) * std::pow((Teff / kSolarTeff), 4.0f);
    float EscapeVelocity = std::sqrt((2.0f * kGravityConstant * MassSol * kSolarMass) / (RadiusSol * kSolarRadius));

    float Theta = _CommonGenerator(_RandomEngine) * 2.0f * kPi;
    float Phi   = _CommonGenerator(_RandomEngine) * kPi;

    DeathStar.SetInitialMass(InputMass * kSolarMass);
    DeathStar.SetAge(Age);
    DeathStar.SetMass(MassSol * kSolarMass);
    DeathStar.SetLifetime(std::numeric_limits<double>::max());
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
    DeathStar.SetEvolutionEnding(EvolutionEnding);
    DeathStar.SetStellarClass(StellarClass(DeathStarType, DeathStarClass));

    CalcSpectralType(DeathStar, 0.0);
    GenerateMagnetic(DeathStar);
    GenerateSpin(DeathStar);
}

void StellarGenerator::GenerateMagnetic(Astro::Star& StarData) {
    Distribution<>* MagneticGenerator = nullptr;

    StellarClass::StarType StarType = StarData.GetStellarClass().GetStarType();
    float MassSol = static_cast<float>(StarData.GetMass() / kSolarMass);
    Astro::Star::Phase EvolutionPhase = StarData.GetEvolutionPhase();

    float MagneticField = 0.0f;

    switch (StarType) {
    case StellarClass::StarType::kNormalStar: {
        if (MassSol >= 0.075f && MassSol < 0.33f) {
            MagneticGenerator = &_MagneticGenerators[0];
        } else if (MassSol >= 0.33f && MassSol < 0.6f) {
            MagneticGenerator = &_MagneticGenerators[1];
        } else if (MassSol >= 0.6f && MassSol < 1.5f) {
            MagneticGenerator = &_MagneticGenerators[2];
        } else if (MassSol >= 1.5f && MassSol < 20.0f) {
            auto SpectralType = StarData.GetStellarClass().Data();
            if (EvolutionPhase == Astro::Star::Phase::kMainSequence &&
                (SpectralType.HSpectralClass == StellarClass::SpectralClass::kSpectral_A ||
                    SpectralType.HSpectralClass == StellarClass::SpectralClass::kSpectral_B)) {
                BernoulliDistribution ProbabilityGenerator(0.15); //  p 星的概率
                if (ProbabilityGenerator(_RandomEngine)) {
                    MagneticGenerator = &_MagneticGenerators[3];
                    SpectralType.SpecialMark |= std::to_underlying(StellarClass::SpecialPeculiarities::kCode_p);
                    StarData.SetStellarClass(StellarClass(StellarClass::StarType::kNormalStar, SpectralType));
                } else {
                    MagneticGenerator = &_MagneticGenerators[4];
                }
            } else {
                MagneticGenerator = &_MagneticGenerators[4];
            }
        } else {
            MagneticGenerator = &_MagneticGenerators[5];
        }

        MagneticField = std::pow(10.0f, MagneticGenerator->Generate(_RandomEngine)) / 10000;

        break;
    }
    case StellarClass::StarType::kWhiteDwarf: {
        MagneticGenerator = &_MagneticGenerators[6];
        MagneticField = std::pow(10.0f, MagneticGenerator->Generate(_RandomEngine));
        break;
    }
    case StellarClass::StarType::kNeutronStar: {
        MagneticGenerator = &_MagneticGenerators[7];
        float B0 = MagneticGenerator->Generate(_RandomEngine);
        MagneticField = B0 / (std::pow((0.034f * StarData.GetAge() / 1e4f), 1.17f) + 0.84f);
        break;
    }
    case StellarClass::StarType::kBlackHole: {
        MagneticField = 0.0f;
        break;
    }
    case StellarClass::StarType::kDeathStarPlaceholder: {
        break;
    }
    default: {
        break;
    }
    }

    StarData.SetMagneticField(MagneticField);
}

void StellarGenerator::GenerateSpin(Astro::Star& StarData) {
    StellarClass::StarType StarType = StarData.GetStellarClass().GetStarType();
    float StarAge   = StarData.GetAge();
    float MassSol   = static_cast<float>(StarData.GetMass() / kSolarMass);
    float RadiusSol = StarData.GetRadius() / kSolarRadius;
    float Spin      = 0.0f;

    Distribution<float>* SpinGenerator = nullptr;

    switch (StarType) {
    case StellarClass::StarType::kNormalStar: {
        float Base = 1.0f + _CommonGenerator(_RandomEngine);
        if (StarData.GetStellarClass().Data().SpecialMark & std::to_underlying(StellarClass::SpecialPeculiarities::kCode_p)) {
            Base *= 10;
        }

        float LgMass = std::log10(MassSol);
        float Term1  = 0.0f;
        float Term2  = 0.0f;
        float Term3  = std::pow(2.0f, std::sqrt(Base * (StarAge + 1e6f) * 1e-9f));

        if (MassSol <= 1.4f) {
            Term1 = std::pow(10.0f, 30.893f - 25.34303f * std::exp(LgMass) + 21.7577f * LgMass + 7.34205f * std::pow(LgMass, 2.0f) + 0.12951f * std::pow(LgMass, 3.0f));
            Term2 = std::pow(RadiusSol / std::pow(MassSol, 0.9f), 2.5f);
        } else {
            Term1 = std::pow(10.0f, 28.0784f - 22.15753f * std::exp(LgMass) + 12.55134f * LgMass + 30.9045f * std::pow(LgMass, 2.0f) - 10.1479f * std::pow(LgMass, 3.0f) + 4.6894f * std::pow(LgMass, 4.0f));
            Term2 = std::pow(RadiusSol / (1.1062f * std::pow(MassSol, 0.6f)), 2.5f);
        }

        Spin = Term1 * Term2 * Term3;

        break;
    }
    case StellarClass::StarType::kWhiteDwarf: {
        SpinGenerator = &_SpinGenerators[0];
        Spin = std::pow(10.0f, SpinGenerator->Generate(_RandomEngine));
        break;
    }
    case StellarClass::StarType::kNeutronStar: {
        Spin = (StarAge * 3 * 1e-9f) + 1e-3f;
        break;
    }
    case StellarClass::StarType::kBlackHole: { // 此处表示无量纲自旋参数，而非自转时间
        SpinGenerator = &_SpinGenerators[1];
        Spin = SpinGenerator->Generate(_RandomEngine);
        break;
    }
    default: {
        break;
    }
    }

    if (StarType != StellarClass::StarType::kBlackHole) {
        float Oblateness = 4.0f * std::pow(kPi, 2.0f) * std::pow(StarData.GetRadius(), 3.0f);
        Oblateness /= (std::pow(Spin, 2.0f) * kGravityConstant * static_cast<float>(StarData.GetMass()));
        StarData.SetOblateness(Oblateness);
    }

    StarData.SetSpin(Spin);
}

template<typename CsvType>
std::shared_ptr<CsvType> StellarGenerator::LoadCsvAsset(const std::string& Filename, const std::vector<std::string>& Headers) {
    {
        std::shared_lock Lock(_kCacheMutex);
        auto Asset = Assets::AssetManager::GetAsset<CsvType>(Filename);
        if (Asset != nullptr) {
            return Asset;
        }
    }

    std::unique_lock Lock(_kCacheMutex);
    auto CsvAsset = std::make_shared<CsvType>(Filename, Headers);
    Assets::AssetManager::AddAsset<CsvType>(Filename, CsvAsset);

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

const std::vector<std::string> StellarGenerator::_kMistHeaders{ "star_age", "star_mass", "star_mdot", "log_Teff", "log_R", "log_surf_z", "surface_h1", "surface_he3", "log_center_T", "log_center_Rho", "phase", "x" };
const std::vector<std::string> StellarGenerator::_kWdMistHeaders{ "star_age", "log_R", "log_Teff", "log_center_T", "log_center_Rho" };
const std::vector<std::string> StellarGenerator::_kHrDiagramHeaders{ "B-V", "Ia", "Ib", "II", "III", "IV", "V" };
std::unordered_map<std::string, std::vector<float>> StellarGenerator::_kMassFileCache;
std::unordered_map<std::shared_ptr<StellarGenerator::MistData>, std::vector<std::vector<double>>> StellarGenerator::_kPhaseChangesCache;
std::shared_mutex StellarGenerator::_kCacheMutex;
bool StellarGenerator::_kbMistDataInitiated = false;

// Tool functions implementations
// ------------------------------
float DefaultAgePdf(float Fuck, float UniverseAge) {
    float pt = 0.0f;
    if (Fuck - (UniverseAge - 13.8f) < 8.0f) {
        pt = std::exp((Fuck - (UniverseAge - 13.8f) / 8.4f));
    } else {
        pt = 2.6f * std::exp((-0.5f * std::pow((Fuck - (UniverseAge - 13.8f)) - 8.0f, 2.0f)) / (std::pow(1.5f, 2.0f)));
    }

    return static_cast<float>(pt);
}

float DefaultLogMassPdf(float Fuck, bool bIsBinary) {
    float g = 0.0f;
    if (!bIsBinary) {
        if (std::pow(10.0f, Fuck) <= 1.0f) {
            g = 0.158f * std::exp(-1.0f * std::pow(Fuck + 1.0f, 2.0f) / 1.101128f);
        } else {
            float n01 = 0.06371598f;
            g = n01 * std::pow(std::pow(10.0f, Fuck), -0.65f);
        }
    } else {
        if (std::pow(10.0, Fuck) <= 1.0f) {
            g = 0.086f * std::exp(-1.0f * std::pow(Fuck + 0.65757734f, 2.0f) / 1.101128f);
        } else {
            float n02 = 0.058070157f;
            g = n02 * std::pow(std::pow(10.0f, Fuck), -0.65f);
        }
    }

    return g;
}

double CalcEvolutionProgress(std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& PhaseChanges, double TargetAge, double MassCoefficient) {
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
        if (PhaseChanges.first.size() == PhaseChanges.second.size() && (*std::prev(PhaseChanges.first.end(), 2))[StellarGenerator::_kPhaseIndex] == (*std::prev(PhaseChanges.second.end(), 2))[StellarGenerator::_kPhaseIndex]) {
            const auto& TimePointResults = FindSurroundingTimePoints(PhaseChanges, TargetAge, MassCoefficient);

            Phase = TimePointResults.first;
            std::size_t Index = TimePointResults.second;

            if (Index + 1 != PhaseChanges.first.size()) {
                std::pair<double, double> LowerTimePoints = { PhaseChanges.first[Index][StellarGenerator::_kStarAgeIndex], PhaseChanges.first[Index + 1][StellarGenerator::_kStarAgeIndex] };
                std::pair<double, double> UpperTimePoints = { PhaseChanges.second[Index][StellarGenerator::_kStarAgeIndex], PhaseChanges.second[Index + 1][StellarGenerator::_kStarAgeIndex] };

                const auto& [LowerLowerTimePoint, LowerUpperTimePoint] = LowerTimePoints;
                const auto& [UpperLowerTimePoint, UpperUpperTimePoint] = UpperTimePoints;

                double LowerTimePoint = LowerLowerTimePoint + (UpperLowerTimePoint - LowerLowerTimePoint) * MassCoefficient;
                double UpperTimePoint = LowerUpperTimePoint + (UpperUpperTimePoint - LowerUpperTimePoint) * MassCoefficient;

                Result = (TargetAge - LowerTimePoint) / (UpperTimePoint - LowerTimePoint) + Phase;

                if (Result > PhaseChanges.first.back()[StellarGenerator::_kPhaseIndex] + 1) {
                    return 0.0;
                }
            } else {
                Result = 0.0;
            }
        } else {
            if (PhaseChanges.first.back()[StellarGenerator::_kPhaseIndex] == PhaseChanges.second.back()[StellarGenerator::_kPhaseIndex]) {
                double FirstDiscardTimePoint = 0.0;
                double FirstCommonTimePoint = (*std::prev(PhaseChanges.first.end(), 2))[StellarGenerator::_kStarAgeIndex];

                std::size_t MinSize = std::min(PhaseChanges.first.size(), PhaseChanges.second.size());
                for (std::size_t i = 0; i != MinSize - 1; ++i) {
                    if (PhaseChanges.first[i][StellarGenerator::_kPhaseIndex] != PhaseChanges.second[i][StellarGenerator::_kPhaseIndex]) {
                        FirstDiscardTimePoint = PhaseChanges.first[i][StellarGenerator::_kStarAgeIndex];
                        break;
                    }
                }

                double DeltaTimePoint = FirstCommonTimePoint - FirstDiscardTimePoint;
                (*std::prev(PhaseChanges.first.end(), 2))[StellarGenerator::_kStarAgeIndex] -= DeltaTimePoint;
                PhaseChanges.first.back()[StellarGenerator::_kStarAgeIndex] -= DeltaTimePoint;
            }

            AlignArrays(PhaseChanges);

            Result = CalcEvolutionProgress(PhaseChanges, TargetAge, MassCoefficient);
            double IntegerPart = 0.0;
            double FractionalPart = std::modf(Result, &IntegerPart);
            if (PhaseChanges.second.back()[StellarGenerator::_kPhaseIndex] == 9 && FractionalPart > 0.99 && Result < 9.0 && IntegerPart >= (*std::prev(PhaseChanges.first.end(), 3))[StellarGenerator::_kPhaseIndex]) {
                Result = 9.0;
            }
        }
    }

    return Result;
}

std::pair<double, std::pair<double, double>> FindSurroundingTimePoints(const std::vector<std::vector<double>>& PhaseChanges, double TargetAge) {
    std::vector<std::vector<double>>::const_iterator LowerTimePoint;
    std::vector<std::vector<double>>::const_iterator UpperTimePoint;

    if (PhaseChanges.size() != 2 || PhaseChanges.front()[StellarGenerator::_kPhaseIndex] != PhaseChanges.back()[StellarGenerator::_kPhaseIndex]) {
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

    return { (*LowerTimePoint)[StellarGenerator::_kXIndex], { (*LowerTimePoint)[StellarGenerator::_kStarAgeIndex], (*UpperTimePoint)[StellarGenerator::_kStarAgeIndex] } };
}

std::pair<double, std::size_t> FindSurroundingTimePoints(const std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& PhaseChanges, double TargetAge, double MassCoefficient) {
    std::vector<double> LowerPhaseChangeTimePoints;
    std::vector<double> UpperPhaseChangeTimePoints;
    for (std::size_t i = 0; i != PhaseChanges.first.size(); ++i) {
        LowerPhaseChangeTimePoints.emplace_back(PhaseChanges.first[i][StellarGenerator::_kStarAgeIndex]);
        UpperPhaseChangeTimePoints.emplace_back(PhaseChanges.second[i][StellarGenerator::_kStarAgeIndex]);
    }

    std::vector<double> PhaseChangeTimePoints = InterpolateArray({ LowerPhaseChangeTimePoints, UpperPhaseChangeTimePoints }, MassCoefficient);

    if (TargetAge > PhaseChangeTimePoints.back()) {
        double Lifetime = LowerPhaseChangeTimePoints.back() + (UpperPhaseChangeTimePoints.back() - LowerPhaseChangeTimePoints.back()) * MassCoefficient;
        GenerateDeathStarPlaceholder(Lifetime);
    }

    std::vector<std::pair<double, double>> TimePointPairs;
    for (std::size_t i = 0; i != PhaseChanges.first.size(); ++i) {
        TimePointPairs.emplace_back(PhaseChanges.first[i][StellarGenerator::_kPhaseIndex], PhaseChangeTimePoints[i]);
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

void AlignArrays(std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& Arrays) {
    auto TrimArray = [](std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& Arrays) -> void {
        const auto& LastArray1 = Arrays.first.back();
        const auto& LastArray2 = Arrays.second.back();
        const auto& SubLastArray1 = *std::prev(Arrays.first.end(), 2);
        const auto& SubLastArray2 = *std::prev(Arrays.second.end(), 2);

        std::size_t MinSize = std::min(Arrays.first.size(), Arrays.second.size());

        Arrays.first.resize(MinSize - 2);
        Arrays.second.resize(MinSize - 2);
        Arrays.first.emplace_back(SubLastArray1);
        Arrays.first.emplace_back(LastArray1);
        Arrays.second.emplace_back(SubLastArray2);
        Arrays.second.emplace_back(LastArray2);
    };

    if (Arrays.first.back()[StellarGenerator::_kPhaseIndex] != 9 && Arrays.second.back()[StellarGenerator::_kPhaseIndex] != 9) {
        std::size_t MinSize = std::min(Arrays.first.size(), Arrays.second.size());
        Arrays.first.resize(MinSize);
        Arrays.second.resize(MinSize);
    } else if (Arrays.first.back()[StellarGenerator::_kPhaseIndex] != 9 && Arrays.second.back()[StellarGenerator::_kPhaseIndex] == 9) {
        if (Arrays.first.size() + 1 == Arrays.second.size()) {
            Arrays.second.pop_back();
            Arrays.second.back()[StellarGenerator::_kPhaseIndex] = Arrays.first.back()[StellarGenerator::_kPhaseIndex];
            Arrays.second.back()[StellarGenerator::_kXIndex] = Arrays.first.back()[StellarGenerator::_kXIndex];
        } else {
            std::size_t MinSize = std::min(Arrays.first.size(), Arrays.second.size());
            Arrays.first.resize(MinSize - 1);
            Arrays.second.resize(MinSize - 1);
            Arrays.second.back()[StellarGenerator::_kPhaseIndex] = Arrays.first.back()[StellarGenerator::_kPhaseIndex];
            Arrays.second.back()[StellarGenerator::_kXIndex] = Arrays.first.back()[StellarGenerator::_kXIndex];
        }
    } else if (Arrays.first.back()[StellarGenerator::_kPhaseIndex] == 9 && Arrays.second.back()[StellarGenerator::_kPhaseIndex] == 9) {
        TrimArray(Arrays);
    } else {
        auto LastArray1 = Arrays.first.back();
        auto LastArray2 = Arrays.second.back();
        std::size_t MinSize = std::min(Arrays.first.size(), Arrays.second.size());
        Arrays.first.resize(MinSize - 1);
        Arrays.second.resize(MinSize - 1);
        Arrays.first.emplace_back(LastArray1);
        Arrays.second.emplace_back(LastArray2);
    }
}

std::vector<double> InterpolateRows(const std::shared_ptr<StellarGenerator::HrDiagram>& Data, double BvColorIndex) {
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

std::vector<double> InterpolateRows(const std::shared_ptr<StellarGenerator::MistData>& Data, double EvolutionProgress) {
    return InterpolateRows(Data, EvolutionProgress, "x", StellarGenerator::_kXIndex, false);
}

std::vector<double> InterpolateRows(const std::shared_ptr<StellarGenerator::WdMistData>& Data, double TargetAge) {
    return InterpolateRows(Data, TargetAge, "star_age", StellarGenerator::_kWdStarAgeIndex, true);
}

std::vector<double> InterpolateRows(const auto& Data, double Target, const std::string& Header, int Index, bool bIsWhiteDwarf) {
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

        double Coefficient = (Target - SurroundingRows.first[Index]) / (SurroundingRows.second[Index] - SurroundingRows.first[Index]);
        Result = InterpolateFinalData(SurroundingRows, Coefficient, bIsWhiteDwarf);
    } else {
        Result = SurroundingRows.first;
    }

    return Result;
}

std::vector<double> InterpolateArray(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Coefficient) {
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

std::vector<double> InterpolateFinalData(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Coefficient, bool bIsWhiteDwarf) {
    if (DataArrays.first.size() != DataArrays.second.size()) {
        throw std::runtime_error("Data arrays size mismatch.");
    }

    std::vector<double> Result = InterpolateArray(DataArrays, Coefficient);

    if (!bIsWhiteDwarf) {
        Result[StellarGenerator::_kPhaseIndex] = DataArrays.first[StellarGenerator::_kPhaseIndex];
    }

    return Result;
}

void ExpandMistData(std::vector<double>& StarData, double TargetMass) {
    double RadiusSol     = std::pow(10.0, StarData[StellarGenerator::_kLogRIndex]);
    double Teff          = std::pow(10.0, StarData[StellarGenerator::_kLogTeffIndex]);
    double LuminositySol = std::pow(RadiusSol, 2.0) * std::pow((Teff / kSolarTeff), 4.0);

    double& StarMass = StarData[StellarGenerator::_kStarMassIndex];
    double& StarMdot = StarData[StellarGenerator::_kStarMdotIndex];
    double& LogR     = StarData[StellarGenerator::_kLogRIndex];
    double& LogTeff  = StarData[StellarGenerator::_kLogTeffIndex];

    double LogL = std::log10(LuminositySol);

    StarMass = TargetMass * (StarMass / 0.1);
    StarMdot = TargetMass * (StarMdot / 0.1);

    RadiusSol = std::pow(10.0, LogR) * std::pow(TargetMass / 0.1, 2.3);
    LuminositySol = std::pow(10.0, LogL) * std::pow(TargetMass / 0.1, 2.3);

    Teff = kSolarTeff * std::pow((LuminositySol / std::pow(RadiusSol, 2.0)), 0.25);
    LogTeff = std::log10(Teff);

    LogR = std::log10(RadiusSol);
}

_MODULES_END
_NPGS_END
