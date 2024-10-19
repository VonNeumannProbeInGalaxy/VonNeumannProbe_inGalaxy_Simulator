#pragma once

#include "Engine/Core/Base.h"

_NPGS_BEGIN
_MODULES_BEGIN

inline StellarGenerator& StellarGenerator::SetLogMassSuggestDistribution(UniformRealDistribution<>& Distribution) {
    _LogMassGenerator = Distribution;
    return *this;
}

inline StellarGenerator& StellarGenerator::SetUniverseAge(float Age) {
    _UniverseAge = Age;
    return *this;
}

inline StellarGenerator& StellarGenerator::SetAgeLowerLimit(float Limit) {
    _AgeLowerLimit = Limit;
    return *this;
}

inline StellarGenerator& StellarGenerator::SetAgeUpperLimit(float Limit) {
    _AgeUpperLimit = Limit;
    return *this;
}

inline StellarGenerator& StellarGenerator::SetFeHLowerLimit(float Limit) {
    _FeHLowerLimit = Limit;
    return *this;
}

inline StellarGenerator& StellarGenerator::SetFeHUpperLimit(float Limit) {
    _FeHUpperLimit = Limit;
    return *this;
}

inline StellarGenerator& StellarGenerator::SetMassLowerLimit(float Limit) {
    _MassLowerLimit = Limit;
    return *this;
}

inline StellarGenerator& StellarGenerator::SetMassUpperLimit(float Limit) {
    _MassUpperLimit = Limit;
    return *this;
}

inline StellarGenerator& StellarGenerator::SetCoilTempLimit(float Limit) {
    _CoilTempLimit = Limit;
    return *this;
}

inline StellarGenerator& StellarGenerator::SetdEpdM(float dEpdM) {
    _dEpdM = dEpdM;
    return *this;
}

inline StellarGenerator& StellarGenerator::SetAgePdf(const std::function<float(const glm::vec3&, float, float)>& AgePdf) {
    _AgePdf = AgePdf;
    return *this;
}

inline StellarGenerator& StellarGenerator::SetAgeMaxPdf(const glm::vec2& MaxPdf) {
    _AgeMaxPdf = MaxPdf;
    return *this;
}

inline StellarGenerator& StellarGenerator::SetMassPdfs(const std::array<std::function<float(float, std::function<float(float)>)>, 3>& MassPdfs) {
    _MassPdfs = MassPdfs;
    return *this;
}

inline StellarGenerator& StellarGenerator::SetMassMaxPdfs(const std::array<glm::vec2, 3>& MaxPdfs) {
    _MassMaxPdfs = MaxPdfs;
    return *this;
}

inline StellarGenerator& StellarGenerator::SetAgeDistribution(GenerateDistribution Distribution) {
    _AgeDistribution = Distribution;
    return *this;
}

inline StellarGenerator& StellarGenerator::SetFeHDistribution(GenerateDistribution Distribution) {
    _FeHDistribution = Distribution;
    return *this;
}

inline StellarGenerator& StellarGenerator::SetMassDistribution(GenerateDistribution Distribution) {
    _MassDistribution = Distribution;
    return *this;
}

inline StellarGenerator& StellarGenerator::SetGenerateOption(GenerateOption Option) {
    _Option = Option;
    return *this;
}

_NPGS_END
_MODULES_END
