#include "StellarGenerator.h"

#include <cmath>

#include <array>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>

#include "Engine/Core/Logger.h"

_NPGS_BEGIN
_MODULES_BEGIN

StellarGenerator::StellarGenerator(int Seed) : _RandomEngine(Seed) {}

AstroObject::Star StellarGenerator::GenStar() {
    BasicProperties BasicData = GenBasicProperties();

    double a, f, m;
    while (std::cin >> f >> m >> a) {
        BasicData.Age = a;
        BasicData.FeH = f;
        BasicData.Mass = m;
        std::vector<double> ActuallyData = GetActuallyMistData(static_cast<BasicProperties>(BasicData));

        for (double Data : ActuallyData) {
            std::cout << Data << " ";
        }
        std::endl(std::cout);
    }

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
    TimerBegin;

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

    TimerEnd;

    return Result;
}

std::shared_ptr<StellarGenerator::MistData> StellarGenerator::LoadMistData(const std::string& Filename) {
    if (Assets::AssetManager::GetAsset<MistData>(Filename) == nullptr) {
        Assets::AssetManager::AddAsset<MistData>(Filename, std::make_shared<MistData>(MistData(Filename, _MistHeaders)));
    }
    return Assets::AssetManager::GetAsset<MistData>(Filename);
}

std::vector<double> StellarGenerator::InterpolateMistData(const std::pair<std::string, std::string>& Files, double TargetAge, double MassFactor) {
    TimerBegin;

    std::vector<double> Result(12);

    try {
        if (Files.first != Files.second) [[likely]] {
            std::shared_ptr<MistData> LowerData = LoadMistData(Files.first);
            std::shared_ptr<MistData> UpperData = LoadMistData(Files.second);

            auto LowerPhaseChanges = FindPhaseChanges(LowerData);
            auto UpperPhaseChanges = FindPhaseChanges(UpperData);

            std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>> PhaseChangePair{ LowerPhaseChanges, UpperPhaseChanges };
            double EvolutionProgress = CalcEvolutionProgress(PhaseChangePair, TargetAge, MassFactor);

            auto LowerSurroundingRows = LowerData->FindSurroundingValues("x", EvolutionProgress);
            auto UpperSurroundingRows = UpperData->FindSurroundingValues("x", EvolutionProgress);

            std::vector<double> LowerRows = InterpolateRows(LowerData, EvolutionProgress);
            std::vector<double> UpperRows = InterpolateRows(UpperData, EvolutionProgress);

            Result = InterpolateFinalData({ LowerRows, UpperRows }, MassFactor);
        } else [[unlikely]] {
            std::shared_ptr<MistData> Data = LoadMistData(Files.first);
            auto PhaseChanges = FindPhaseChanges(Data);
            std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>> PhaseChangePair{ PhaseChanges, {} };
            double EvolutionProgress = CalcEvolutionProgress(PhaseChangePair, TargetAge, MassFactor);

            Result = InterpolateRows(Data, EvolutionProgress);
        }
    } catch (std::exception& e) {
        NpgsCoreError("Error: " + std::string(e.what()));
    }

    TimerEnd;

    return Result;
}

std::vector<std::vector<double>> StellarGenerator::FindPhaseChanges(std::shared_ptr<MistData> DataCsv) {
    TimerBegin;

    std::vector<std::vector<double>> Result;

    if (_PhaseChangesCache.contains(DataCsv)) {
        Result = _PhaseChangesCache[DataCsv];
    } else {
        auto* DataArray = DataCsv->Data();
        int CurrentPhase = -2;
        for (const auto& Row : *DataArray) {
            if (Row[10] != CurrentPhase || Row[11] == 10.0) {
                CurrentPhase = static_cast<int>(Row[10]);
                Result.emplace_back(Row);
            }
        }

        _PhaseChangesCache.emplace(DataCsv, Result);
    }

    TimerEnd;

    return Result;
}

