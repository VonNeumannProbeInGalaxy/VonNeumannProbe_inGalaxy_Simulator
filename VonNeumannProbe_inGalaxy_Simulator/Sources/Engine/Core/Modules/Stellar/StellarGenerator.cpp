#include "StellarGenerator.h"

#include <cmath>

#include <array>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <sstream>
#include <utility>
#include <vector>

#include <boost/math/interpolators/barycentric_rational.hpp>

#include "Engine/Core/Logger.h"

_NPGS_BEGIN
_MODULES_BEGIN

StellarGenerator::StellarGenerator(int Seed) : _RandomEngine(Seed) {}

AstroObject::Star StellarGenerator::GenStar() {
    BasicProperties BasicData = GenBasicProperties();
    std::vector<double> ActuallyData = GetActuallyMesaData(static_cast<BasicProperties>(BasicData));
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

    double FeH = -2.0 + _UniformDistribution(_RandomEngine) * (0.5 - (-2.0));
    Properties.FeH = FeH;

    return Properties;
}

std::vector<double> StellarGenerator::GetActuallyMesaData(const BasicProperties& Properties) {
    std::array<double, 6> PresetFeH{ -2.0, -1.5, -1.0, -0.5, 0.0, 0.5 };
    double TargetAge  = Properties.Age;
    double TargetFeH  = Properties.FeH;
    double TargetMass = Properties.Mass;

    double ClosestFeH = *std::min_element(PresetFeH.begin(), PresetFeH.end(), [TargetFeH](double Lhs, double Rhs) -> bool {
        return std::abs(Lhs - TargetFeH) < std::abs(Rhs - TargetFeH);
    });

    TargetFeH = ClosestFeH;

    std::stringstream MassStream;
    MassStream << std::fixed << std::setprecision(2) << TargetMass;
    std::string MassStr = MassStream.str() + "0";

    std::pair<std::string, std::string> Files;
    std::stringstream FeHStream;
    FeHStream << std::fixed << std::setprecision(1) << TargetFeH;
    std::string FeHStr = FeHStream.str();
    if (TargetFeH >= 0.0) {
        FeHStr.insert(FeHStr.begin(), '+');
    }
    FeHStr.insert(0, "Assets/Models/MESA/[Fe_H]=");

    std::vector<double> Masses;
    for (const auto& Entry : std::filesystem::directory_iterator(FeHStr)) {
        std::string Filename = Entry.path().filename().string();
        double Mass = std::stod(Filename.substr(0, Filename.find("Ms_track.csv")));
        Masses.emplace_back(Mass);
    }

    auto LowerMass = std::lower_bound(Masses.begin(), Masses.end(), TargetMass);
    auto UpperMass = std::upper_bound(Masses.begin(), Masses.end(), TargetMass);

    if (LowerMass != Masses.begin() && LowerMass != Masses.end()) {
        --LowerMass;
    }

    if (LowerMass == Masses.end()) {
        throw std::out_of_range("Mass value out of range.");
    }

    double MassFactor = (TargetMass - *LowerMass) / (*UpperMass - *LowerMass);

    MassStr.clear();
    MassStream.str("");
    MassStream.clear();

    MassStream << std::fixed << std::setprecision(2) << *LowerMass;
    MassStr = MassStream.str() + "0";
    std::string LowerMassFile = FeHStr + "/" + MassStr + "Ms_track.csv";

    MassStr.clear();
    MassStream.str("");
    MassStream.clear();

    MassStream << std::fixed << std::setprecision(2) << *UpperMass;
    MassStr = MassStream.str() + "0";
    std::string UpperMassFile = FeHStr + "/" + MassStr + "Ms_track.csv";

    Files.first  = LowerMassFile;
    Files.second = UpperMassFile;

    std::vector<double> Result = InterpolateMesaData(Files, TargetAge, MassFactor);

    return Result;
}

