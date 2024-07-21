#include "StellarGenerator.h"

#include <cmath>

_NPGS_BEGIN

namespace Modules {

StellarGenerator::StellarGenerator(int Seed) : _RandomEngine(Seed) {}

double StellarGenerator::ProbabilityDensity(double Mass) {
    // g1 = (0.158 / (log(10) * m)) * exp(-1.0 * pow(log10(1) - log10(0.08), 2.0) / 2 * pow(0.69, 2.0)), m <= 1Msun
    // g1 = n01 * pow(m, -2.35), m > 1Msun
    double g1 = 0.0;
    if (Mass <= 1.0) {
        g1 = (0.158 / (std::log(10) * Mass)) * std::exp(-1.0 * std::pow(std::log10(1) - std::log10(0.08), 2.0) / 2 * std::pow(0.69, 2.0));
    } else {
        double n01 = (0.158 / std::log(10)) * std::exp(-1.0 * std::pow(std::log10(1) - std::log10(0.08), 2.0) / 2 * std::pow(0.69, 2.0));
        g1 =  n01 * std::pow(Mass, -2.35);
    }

    return g1;
}

StellarGenerator::BasicProperties StellarGenerator::GenBasicProperties() {
    BasicProperties Properties;

    int PosX     = static_cast<int>(_UniformDistribution(_RandomEngine) * 1000);
    int PosY     = static_cast<int>(_UniformDistribution(_RandomEngine) * 1000);
    int PosZ     = static_cast<int>(_UniformDistribution(_RandomEngine) * 1000);
    int Distance = static_cast<int>(_UniformDistribution(_RandomEngine) * 10000);
    Properties.StarSys.Name = "S-" + std::to_string(PosX) + "-" + std::to_string(PosY) + "-" + std::to_string(PosZ) + "-" + std::to_string(Distance);
    Properties.StarSys.Position = glm::dvec3(PosX, PosY, PosZ);

    double Mass = GenMass(3.125);
    Properties.Mass = Mass;

    double Lifetime = pow(10, 10) * pow(Mass, -2.5);

    double Age = _UniformDistribution(_RandomEngine) * std::min(Lifetime, 3e12);
    Properties.Age = Age;

    double FeH = -1.5 + _UniformDistribution(_RandomEngine) * (0.5 - (-1.5));
    Properties.FeH = FeH;

    return Properties;
}

double StellarGenerator::GenMass(double MaxPdf) {
    double Mass = 0.0;
    double Probability = 0.0;
    do {
        Mass = 0.1 + _UniformDistribution(_RandomEngine) * (300.0 - 0.1);
        Probability = ProbabilityDensity(Mass);
    } while (_UniformDistribution(_RandomEngine) * MaxPdf > Probability);

    return Mass;
}

}

_NPGS_END