std::pair<double, std::pair<double, double>> StellarGenerator::FindSurroundingTimePoints(const std::vector<std::vector<double>>& PhaseChanges, double TargetAge) {
    TimerBegin;

    std::vector<std::vector<double>>::const_iterator LowerTimePoint;
    std::vector<std::vector<double>>::const_iterator UpperTimePoint;
    
    if (PhaseChanges.size() != 2 || PhaseChanges.front().at(10) != PhaseChanges.back().at(10)) {
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

    TimerEnd;

    return { LowerTimePoint->at(11), { LowerTimePoint->at(0), UpperTimePoint->at(0) } };
}

std::pair<double, std::size_t> StellarGenerator::FindSurroundingTimePoints(const std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& PhaseChanges, double TargetAge, double MassFactor) {
    TimerBegin;
    
    std::vector<double> LowerPhaseChangeTimePoints;
    std::vector<double> UpperPhaseChangeTimePoints;
    for (std::size_t i = 0; i != PhaseChanges.first.size(); ++i) {
        LowerPhaseChangeTimePoints.emplace_back(PhaseChanges.first.at(i).at(0));
        UpperPhaseChangeTimePoints.emplace_back(PhaseChanges.second.at(i).at(0));
    }

    std::vector<double> PhaseChangeTimePoints = InterpolateArray({ LowerPhaseChangeTimePoints, UpperPhaseChangeTimePoints }, MassFactor);

    std::vector<std::pair<double, double>> TimePointPairs;
    for (std::size_t i = 0; i != PhaseChanges.first.size(); ++i) {
        TimePointPairs.emplace_back(PhaseChanges.first.at(i).at(10), PhaseChangeTimePoints[i]);
    }

    std::pair<double, std::size_t> Result;
    for (std::size_t i = 0; i != TimePointPairs.size(); ++i) {
        if (TimePointPairs[i].second >= TargetAge) {
            Result.first = TimePointPairs[i == 0 ? 0 : i - 1].first;
            Result.second = i == 0 ? 0 : i - 1;
            break;
        }
    }

    TimerEnd;

    return Result;
}

double StellarGenerator::CalcEvolutionProgress(std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& PhaseChanges, double TargetAge, double MassFactor) {
    TimerBegin;
    
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
                std::pair<double, double> LowerTimePoints = { PhaseChanges.first.at(Index).at(0), PhaseChanges.first.at(Index + 1).at(0) };
                std::pair<double, double> UpperTimePoints = { PhaseChanges.second.at(Index).at(0), PhaseChanges.second.at(Index + 1).at(0) };

                const auto& [LowerLowerTimePoint, LowerUpperTimePoint] = LowerTimePoints;
                const auto& [UpperLowerTimePoint, UpperUpperTimePoint] = UpperTimePoints;

                double LowerTimePoint = LowerLowerTimePoint + (UpperLowerTimePoint - LowerLowerTimePoint) * MassFactor;
                double UpperTimePoint = LowerUpperTimePoint + (UpperUpperTimePoint - LowerUpperTimePoint) * MassFactor;

                Result = (TargetAge - LowerTimePoint) / (UpperTimePoint - LowerTimePoint) + Phase;
            } else {
                Result = 0.0;
            }
        } else {
            if (PhaseChanges.first.back().at(10) == PhaseChanges.second.back().at(10)) {
                double FirstDiscardTimePoint = 0.0;
                double FirstCommonTimePoint = std::prev(PhaseChanges.first.end(), 2)->at(0);

                std::size_t MinSize = std::min(PhaseChanges.first.size(), PhaseChanges.second.size());
                for (std::size_t i = 0; i != MinSize - 1; ++i) {
                    if (PhaseChanges.first.at(i).at(10) != PhaseChanges.second.at(i).at(10)) {
                        FirstDiscardTimePoint = PhaseChanges.first.at(i).at(0);
                        break;
                    }
                }

                double DeltaTimePoint = FirstCommonTimePoint - FirstDiscardTimePoint;
                std::prev(PhaseChanges.first.end(), 2)->at(0) -= DeltaTimePoint;
                PhaseChanges.first.back().at(0) -= DeltaTimePoint;
            }

            AlignArrays(PhaseChanges);

            Result = CalcEvolutionProgress(PhaseChanges, TargetAge, MassFactor);
            double IntegerPart = 0.0;
            double FractionalPart = std::modf(Result, &IntegerPart);
            if (FractionalPart > 0.99 && Result < 9.0 && IntegerPart >= std::prev(PhaseChanges.first.end(), 3)->at(10)) {
                Result = 9.0;
            }
        }
    }

    TimerEnd;

    return Result;
}

std::vector<double> StellarGenerator::InterpolateRows(const std::shared_ptr<MistData>& Data, double EvolutionProgress) {
    std::vector<double> Result;

    auto SurroundingRows = Data->FindSurroundingValues("x", EvolutionProgress);
    if (SurroundingRows.first != SurroundingRows.second) {
        int LowerTempPhase = static_cast<int>(SurroundingRows.first.at(10));
        int UpperTempPhase = static_cast<int>(SurroundingRows.second.at(10));
        if (LowerTempPhase != UpperTempPhase) {
            SurroundingRows.second.at(11) = LowerTempPhase + 1;
        }
        double ProgressFactor = (EvolutionProgress - SurroundingRows.first.at(11)) / (SurroundingRows.second.at(11) - SurroundingRows.first.at(11));
        Result = InterpolateFinalData(SurroundingRows, ProgressFactor);
    } else {
        Result = SurroundingRows.first;
    }

    return Result;
}

void StellarGenerator::AlignArrays(std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& Arrays) {
    TimerBegin;

    if (Arrays.first.back().at(10) != 9 && Arrays.second.back().at(10) != 9) {
        std::size_t MinSize = std::min(Arrays.first.size(), Arrays.second.size());
        Arrays.first.resize(MinSize);
        Arrays.second.resize(MinSize);
    } else if (Arrays.first.back().at(10) != 9 && Arrays.second.back().at(10) == 9) {
        Arrays.first.back().at(10) = 9;
        Arrays.first.back().at(11) = 9.0;
    } else if (Arrays.first.back().at(10) == 9 && Arrays.second.back().at(10) == 9) {
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

    TimerEnd;
}

std::vector<double> StellarGenerator::InterpolateArray(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Factor) {
    TimerBegin;

    if (DataArrays.first.size() != DataArrays.second.size()) {
        throw std::runtime_error("Data arrays size mismatch.");
    }

    std::size_t Size = DataArrays.first.size();
    std::vector<double> Result(Size);
    for (std::size_t i = 0; i != Size; ++i) {
        Result[i] = DataArrays.first[i] + (DataArrays.second[i] - DataArrays.first[i]) * Factor;
    }

    TimerEnd;

    return Result;
}

std::vector<double> StellarGenerator::InterpolateFinalData(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Factor) {
    TimerBegin;
    
    if (DataArrays.first.size() != DataArrays.second.size()) {
        throw std::runtime_error("Data arrays size mismatch.");
    }

    std::vector<double> Result = InterpolateArray(DataArrays, Factor);
    Result[10] = DataArrays.first[10];

    TimerEnd;

    return Result;
}

_MODULES_END
_NPGS_END
