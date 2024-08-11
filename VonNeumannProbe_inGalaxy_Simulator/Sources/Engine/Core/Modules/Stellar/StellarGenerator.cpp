#include "StellarGenerator.h"

#include <cmath>

#include <algorithm>
#include <array>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Engine/Core/Constants.h"
#include "Engine/Core/Logger.h"

_NPGS_BEGIN
_MODULES_BEGIN

// Tool functions
// --------------
static double CalcEvolutionProgress(std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& PhaseChanges, double TargetAge, double MassFactor);
static std::pair<double, std::pair<double, double>> FindSurroundingTimePoints(const std::vector<std::vector<double>>& PhaseChanges, double TargetAge);
static std::pair<double, std::size_t> FindSurroundingTimePoints(const std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& PhaseChanges, double TargetAge, double MassFactor);
static void AlignArrays(std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& Arrays);
static std::vector<double> InterpolateRows(const std::shared_ptr<StellarGenerator::MistData>& Data, double EvolutionProgress);
static std::vector<double> InterpolateArray(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Factor);
static std::vector<double> InterpolateFinalData(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Factor);
static void GenSpectralType(AstroObject::Star& StarData);
static StellarClass::LuminosityClass CalcLuminosityClass(const AstroObject::Star& StarData);

// StellarGenerator implementations
// --------------------------------
StellarGenerator::StellarGenerator(int Seed) :
    _RandomEngine(Seed),
    _MistHeaders({ "star_age",   "star_mass",   "star_mdot",    "log_Teff",       "log_R", "log_surf_z",
                   "surface_h1", "surface_he3", "log_center_T", "log_center_Rho", "phase", "x" })
{}

StellarGenerator::BasicProperties StellarGenerator::GenBasicProperties() {
    BasicProperties Properties;

    int PosX     = static_cast<int>(_UniformDistribution(_RandomEngine) * 1000);
    int PosY     = static_cast<int>(_UniformDistribution(_RandomEngine) * 1000);
    int PosZ     = static_cast<int>(_UniformDistribution(_RandomEngine) * 1000);
    int Distance = static_cast<int>(_UniformDistribution(_RandomEngine) * 10000);
    Properties.StarSys.Name     = "S-" + std::to_string(PosX) + "-" + std::to_string(PosY) + "-" + std::to_string(PosZ) + "-" + std::to_string(Distance);
    Properties.StarSys.Position = glm::dvec3(PosX, PosY, PosZ);
    Properties.StarSys.Distance = Distance;

    double Mass = GenMass(3.125);
    Properties.Mass = Mass;

    // double Lifetime = pow(10, 10) * pow(Mass, -2.5);

    double Age = _UniformDistribution(_RandomEngine) * std::min(1e7, 3e12);
    Properties.Age = Age;

    double FeH = -1.5 + _UniformDistribution(_RandomEngine) * (0.5 - (-1.5));
    Properties.FeH = FeH;

    return Properties;
}

AstroObject::Star StellarGenerator::GenStar() {
    BasicProperties BasicData = GenBasicProperties();
    // std::println("{}, {}, {}", BasicData.Age, BasicData.FeH, BasicData.Mass);
    return GenStar(BasicData);
}

AstroObject::Star StellarGenerator::GenStar(const BasicProperties& Properties) {
    AstroObject::Star Star(Properties);
    auto StarData = GetActuallyMistData(Properties);

    if (StarData.empty()) {
        return {};
    }

    double Age               = StarData[_kStarAgeIndex];
    double MassSol           = StarData[_kStarMassIndex];
    double RadiusSol         = std::pow(10.0, StarData[_kLogRIndex]);
    double Teff              = std::pow(10.0, StarData[_kLogTeffIndex]);
    double SurfaceFeH        = StarData[_kLogSurfZIndex];
    double CoreTemp          = std::pow(10.0, StarData[_kLogCenterTIndex]);
    double CoreDensity       = std::pow(10.0, StarData[_kLogCenterRhoIndex]);
    double MassLossRate      = StarData[_kStarMdotIndex];
    double EvolutionProgress = StarData[_kXIndex];
    double Lifetime          = StarData[_kLifetimeIndex];

    double LuminositySol     = std::pow(RadiusSol, 2) * std::pow((Teff / kSolarTeff), 4);
    double AbsoluteMagnitude = kSolarAbsoluteMagnitude - 2.5 * std::log10(LuminositySol);
    double EscapeVelocity    = std::sqrt((2 * kGravityConstant * MassSol * kSolarMass) / (RadiusSol * kSolarRadius));

    double LifeProgress      = Age / Lifetime;
    double WindSpeedFactor   = 3.0 - LifeProgress;
    double StellarWindSpeed  = WindSpeedFactor * EscapeVelocity;

    AstroObject::Star::Phase EvolutionPhase = static_cast<AstroObject::Star::Phase>(StarData[10]);

    Star.SetAge(Age).SetMass(MassSol * kSolarMass).SetRadius(RadiusSol * kSolarRadius).SetEscapeVelocity(EscapeVelocity);
    Star.SetLuminosity(LuminositySol * kSolarLuminosity)
        .SetAbsoluteMagnitude(AbsoluteMagnitude)
        .SetTeff(Teff)
        .SetSurfaceFeH(SurfaceFeH)
        .SetCoreTemp(CoreTemp)
        .SetCoreDensity(CoreDensity)
        .SetStellarWindSpeed(StellarWindSpeed)
        .SetStellarWindMassLossRate(-(MassLossRate * kSolarMass / 31536000))
        .SetEvolutionProgress(EvolutionProgress)
        .SetEvolutionPhase(EvolutionPhase)
        .SetLifetime(Lifetime);

    GenSpectralType(Star);

    return Star;
}

