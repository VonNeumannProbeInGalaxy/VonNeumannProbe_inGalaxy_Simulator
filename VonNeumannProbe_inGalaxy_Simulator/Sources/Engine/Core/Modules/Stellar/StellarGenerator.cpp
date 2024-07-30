#include "StellarGenerator.h"

#include <cmath>

#include <array>
#include <filesystem>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <sstream>
#include <utility>
#include <vector>

#include <boost/math/interpolators/barycentric_rational.hpp>

_NPGS_BEGIN

namespace Modules {

StellarGenerator::StellarGenerator(int Seed) : _RandomEngine(Seed) {}

AstroObject::Star StellarGenerator::GenStar() {
    BasicProperties BasicData = GenBasicProperties();
    std::vector<double> ActuallyData = GetActuallyData(static_cast<BasicProperties>(BasicData));
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
    Properties.StarSys.Name = "S-" + std::to_string(PosX) + "-" + std::to_string(PosY) + "-" + std::to_string(PosZ) + "-" + std::to_string(Distance);
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

std::vector<double> StellarGenerator::GetActuallyData(const BasicProperties& Properties) {
    std::array<double, 4> PresetFeH{ -1.5, -0.5, 0.0, 0.5 };
    double TargetAge  = Properties.Age;
    double TargetFeH  = Properties.FeH;
    double TargetMass = Properties.Mass;

    auto LowerFeH = std::lower_bound(PresetFeH.begin(), PresetFeH.end(), TargetFeH);
    auto UpperFeH = std::upper_bound(PresetFeH.begin(), PresetFeH.end(), TargetFeH);
    if (UpperFeH == PresetFeH.end()) {
        UpperFeH = LowerFeH;
    }

    if (LowerFeH == PresetFeH.end()) {
        throw std::out_of_range("FeH value out of range.");
    }

    std::ostringstream MassStream;
    MassStream << std::fixed << std::setprecision(2) << TargetMass;
    std::string MassStr = MassStream.str() + "0";

    std::vector<std::pair<std::string, std::string>> Files;
    for (double FeH : { *LowerFeH, *UpperFeH }) {
        std::ostringstream FeHStream;
        FeHStream << std::fixed << std::setprecision(1) << FeH;
        std::string FeHStr = FeHStream.str();
        if (FeH >= 0.0) {
            FeHStr.insert(FeHStr.begin(), '+');
        }
        FeHStr.insert(0, "Assets/Models/MESA/[Fe_H]=");

        std::vector<double> Masses;
        for (const auto& Entry : std::filesystem::directory_iterator(FeHStr)) {
            std::string Filename = Entry.path().filename().string();
            if (Filename.find(MassStr) != std::string::npos) {
                double Mass = std::stod(Filename.substr(0, Filename.find("Ms_track.csv")));
                Masses.push_back(Mass);
            }
        }

        std::sort(Masses.begin(), Masses.end());
        auto LowerMass = std::lower_bound(Masses.begin(), Masses.end(), TargetMass);
        auto UpperMass = std::upper_bound(Masses.begin(), Masses.end(), TargetMass);
        if (Masses.empty() && (LowerMass == Masses.end() || UpperMass == Masses.end())) {
            throw std::out_of_range("Mass value out of range.");
        }

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

        Files.emplace_back(LowerMassFile, UpperMassFile);
    }

    std::vector<std::vector<double>> InterpolatedData;

    for (const auto& FilePair : Files) {
        MesaData LowerData(FilePair.first,  "star_age", "star_mass", "star_mdot", "log_L", "log_Teff", "log_R", "log_surf_z", "v_wind_Km_per_s", "log_center_T", "log_center_Rho", "phase");
        MesaData UpperData(FilePair.second, "star_age", "star_mass", "star_mdot", "log_L", "log_Teff", "log_R", "log_surf_z", "v_wind_Km_per_s", "log_center_T", "log_center_Rho", "phase");

        auto LowerRows = LowerData.FindSurroundingValues("star_age", std::to_string(TargetAge));
        auto UpperRows = UpperData.FindSurroundingValues("star_age", std::to_string(TargetAge));

        if (LowerRows.first.empty() || LowerRows.second.empty() || UpperRows.first.empty() || UpperRows.second.empty()) {
            throw std::out_of_range("Age value out of range.");
        }

        std::vector<double> LowerLowerRow;
        std::vector<double> LowerUpperRow;
        std::vector<double> UpperLowerRow;
        std::vector<double> UpperUpperRow;
        for (const auto& Value : LowerRows.first) {
            LowerLowerRow.push_back(std::stod(Value));
        }
        for (const auto& Value : LowerRows.second) {
            LowerUpperRow.push_back(std::stod(Value));
        }
        for (const auto& Value : UpperRows.first) {
            UpperLowerRow.push_back(std::stod(Value));
        }
        for (const auto& Value : UpperRows.second) {
            UpperUpperRow.push_back(std::stod(Value));
        }

        std::vector<double> LowerRow = InterpolateArrays(LowerLowerRow, LowerUpperRow, TargetAge);
        std::vector<double> UpperRow = InterpolateArrays(UpperLowerRow, UpperUpperRow, TargetAge);
        std::vector<double> FinalRow = InterpolateArrays(LowerRow, UpperRow, TargetMass);
        InterpolatedData.push_back(FinalRow);
    }

    std::vector<double> FinalData = InterpolateArrays(InterpolatedData[0], InterpolatedData[1], TargetFeH);
    return FinalData;
}

std::vector<double> StellarGenerator::InterpolateArrays(const std::vector<double>& Lower, const std::vector<double>& Upper, double Target) {
    if (Lower.size() != Upper.size()) {
        throw std::invalid_argument("Row size mismatch.");
    }

    std::vector<double> Result(Lower.size());
    for (std::size_t i = 0; i < Lower.size(); ++i) {
        std::vector<double> x = { 0.0, 1.0 };
        std::vector<double> y = { Lower[i], Upper[i] };
        boost::math::interpolators::barycentric_rational<double> Interpolator(x.begin(), x.end(), y.begin());
        Result[i] = Interpolator(Target);
    }

    return Result;
}

}

_NPGS_END
