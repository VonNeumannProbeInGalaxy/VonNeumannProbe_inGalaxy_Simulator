#include "StellarGenerator.h"

#include <cmath>

#include <array>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <iterator>
#include <stdexcept>
#include <string>
#include <sstream>
#include <utility>
#include <vector>

#include "Engine/Core/Logger.h"

_NPGS_BEGIN
_MODULES_BEGIN

StellarGenerator::StellarGenerator(int Seed) : _RandomEngine(Seed) {}

AstroObject::Star StellarGenerator::GenStar() {
    BasicProperties BasicData = GenBasicProperties();
    std::vector<double> ActuallyData = GetActuallyMistData(static_cast<BasicProperties>(BasicData));
    return {};
}

AstroObject::Star StellarGenerator::operator=(const BasicProperties& Properties) {
    AstroObject::Star Star;
    Star.SetParentBody(Properties.StarSys);
    Star.SetMass(Properties.Mass);
    Star.SetAge(Properties.Age);
    Star.SetFeH(Properties.FeH);

    return Star;
}

double StellarGenerator::DefaultPdf(double Mass) {
    // n01 = (0.158 /  log(10))      * exp(-1.0 * pow(log10(1) - log10(0.08), 2.0) / 2 * pow(0.69, 2.0))
    // g1  = (0.158 / (log(10) * m)) * exp(-1.0 * pow(log10(1) - log10(0.08), 2.0) / 2 * pow(0.69, 2.0)), m <= 1Msun
    // g1  = n01 * pow(m, -2.35), m > 1Msun
    // ------------------------------------
    double g1 = 0.0;
    if (Mass <= 1.0) {
        g1 = (0.158 / (std::log(10) * Mass)) * std::exp(-1.0 * std::pow(std::log10(1) - std::log10(0.08), 2.0) / 2 * std::pow(0.69, 2.0));
    } else {
        double n01 = (0.158 / std::log(10)) * std::exp(-1.0 * std::pow(std::log10(1) - std::log10(0.08), 2.0) / 2 * std::pow(0.69, 2.0));
        g1 = n01 * std::pow(Mass, -2.35);
    }

    return g1;
}

double StellarGenerator::GenMass(double MaxPdf) {
    double Mass = 0.0;
    double Probability = 0.0;
    do {
        Mass = 0.1 + _UniformDistribution(_RandomEngine) * (300.0 - 0.1);
        Probability = DefaultPdf(Mass);
    } while (_UniformDistribution(_RandomEngine) * MaxPdf > Probability);

    return Mass;
}

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

    double Lifetime = pow(10, 10) * pow(Mass, -2.5);

    double Age = _UniformDistribution(_RandomEngine) * std::min(Lifetime, 3e12);
    Properties.Age = Age;

    double FeH = -1.5 + _UniformDistribution(_RandomEngine) * (0.5 - (-1.5));
    Properties.FeH = FeH;

    return Properties;
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
    for (const auto& Entry : std::filesystem::directory_iterator(FeHStr)) {
        std::string Filename = Entry.path().filename().string();
        double Mass = std::stod(Filename.substr(0, Filename.find("Ms_track.csv")));
        Masses.emplace_back(Mass);
    }

    auto it = std::lower_bound(Masses.begin(), Masses.end(), TargetMass);
    if (it == Masses.end()) {
        throw std::out_of_range("Mass value out of range.");
    }

    double LowerMass = 0;
    double UpperMass = 0;

    if (*it == TargetMass) {
        LowerMass = UpperMass = *it;
    } else {
        LowerMass =  it == Masses.begin() ? *it : *(it - 1);
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

    Files.first  = LowerMassFile;
    Files.second = UpperMassFile;

    std::vector<double> Result = InterpolateMistData(Files, TargetAge, MassFactor);

    return Result;
}