double StellarGenerator::DefaultPdf(double Fuck) {
    // n01 = (0.158 /  log(10))      * exp(-1.0 * pow(log10(1) - log10(0.08), 2.0) / 2 * pow(0.69, 2.0))
    // g1  = (0.158 / (log(10) * m)) * exp(-1.0 * pow(log10(1) - log10(0.08), 2.0) / 2 * pow(0.69, 2.0)), m <= 1Msun
    // g1  = n01 * pow(m, -2.35),                                                                         m >  1Msun
    // -------------------------------------------------------------------------------------------------------------
    // double g1 = 0.0;
    // if (Mass <= 1.0) {
    //     g1 = (0.158 / (std::log(10) * Mass)) * std::exp(-1.0 * std::pow(std::log10(1) - std::log10(0.08), 2.0) / 2 * std::pow(0.69, 2.0));
    // } else {
    //     double n01 = (0.158 / std::log(10)) * std::exp(-1.0 * std::pow(std::log10(1) - std::log10(0.08), 2.0) / 2 * std::pow(0.69, 2.0));
    //     g1 = n01 * std::pow(Mass, -2.35);
    // }

    double g2 = 0.0;
    if (std::pow(10, Fuck) <= 1.0) {
        g2 = (0.158 / (std::pow(10, Fuck) * std::log(10))) * std::exp(-1 * (std::pow(Fuck - std::log10(0.08), 2)) / (2 * std::pow(0.69, 2))) * (1 / std::pow(10, Fuck) * std::log(10));
    } else {
        double n01 = (0.158 / std::log(10)) * std::exp(-1.0 * std::pow(std::log10(1) - std::log10(0.08), 2) / 2 * std::pow(0.69, 2));
        g2 = n01 * std::pow(std::pow(10, Fuck), -2.35) * (1 / std::pow(10, Fuck) * std::log(10));
    }

    return g2;
}

double StellarGenerator::GenMass(double MaxPdf) {
    std::uniform_real_distribution<double> UniformDistribution(0.1, 2.0);
    std::uniform_real_distribution<double> LogDistribution(std::log10(2.0), std::log10(300.0));
    // std::exponential_distribution<double> ExponentialDistbution(1.0);

    double Mass = 0.0;
    double Probability = 0.0;
    do {
        if (_UniformDistribution(_RandomEngine) < 0.5) {
            Mass = UniformDistribution(_RandomEngine);
        } else {
            // Mass = 2.0 + ExponentialDistbution(_RandomEngine);
            Mass = std::exp(LogDistribution(_RandomEngine));
        }
        Probability = DefaultPdf(std::log10(Mass));
    } while (_UniformDistribution(_RandomEngine) * MaxPdf > Probability);

    return Mass;
}