std::vector<double> StellarGenerator::InterpolateMesaData(const std::pair<std::string, std::string>& Files, double TargetAge, double MassFactor) {
    std::vector<double> Result(11);
    
    if (Files.first != Files.second) [[likely]] {
        if (Assets::AssetManager::GetAsset<MesaData>(Files.first) == nullptr) {
            Assets::AssetManager::AddAsset<MesaData>(Files.first, std::make_shared<MesaData>(MesaData(Files.first, _MesaHeaders)));
        }

        if (Assets::AssetManager::GetAsset<MesaData>(Files.second) == nullptr) {
            Assets::AssetManager::AddAsset<MesaData>(Files.second, std::make_shared<MesaData>(MesaData(Files.second, _MesaHeaders)));
        }

        std::shared_ptr<MesaData> LowerData = Assets::AssetManager::GetAsset<MesaData>(Files.first);
        std::shared_ptr<MesaData> UpperData = Assets::AssetManager::GetAsset<MesaData>(Files.second);

        auto LowerPhaseChanges = FindPhaseChanges(LowerData);
        auto UpperPhaseChanges = FindPhaseChanges(UpperData);

        double LowerEvolutionProgress = CalcEvolutionProgress(LowerPhaseChanges, TargetAge);
        double UpperEvolutionProgress = CalcEvolutionProgress(UpperPhaseChanges, TargetAge);

        auto LowerSurroundingRows = LowerData->FindSurroundingValues("x", LowerEvolutionProgress);
        auto UpperSurroundingRows = UpperData->FindSurroundingValues("x", UpperEvolutionProgress);

        auto LowerRows = InterpolateArray(LowerSurroundingRows, MassFactor, true);
        auto UpperRows = InterpolateArray(UpperSurroundingRows, MassFactor, true);

        Result = InterpolateArray({ LowerRows, UpperRows }, MassFactor, false);
    } else [[unlikely]] {
        if (Assets::AssetManager::GetAsset<MesaData>(Files.first) == nullptr) {
            Assets::AssetManager::AddAsset<MesaData>(Files.first, std::make_shared<MesaData>(MesaData(Files.first, _MesaHeaders)));
        }

        std::shared_ptr<MesaData> Data = Assets::AssetManager::GetAsset<MesaData>(Files.first);
        try {
            auto SurroundingRows = Data->FindSurroundingValues("star_age", 1.77636786e+03);
            const auto& LowerRow = SurroundingRows.first;
            const auto& UpperRow = SurroundingRows.second;

            for (std::size_t i = 0; i != 10; ++i) {
                Result[i] = (LowerRow[i] + UpperRow[i]) / 2.0;
            }

            Result[10] = LowerRow[10];
        } catch (const std::out_of_range& e) {
            NpgsError("Error: " + std::string(e.what()));
        }
    }

    return Result;
}

std::vector<std::vector<double>> StellarGenerator::FindPhaseChanges(std::shared_ptr<MesaData> DataCsv) {
    std::vector<std::vector<double>> Result;

    auto* DataArray = DataCsv->Data();
    int CurrentPhase = -2;
    for (const auto& Row : *DataArray) {
        if (Row[10] != CurrentPhase) {
            CurrentPhase = static_cast<int>(Row[10]);
            Result.emplace_back(Row);
        }
    }

    return Result;
}

double StellarGenerator::CalcEvolutionProgress(const std::vector<std::vector<double>>& PhaseChanges, double TargetAge) {
    auto LowerTimePoint = std::lower_bound(PhaseChanges.begin(), PhaseChanges.end(), TargetAge,
        [](const std::vector<double>& Lhs, double Rhs) -> bool {
            return Lhs[0] < Rhs;
        }
    );

    auto UpperTimePoint = std::upper_bound(PhaseChanges.begin(), PhaseChanges.end(), TargetAge,
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

    return LowerTimePoint->at(10) + (TargetAge - LowerTimePoint->at(0)) / (UpperTimePoint->at(0) - TargetAge);
}

std::vector<double> StellarGenerator::InterpolateArray(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double MassFactor, bool bIsSameFile) {
    if (DataArrays.first.size() != DataArrays.second.size()) {
        throw std::runtime_error("Data arrays size mismatch.");
    }

    std::vector<double> Result(11);
    if (!bIsSameFile) {
        for (std::size_t i = 0; i != 10; ++i) {
            Result[i] = DataArrays.first[i] + (DataArrays.second[i] - DataArrays.first[i]) * MassFactor;
        }
    } else {
        for (std::size_t i = 0; i != 10; ++i) {
            Result[i] = (DataArrays.first[i] + DataArrays.second[i]) / 2.0;
        }
    }

    Result[10] = DataArrays.first[10];
    return Result;
}

_MODULES_END
_NPGS_END