std::vector<double> StellarGenerator::InterpolateMistData(const std::pair<std::string, std::string>& Files, double TargetAge, double MassFactor) {
    std::vector<double> Result(12);
    
    try {
        if (Files.first != Files.second) [[likely]] {
            if (Assets::AssetManager::GetAsset<MistData>(Files.first) == nullptr) {
                Assets::AssetManager::AddAsset<MistData>(Files.first, std::make_shared<MistData>(MistData(Files.first, _MistHeaders)));
            }

            if (Assets::AssetManager::GetAsset<MistData>(Files.second) == nullptr) {
                Assets::AssetManager::AddAsset<MistData>(Files.second, std::make_shared<MistData>(MistData(Files.second, _MistHeaders)));
            }

            std::shared_ptr<MistData> LowerData = Assets::AssetManager::GetAsset<MistData>(Files.first);
            std::shared_ptr<MistData> UpperData = Assets::AssetManager::GetAsset<MistData>(Files.second);

            auto LowerPhaseChanges = FindPhaseChanges(LowerData);
            auto UpperPhaseChanges = FindPhaseChanges(UpperData);

            std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>> PhaseChangePair{ LowerPhaseChanges, UpperPhaseChanges };
            double EvolutionProgress = CalcEvolutionProgress(PhaseChangePair, TargetAge, MassFactor);

            auto LowerSurroundingRows = LowerData->FindSurroundingValues("x", EvolutionProgress);
            auto UpperSurroundingRows = UpperData->FindSurroundingValues("x", EvolutionProgress);

            double LowerProgressFactor = (EvolutionProgress - LowerSurroundingRows.first[11]) / (LowerSurroundingRows.second[11] - LowerSurroundingRows.first[11]);
            double UpperProgressFactor = (EvolutionProgress - UpperSurroundingRows.first[11]) / (UpperSurroundingRows.second[11] - UpperSurroundingRows.first[11]);

            auto LowerRows = InterpolateArray(LowerSurroundingRows, LowerProgressFactor);
            auto UpperRows = InterpolateArray(UpperSurroundingRows, UpperProgressFactor);

            Result = InterpolateArray({ LowerRows, UpperRows }, MassFactor);
            Result.emplace_back(EvolutionProgress);
        } else [[unlikely]] {
            if (Assets::AssetManager::GetAsset<MistData>(Files.first) == nullptr) {
                Assets::AssetManager::AddAsset<MistData>(Files.first, std::make_shared<MistData>(MistData(Files.first, _MistHeaders)));
            }

            std::shared_ptr<MistData> Data = Assets::AssetManager::GetAsset<MistData>(Files.first);
            auto   PhaseChanges      = FindPhaseChanges(Data);
            std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>> PhaseChangePair{ PhaseChanges, {} };
            double EvolutionProgress = CalcEvolutionProgress(PhaseChangePair, TargetAge, MassFactor);
            auto   SurroundingRows   = Data->FindSurroundingValues("x", EvolutionProgress);
            double ProgressFactor    = (EvolutionProgress - SurroundingRows.first[11]) / (SurroundingRows.second[11] - SurroundingRows.first[11]);

            Result = InterpolateArray(SurroundingRows, ProgressFactor);
            Result.emplace_back(EvolutionProgress);
        }
    } catch (std::exception& e) {
        NpgsError("Error: " + std::string(e.what()));
    }

    return Result;
}

std::vector<std::vector<double>> StellarGenerator::FindPhaseChanges(std::shared_ptr<MistData> DataCsv) {
    std::vector<std::vector<double>> Result;

    auto* DataArray = DataCsv->Data();
    int CurrentPhase = -2;
    int OccurredTimes = 1;
    for (const auto& Row : *DataArray) {
        ++OccurredTimes;
        if (Row[10] != CurrentPhase) {
            CurrentPhase = static_cast<int>(Row[10]);
            Result.emplace_back(Row);
            OccurredTimes = 1;
        }
    }

    if (OccurredTimes == 1) {
        Result.pop_back();
    }

    Result.emplace_back(DataArray->back());

    return Result;
}

std::pair<double, std::pair<double, double>> StellarGenerator::FindSurroundingTimePoints(const std::vector<std::vector<double>>& PhaseChanges, double TargetAge, double MassFactor) {
    std::vector<std::vector<double>>::const_iterator LowerTimePoint;
    std::vector<std::vector<double>>::const_iterator UpperTimePoint;
    
    if (PhaseChanges.size() != 2) {
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

        if (LowerTimePoint == PhaseChanges.end()) {
            throw std::out_of_range("Time point out of range.");
        }
    } else {
        LowerTimePoint = PhaseChanges.begin();
        UpperTimePoint = std::prev(PhaseChanges.end(), 1);
    }

    return { LowerTimePoint->at(11), { LowerTimePoint->at(0), UpperTimePoint->at(0) } };
}