std::vector<double> StellarGenerator::GetActuallyMistData(const BasicProperties& Properties) {
    std::array<double, 5> PresetFeH{ -1.5, -1.0, -0.5, 0.0, 0.5 };
    double TargetAge  = Properties.Age;
    double TargetFeH  = Properties.FeH;
    double TargetMass = Properties.Mass;

    double ClosestFeH = *std::min_element(PresetFeH.begin(), PresetFeH.end(), [TargetFeH](double Lhs, double Rhs) -> bool {
        return std::abs(Lhs - TargetFeH) < std::abs(Rhs - TargetFeH);
    });

    TargetFeH = ClosestFeH;

    std::stringstream MassStream;
    MassStream << std::fixed << std::setfill('0') << std::setw(6) << std::setprecision(2) << TargetMass;
    std::string MassStr = MassStream.str() + "0";

    std::pair<std::string, std::string> Files;
    std::stringstream FeHStream;
    FeHStream << std::fixed << std::setprecision(1) << TargetFeH;
    std::string FeHStr = FeHStream.str();
    if (TargetFeH >= 0.0) {
        FeHStr.insert(FeHStr.begin(), '+');
    }
    FeHStr.insert(0, "Assets/Models/MIST/[Fe_H]=");

    std::vector<double> Masses;
    _CacheMutex.lock();
    if (_MassFileCache.contains(FeHStr)) {
        Masses = _MassFileCache[FeHStr];
    } else {
        _CacheMutex.unlock();
        for (const auto& Entry : std::filesystem::directory_iterator(FeHStr)) {
            std::string Filename = Entry.path().filename().string();
            double Mass = std::stod(Filename.substr(0, Filename.find("Ms_track.csv")));
            Masses.emplace_back(Mass);
        }

        _CacheMutex.lock();
        _MassFileCache.emplace(FeHStr, Masses);
    }
    _CacheMutex.unlock();

    auto it = std::lower_bound(Masses.begin(), Masses.end(), TargetMass);
    if (it == Masses.end()) {
        throw std::out_of_range("Mass value out of range.");
    }

    double LowerMass = 0;
    double UpperMass = 0;

    if (*it == TargetMass) {
        LowerMass = UpperMass = *it;
    } else {
        LowerMass = it == Masses.begin() ? *it : *(it - 1);
        UpperMass = *it;
    }

    double MassFactor = (TargetMass - LowerMass) / (UpperMass - LowerMass);

    MassStr.clear();
    MassStream.str("");
    MassStream.clear();

    MassStream << std::fixed << std::setfill('0') << std::setw(6) << std::setprecision(2) << LowerMass;
    MassStr = MassStream.str() + "0";
    std::string LowerMassFile = FeHStr + "/" + MassStr + "Ms_track.csv";

    MassStr.clear();
    MassStream.str("");
    MassStream.clear();

    MassStream << std::fixed << std::setfill('0') << std::setw(6) << std::setprecision(2) << UpperMass;
    MassStr = MassStream.str() + "0";
    std::string UpperMassFile = FeHStr + "/" + MassStr + "Ms_track.csv";

    Files.first = LowerMassFile;
    Files.second = UpperMassFile;

    std::vector<double> Result = InterpolateMistData(Files, TargetAge, MassFactor);

    return Result;
}

std::shared_ptr<StellarGenerator::MistData> StellarGenerator::LoadMistData(const std::string& Filename) {
    std::lock_guard<std::mutex> Lock(_CacheMutex);
    if (Assets::AssetManager::GetAsset<MistData>(Filename) == nullptr) {
        Assets::AssetManager::AddAsset<MistData>(Filename, std::make_shared<MistData>(MistData(Filename, _MistHeaders)));
    }

    return Assets::AssetManager::GetAsset<MistData>(Filename);
}

std::vector<double> StellarGenerator::InterpolateMistData(const std::pair<std::string, std::string>& Files, double TargetAge, double MassFactor) {
    std::vector<double> Result;

    try {
        if (Files.first != Files.second) [[likely]] {
            std::shared_ptr<StellarGenerator::MistData> LowerData = LoadMistData(Files.first);
            std::shared_ptr<StellarGenerator::MistData> UpperData = LoadMistData(Files.second);

            auto LowerPhaseChanges = FindPhaseChanges(LowerData);
            auto UpperPhaseChanges = FindPhaseChanges(UpperData);

            std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>> PhaseChangePair{ LowerPhaseChanges, UpperPhaseChanges };
            double EvolutionProgress = CalcEvolutionProgress(PhaseChangePair, TargetAge, MassFactor);

            double LowerLifetime = PhaseChangePair.first.back()[_kStarAgeIndex];
            double UpperLifetime = PhaseChangePair.second.back()[_kStarAgeIndex];

            auto LowerSurroundingRows = LowerData->FindSurroundingValues("x", EvolutionProgress);
            auto UpperSurroundingRows = UpperData->FindSurroundingValues("x", EvolutionProgress);

            std::vector<double> LowerRows = InterpolateRows(LowerData, EvolutionProgress);
            std::vector<double> UpperRows = InterpolateRows(UpperData, EvolutionProgress);

            LowerRows.emplace_back(LowerLifetime);
            UpperRows.emplace_back(UpperLifetime);

            Result = InterpolateFinalData({ LowerRows, UpperRows }, MassFactor);
        } else [[unlikely]] {
            std::shared_ptr<MistData> StarData = LoadMistData(Files.first);
            auto PhaseChanges = FindPhaseChanges(StarData);
            std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>> PhaseChangePair{ PhaseChanges, {} };
            double EvolutionProgress = CalcEvolutionProgress(PhaseChangePair, TargetAge, MassFactor);
            double Lifetime = PhaseChanges.back()[_kStarAgeIndex];
            Result = InterpolateRows(StarData, EvolutionProgress);
            Result.emplace_back(Lifetime);
        }
    } catch (std::exception& e) {
        NpgsCoreError("Error: " + std::string(e.what()));
    }

    return Result;
}

