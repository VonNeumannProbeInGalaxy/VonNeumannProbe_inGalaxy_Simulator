#pragma once

#include "StellarGenerator.h"

_NPGS_BEGIN
_MODULE_BEGIN

NPGS_INLINE StellarGenerator& StellarGenerator::SetLogMassSuggestDistribution(std::unique_ptr<Util::Distribution<>> Distribution)
{
	_LogMassGenerator = std::move(Distribution);
	return *this;
}

NPGS_INLINE StellarGenerator& StellarGenerator::SetUniverseAge(float Age)
{
	_UniverseAge = Age;
	return *this;
}

NPGS_INLINE StellarGenerator& StellarGenerator::SetAgeLowerLimit(float Limit)
{
	_AgeLowerLimit = Limit;
	return *this;
}

NPGS_INLINE StellarGenerator& StellarGenerator::SetAgeUpperLimit(float Limit)
{
	_AgeUpperLimit = Limit;
	return *this;
}

NPGS_INLINE StellarGenerator& StellarGenerator::SetFeHLowerLimit(float Limit)
{
	_FeHLowerLimit = Limit;
	return *this;
}

NPGS_INLINE StellarGenerator& StellarGenerator::SetFeHUpperLimit(float Limit)
{
	_FeHUpperLimit = Limit;
	return *this;
}

NPGS_INLINE StellarGenerator& StellarGenerator::SetMassLowerLimit(float Limit)
{
	_MassLowerLimit = Limit;
	return *this;
}

NPGS_INLINE StellarGenerator& StellarGenerator::SetMassUpperLimit(float Limit)
{
	_MassUpperLimit = Limit;
	return *this;
}

NPGS_INLINE StellarGenerator& StellarGenerator::SetCoilTempLimit(float Limit)
{
	_CoilTemperatureLimit = Limit;
	return *this;
}

NPGS_INLINE StellarGenerator& StellarGenerator::SetdEpdM(float dEpdM)
{
	_dEpdM = dEpdM;
	return *this;
}

NPGS_INLINE StellarGenerator& StellarGenerator::SetAgePdf(const std::function<float(const glm::vec3&, float, float)>& AgePdf)
{
	_AgePdf = AgePdf;
	return *this;
}

NPGS_INLINE StellarGenerator& StellarGenerator::SetAgeMaxPdf(const glm::vec2& MaxPdf)
{
	_AgeMaxPdf = MaxPdf;
	return *this;
}

NPGS_INLINE StellarGenerator& StellarGenerator::SetMassPdfs(const std::array<std::function<float(float)>, 2>& MassPdfs)
{
	_MassPdfs = MassPdfs;
	return *this;
}

NPGS_INLINE StellarGenerator& StellarGenerator::SetMassMaxPdfs(const std::array<glm::vec2, 2>& MaxPdfs)
{
	_MassMaxPdfs = MaxPdfs;
	return *this;
}

NPGS_INLINE StellarGenerator& StellarGenerator::SetAgeDistribution(GenerateDistribution Distribution)
{
	_AgeDistribution = Distribution;
	return *this;
}

NPGS_INLINE StellarGenerator& StellarGenerator::SetFeHDistribution(GenerateDistribution Distribution)
{
	_FeHDistribution = Distribution;
	return *this;
}

NPGS_INLINE StellarGenerator& StellarGenerator::SetMassDistribution(GenerateDistribution Distribution)
{
	_MassDistribution = Distribution;
	return *this;
}

NPGS_INLINE StellarGenerator& StellarGenerator::SetGenerateOption(GenerateOption Option)
{
	_Option = Option;
	return *this;
}

_NPGS_END
_MODULE_END