double StellarGenerator::CalcEvolutionProgress(std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& PhaseChanges, double TargetAge, double MassFactor) {
    double Result = 0.0;

    const auto& [LowerPhase, LowerTimePoints] = FindSurroundingTimePoints(PhaseChanges.first,  TargetAge, MassFactor);
    if (PhaseChanges.second.empty()) {
        Result = (TargetAge - LowerTimePoints.first) / (LowerTimePoints.second - LowerTimePoints.first) + LowerPhase;
    }

    if (PhaseChanges.first.size() == PhaseChanges.second.size()) {
        bool bIsPhaseChangeSame = true;
        for (std::size_t i = 0; i != PhaseChanges.first.size() - 1; ++i) {
            if (PhaseChanges.first.at(i).at(10) != PhaseChanges.second.at(i).at(10)) {
                bIsPhaseChangeSame = false;
                break;
            }
        }

        if (bIsPhaseChangeSame) {
            const auto& [UpperPhase, UpperTimePoints] = FindSurroundingTimePoints(PhaseChanges.second, TargetAge, MassFactor);

            const auto& [LowerLowerTimePoint, LowerUpperTimePoint] = LowerTimePoints;
            const auto& [UpperLowerTimePoint, UpperUpperTimePoint] = UpperTimePoints;

            double LowerTimePoint = LowerLowerTimePoint + (UpperLowerTimePoint - LowerLowerTimePoint) * MassFactor;
            double UpperTimePoint = LowerUpperTimePoint + (UpperUpperTimePoint - LowerUpperTimePoint) * MassFactor;

            Result = (TargetAge - LowerTimePoint) / (UpperTimePoint - LowerTimePoint) + LowerPhase;
        } else {
            PhaseChanges.first.erase(std::prev(PhaseChanges.first.end(), 2));
            PhaseChanges.second.erase(std::prev(PhaseChanges.second.end(), 2));
            Result = CalcEvolutionProgress(PhaseChanges, TargetAge, MassFactor);
        }
    } else {
        double FirstDiscardTimePoint     = 0.0;
        double FirstCommonTimePoint      = std::prev(PhaseChanges.first.end(), 2)->at(0);
        double FirstLowerDiscardProgress = 0.0;
        double FirstUpperDiscardProgress = 0.0;

        std::size_t MinSize = std::min(PhaseChanges.first.size(), PhaseChanges.second.size());
        for (std::size_t i = 0; i != MinSize - 1; ++i) {
            if (PhaseChanges.first.at(i).at(10) != PhaseChanges.second.at(i).at(10)) {
                FirstDiscardTimePoint = PhaseChanges.first.at(i).at(0);
                FirstLowerDiscardProgress = PhaseChanges.first.at(i - 1).at(11);
                FirstUpperDiscardProgress = PhaseChanges.second.at(i - 1).at(11);
                break;
            }
        }

        if (PhaseChanges.first.at(PhaseChanges.first.size() - 2).at(10) ==
            PhaseChanges.second.at(PhaseChanges.second.size() - 2).at(10))
        {
            double DeltaTimePoint = FirstCommonTimePoint - FirstDiscardTimePoint;
            std::prev(PhaseChanges.first.end(), 2)->at(0) -= DeltaTimePoint;
            PhaseChanges.first.back().at(0) -= DeltaTimePoint;
            PhaseChanges.first.erase(PhaseChanges.first.begin(), PhaseChanges.first.end() - 2);
            PhaseChanges.second.erase(PhaseChanges.second.begin(), PhaseChanges.second.end() - 2);
        } else {
            TrimArrays(PhaseChanges);
        }

        Result = CalcEvolutionProgress(PhaseChanges, TargetAge, MassFactor);
        if (Result > std::min(FirstLowerDiscardProgress, FirstUpperDiscardProgress)) {
            Result = 9.0;
        }
    }

    return Result;
}

void StellarGenerator::TrimArrays(std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& Arrays) {
    std::size_t MinSize = std::min(Arrays.first.size(), Arrays.second.size());
    std::vector<std::vector<double>> TrimmedArray1(Arrays.first.begin(), Arrays.first.begin() + MinSize);
    std::vector<std::vector<double>> TrimmedArray2(Arrays.second.begin(), Arrays.second.begin() + MinSize);

    Arrays.first  = TrimmedArray1;
    Arrays.second = TrimmedArray2;
}

std::vector<double> StellarGenerator::InterpolateArray(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Factor) {
    if (DataArrays.first.size() != DataArrays.second.size()) {
        throw std::runtime_error("Data arrays size mismatch.");
    }

    std::vector<double> Result(11);
    for (std::size_t i = 0; i != 10; ++i) {
        Result[i] = DataArrays.first[i] + (DataArrays.second[i] - DataArrays.first[i]) * Factor;
    }

    Result[10] = DataArrays.first[10];
    return Result;
}

_MODULES_END
_NPGS_END