std::vector<std::vector<double>> StellarGenerator::FindPhaseChanges(const std::shared_ptr<MistData>& DataCsv) {
    std::vector<std::vector<double>> Result;

    _CacheMutex.lock();
    if (_PhaseChangesCache.contains(DataCsv)) {
        Result = _PhaseChangesCache[DataCsv];
    } else {
        _CacheMutex.unlock();
        auto* DataArray = DataCsv->Data();
        int CurrentPhase = -2;
        for (const auto& Row : *DataArray) {
            if (Row[_kPhaseIndex] != CurrentPhase || Row[_kXIndex] == 10.0) {
                CurrentPhase = static_cast<int>(Row[_kPhaseIndex]);
                Result.emplace_back(Row);
            }
        }

        _CacheMutex.lock();
        _PhaseChangesCache.emplace(DataCsv, Result);
    }
    _CacheMutex.unlock();

    return Result;
}

const int StellarGenerator::_kStarAgeIndex      = 0;
const int StellarGenerator::_kStarMassIndex     = 1;
const int StellarGenerator::_kStarMdotIndex     = 2;
const int StellarGenerator::_kLogTeffIndex      = 3;
const int StellarGenerator::_kLogRIndex         = 4;
const int StellarGenerator::_kLogSurfZIndex     = 5;
const int StellarGenerator::_kSurfaceH1Index    = 6;
const int StellarGenerator::_kSurfaceHe3Index   = 7;
const int StellarGenerator::_kLogCenterTIndex   = 8;
const int StellarGenerator::_kLogCenterRhoIndex = 9;
const int StellarGenerator::_kPhaseIndex        = 10;
const int StellarGenerator::_kXIndex            = 11;
const int StellarGenerator::_kLifetimeIndex     = 12;

std::unordered_map<std::string, std::vector<double>> StellarGenerator::_MassFileCache;
std::unordered_map<std::shared_ptr<StellarGenerator::MistData>, std::vector<std::vector<double>>> StellarGenerator::_PhaseChangesCache;
std::mutex StellarGenerator::_CacheMutex;

// Tool functions implementations
// ------------------------------
static double CalcEvolutionProgress(std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& PhaseChanges, double TargetAge, double MassFactor) {
    double Result = 0.0;
    double Phase  = 0.0;

    if (PhaseChanges.second.empty()) [[unlikely]] {
        const auto& TimePointResults = FindSurroundingTimePoints(PhaseChanges.first, TargetAge);
        Phase = TimePointResults.first;
        const auto& TimePoints = TimePointResults.second;
        Result = (TargetAge - TimePoints.first) / (TimePoints.second - TimePoints.first) + Phase;
    } else [[likely]] {
        if (PhaseChanges.first.size() == PhaseChanges.second.size()) {
            const auto& TimePointResults = FindSurroundingTimePoints(PhaseChanges, TargetAge, MassFactor);

            Phase = TimePointResults.first;
            std::size_t Index = TimePointResults.second;

            if (Index + 1 != PhaseChanges.first.size()) {
                std::pair<double, double> LowerTimePoints = { PhaseChanges.first[Index][StellarGenerator::_kStarAgeIndex], PhaseChanges.first[Index + 1][StellarGenerator::_kStarAgeIndex] };
                std::pair<double, double> UpperTimePoints = { PhaseChanges.second[Index][StellarGenerator::_kStarAgeIndex], PhaseChanges.second[Index + 1][StellarGenerator::_kStarAgeIndex] };

                const auto& [LowerLowerTimePoint, LowerUpperTimePoint] = LowerTimePoints;
                const auto& [UpperLowerTimePoint, UpperUpperTimePoint] = UpperTimePoints;

                double LowerTimePoint = LowerLowerTimePoint + (UpperLowerTimePoint - LowerLowerTimePoint) * MassFactor;
                double UpperTimePoint = LowerUpperTimePoint + (UpperUpperTimePoint - LowerUpperTimePoint) * MassFactor;

                Result = (TargetAge - LowerTimePoint) / (UpperTimePoint - LowerTimePoint) + Phase;
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

            Result = CalcEvolutionProgress(PhaseChanges, TargetAge, MassFactor);
            double IntegerPart = 0.0;
            double FractionalPart = std::modf(Result, &IntegerPart);
            if (FractionalPart > 0.99 && Result < 9.0 && IntegerPart >= (*std::prev(PhaseChanges.first.end(), 3))[StellarGenerator::_kPhaseIndex]) {
                Result = 9.0;
            }
        }
    }

    return Result;
}

static std::pair<double, std::pair<double, double>> FindSurroundingTimePoints(const std::vector<std::vector<double>>& PhaseChanges, double TargetAge) {
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
            --LowerTimePoint;
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

static std::pair<double, std::size_t> FindSurroundingTimePoints(const std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& PhaseChanges, double TargetAge, double MassFactor) {
    std::vector<double> LowerPhaseChangeTimePoints;
    std::vector<double> UpperPhaseChangeTimePoints;
    for (std::size_t i = 0; i != PhaseChanges.first.size(); ++i) {
        LowerPhaseChangeTimePoints.emplace_back(PhaseChanges.first[i][StellarGenerator::_kStarAgeIndex]);
        UpperPhaseChangeTimePoints.emplace_back(PhaseChanges.second[i][StellarGenerator::_kStarAgeIndex]);
    }

    std::vector<double> PhaseChangeTimePoints = InterpolateArray({ LowerPhaseChangeTimePoints, UpperPhaseChangeTimePoints }, MassFactor);

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

static void AlignArrays(std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& Arrays) {
    if (Arrays.first.back()[StellarGenerator::_kPhaseIndex] != 9 && Arrays.second.back()[StellarGenerator::_kPhaseIndex] != 9) {
        std::size_t MinSize = std::min(Arrays.first.size(), Arrays.second.size());
        Arrays.first.resize(MinSize);
        Arrays.second.resize(MinSize);
    } else if (Arrays.first.back()[StellarGenerator::_kPhaseIndex] != 9 && Arrays.second.back()[StellarGenerator::_kPhaseIndex] == 9) {
        Arrays.first.back()[StellarGenerator::_kPhaseIndex] = 9;
        Arrays.first.back()[StellarGenerator::_kXIndex] = 9.0;
    } else if (Arrays.first.back()[StellarGenerator::_kPhaseIndex] == 9 && Arrays.second.back()[StellarGenerator::_kPhaseIndex] == 9) {
        auto LastArray1 = Arrays.first.back();
        auto LastArray2 = Arrays.second.back();
        auto SubLastArray1 = *std::prev(Arrays.first.end(), 2);
        auto SubLastArray2 = *std::prev(Arrays.second.end(), 2);
        std::size_t MinSize = std::min(Arrays.first.size(), Arrays.second.size());
        Arrays.first.resize(MinSize - 2);
        Arrays.second.resize(MinSize - 2);
        Arrays.first.emplace_back(SubLastArray1);
        Arrays.first.emplace_back(LastArray1);
        Arrays.second.emplace_back(SubLastArray2);
        Arrays.second.emplace_back(LastArray2);
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

static std::vector<double> InterpolateRows(const std::shared_ptr<StellarGenerator::MistData>& Data, double EvolutionProgress) {
    std::vector<double> Result;

    auto SurroundingRows = Data->FindSurroundingValues("x", EvolutionProgress);
    if (SurroundingRows.first != SurroundingRows.second) {
        int LowerTempPhase = static_cast<int>(SurroundingRows.first[StellarGenerator::_kPhaseIndex]);
        int UpperTempPhase = static_cast<int>(SurroundingRows.second[StellarGenerator::_kPhaseIndex]);
        if (LowerTempPhase != UpperTempPhase) {
            SurroundingRows.second[StellarGenerator::_kXIndex] = LowerTempPhase + 1;
        }
        double ProgressFactor = (EvolutionProgress - SurroundingRows.first[StellarGenerator::_kXIndex]) / (SurroundingRows.second[StellarGenerator::_kXIndex] - SurroundingRows.first[StellarGenerator::_kXIndex]);
        Result = InterpolateFinalData(SurroundingRows, ProgressFactor);
    } else {
        Result = SurroundingRows.first;
    }

    return Result;
}

static std::vector<double> InterpolateArray(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Factor) {
    if (DataArrays.first.size() != DataArrays.second.size()) {
        throw std::runtime_error("Data arrays size mismatch.");
    }

    std::size_t Size = DataArrays.first.size();
    std::vector<double> Result(Size);
    for (std::size_t i = 0; i != Size; ++i) {
        Result[i] = DataArrays.first[i] + (DataArrays.second[i] - DataArrays.first[i]) * Factor;
    }

    return Result;
}

static std::vector<double> InterpolateFinalData(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Factor) {
    if (DataArrays.first.size() != DataArrays.second.size()) {
        throw std::runtime_error("Data arrays size mismatch.");
    }

    std::vector<double> Result = InterpolateArray(DataArrays, Factor);
    Result[StellarGenerator::_kPhaseIndex] = DataArrays.first[StellarGenerator::_kPhaseIndex];

    return Result;
}

static void GenSpectralType(AstroObject::Star& StarData) {
    double Teff = StarData.GetTeff();
    auto EvolutionPhase = StarData.GetEvolutionPhase();

    // Temp
    if (Teff > 54000) {
        return;
    }

    StellarClass::SpectralType SpectralType{};
    SpectralType.bIsAmStar = false;

    std::vector<std::pair<int, int>> SpecialSubclassMap;
    double Subclass = 0.0;

    if (EvolutionPhase != AstroObject::Star::Phase::kWolfRayet) {
        std::uint32_t SpectralClass = 0;

        for (auto it = AstroObject::Star::_kInitialCommonMap.begin(); it != AstroObject::Star::_kInitialCommonMap.end() - 1; ++it) {
            ++SpectralClass;
            if (it->first >= Teff && (it + 1)->first < Teff) {
                SpecialSubclassMap = it->second;
                break;
            }
        }

        SpectralType.HSpectralClass = static_cast<StellarClass::SpectralClass>(SpectralClass);

        for (auto it = SpecialSubclassMap.begin(); it != SpecialSubclassMap.end() - 1; ++it) {
            if (it->first >= Teff && (it + 1)->first < Teff) {
                double TempFactor = (Teff - (it + 1)->first) / (it->first - (it + 1)->first);
                Subclass = it->second + TempFactor;
                Subclass = std::round(Subclass * 10.0) / 10.0;
                break;
            }
        }

        SpectralType.Subclass = Subclass;

        if (EvolutionPhase != AstroObject::Star::Phase::kPrevMainSequence) {
            if (StarData.GetLuminosity() / kSolarLuminosity <= 100) {
                SpectralType.LuminosityClass = AstroObject::Star::_kLuminosity.at(EvolutionPhase);
            } else {
                SpectralType.LuminosityClass = CalcLuminosityClass(StarData);
            }
        }

        StellarClass Class(StellarClass::StarType::kNormalStar, SpectralType);
        StarData.SetStellarClass(Class);
        StarData.SetSpectralType(Class.ToString());
    }
}

static StellarClass::LuminosityClass CalcLuminosityClass(const AstroObject::Star& StarData) {
    StellarClass::LuminosityClass LuminosityClass = StellarClass::LuminosityClass::kLuminosity_Unknown;

    double MassLossRateYearSol = StarData.GetStellarWindMassLossRate() * 31536000 / kSolarMass;
    if (MassLossRateYearSol > 1e-4) {
        return StellarClass::LuminosityClass::kLuminosity_IaPlus;
    }

    double LuminositySol = StarData.GetLuminosity() / kSolarLuminosity;

    if (LuminositySol > 650000) {
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_0;
    } else if (LuminositySol > 100000) {
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_Ia;
    } else if (LuminositySol > 50000) {
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_Iab;
    } else if (LuminositySol > 10000) {
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_Ib;
    } else if (LuminositySol > 10000) {
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_II;
    } else if (LuminositySol > 3000) {
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_III;
    } else if (LuminositySol > 80) {
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_IV;
    } else if (LuminositySol > 1) {
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_V;
    } else if (LuminositySol > 0.1) {
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_VI;
    }

    return LuminosityClass;
}

_MODULES_END
_NPGS_END
