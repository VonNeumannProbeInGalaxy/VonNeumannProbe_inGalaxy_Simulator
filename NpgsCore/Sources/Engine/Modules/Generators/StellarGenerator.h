#pragma once

#include <array>
#include <functional>
#include <memory>
#include <random>
#include <shared_mutex>
#include <unordered_map>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

#include "Engine/AssetLoader/Csv.hpp"
#include "Engine/Base/Astro/Star.h"
#include "Engine/Core/Base.h"
#include "Engine/Utilities/Random.hpp"
#include "Engine/Utilities/StellarClass.h"

_NPGS_BEGIN
_MODULE_BEGIN

class NPGS_API StellarGenerator
{
public:
	using MistData   = Asset::Csv<double, 12>;
	using WdMistData = Asset::Csv<double, 5>;
	using HrDiagram  = Asset::Csv<double, 7>;

	enum class GenerateDistribution
	{
		kFromPdf,
		kUniform,
		kUniformByExponent
	};

	enum class GenerateOption
	{
		kNormal,
		kGiant,
		kDeathStar,
		kMergeStar,
		kBinaryFirstStar,
		kBinarySecondStar,
	};

	struct BasicProperties
	{
		float Age{};
		float FeH{};
		float InitialMassSol{};
		bool  bIsSingleStar{ true };

		// 用于保存生成选项，类的生成选项仅影响该属性。生成的恒星完整信息也将根据该属性决定。该选项用于防止多线程生成恒星时属性和生成器胡乱匹配
		GenerateOption MultiOption;
		GenerateOption TypeOption;

		explicit operator Astro::Star() const
		{
			Astro::Star Star;
			Star.SetAge(Age);
			Star.SetFeH(FeH);
			Star.SetInitialMass(InitialMassSol);
			Star.SetIsSingleStar(bIsSingleStar);

			return Star;
		}
	};

public:
	StellarGenerator() = delete;
	explicit StellarGenerator(const std::seed_seq& SeedSequence,
							  GenerateOption Option = GenerateOption::kNormal,
							  float UniverseAge = 1.38e10f,
							  float MassLowerLimit = 0.1f,
							  float MassUpperLimit = 300.0f,
							  GenerateDistribution MassDistribution = GenerateDistribution::kFromPdf,
							  float AgeLowerLimit = 0.0f,
							  float AgeUpperLimit = 1.26e10f,
							  GenerateDistribution AgeDistribution = GenerateDistribution::kFromPdf,
							  float FeHLowerLimit = -4.0f,
							  float FeHUpperLimit = 0.5f,
							  GenerateDistribution FeHDistribution = GenerateDistribution::kFromPdf,
							  float CoilTemperatureLimit = 1514.114f,
							  float dEpdM = 2e6f,
							  const std::function<float(const glm::vec3&, float, float)>& AgePdf = nullptr,
							  const glm::vec2& AgeMaxPdf = glm::vec2(),
							  const std::array<std::function<float(float)>, 2>& MassPdfs = { nullptr, nullptr },
							  const std::array<glm::vec2, 2>& MassMaxPdfs = { glm::vec2(), glm::vec2() });

	StellarGenerator(const StellarGenerator&) = delete;
	StellarGenerator(StellarGenerator&&) noexcept = default;
	~StellarGenerator() = default;

	StellarGenerator& operator=(const StellarGenerator&) = delete;
	StellarGenerator& operator=(StellarGenerator&&) noexcept = default;

	BasicProperties GenerateBasicProperties();
	BasicProperties GenerateBasicProperties(float Age, float FeH);
	Astro::Star GenerateStar();
	Astro::Star GenerateStar(BasicProperties& Properties);
	Astro::Star GenerateStar(BasicProperties&& Properties);

	StellarGenerator& SetLogMassSuggestDistribution(std::unique_ptr<Util::Distribution<>> Distribution);
	StellarGenerator& SetUniverseAge(float Age);
	StellarGenerator& SetAgeLowerLimit(float Limit);
	StellarGenerator& SetAgeUpperLimit(float Limit);
	StellarGenerator& SetFeHLowerLimit(float Limit);
	StellarGenerator& SetFeHUpperLimit(float Limit);
	StellarGenerator& SetMassLowerLimit(float Limit);
	StellarGenerator& SetMassUpperLimit(float Limit);
	StellarGenerator& SetCoilTempLimit(float Limit);
	StellarGenerator& SetdEpdM(float dEpdM);
	StellarGenerator& SetAgePdf(const std::function<float(const glm::vec3&, float, float)>& AgePdf);
	StellarGenerator& SetAgeMaxPdf(const glm::vec2& MaxPdf);
	StellarGenerator& SetMassPdfs(const std::array<std::function<float(float)>, 2>& MassPdfs);
	StellarGenerator& SetMassMaxPdfs(const std::array<glm::vec2, 2>& MaxPdfs);
	StellarGenerator& SetAgeDistribution(GenerateDistribution Distribution);
	StellarGenerator& SetFeHDistribution(GenerateDistribution Distribution);
	StellarGenerator& SetMassDistribution(GenerateDistribution Distribution);
	StellarGenerator& SetGenerateOption(GenerateOption Option);

private:
	void InitMistData();
	void InitPdfs();

	float GenerateAge(float MaxPdf);
	float GenerateMass(float MaxPdf, auto& LogMassPdf);
	std::vector<double> GetFullMistData(const BasicProperties& Properties, bool bIsWhiteDwarf, bool bIsSingleWhiteDwarf);
	std::vector<double> InterpolateMistData(const std::pair<std::string, std::string>& Files, double TargetAge, double TargetMass, double MassCoefficient);
	std::vector<std::vector<double>> FindPhaseChanges(const MistData* DataCsv);
	double CalculateEvolutionProgress(std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& PhaseChanges, double TargetAge, double MassCoefficient);
	std::pair<double, std::pair<double, double>> FindSurroundingTimePoints(const std::vector<std::vector<double>>& PhaseChanges, double TargetAge);
	std::pair<double, std::size_t> FindSurroundingTimePoints(const std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& PhaseChanges, double TargetAge, double MassCoefficient);
	void AlignArrays(std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& Arrays);
	std::vector<double> InterpolateHrDiagram(HrDiagram* Data, double BvColorIndex);
	std::vector<double> InterpolateStarData(MistData* Data, double EvolutionProgress);
	std::vector<double> InterpolateStarData(WdMistData* Data, double TargetAge);
	std::vector<double> InterpolateStarData(auto* Data, double Target, const std::string& Header, int Index, bool bIsWhiteDwarf);
	std::vector<double> InterpolateArray(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Coefficient);
	std::vector<double> InterpolateFinalData(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Coefficient, bool bIsWhiteDwarf);
	void CalculateSpectralType(float FeH, Astro::Star& StarData);
	Util::StellarClass::LuminosityClass CalculateLuminosityClass(const Astro::Star& StarData);
	void ProcessDeathStar(Astro::Star& DeathStar, GenerateOption Option = GenerateOption::kNormal);
	void GenerateMagnetic(Astro::Star& StarData);
	void GenerateSpin(Astro::Star& StarData);
	void ExpandMistData(double TargetMass, std::vector<double>& StarData);

	template <typename CsvType>
	static CsvType* LoadCsvAsset(const std::string& Filename, const std::vector<std::string>& Headers);

public:
	static const int _kStarAgeIndex;
	static const int _kStarMassIndex;
	static const int _kStarMdotIndex;
	static const int _kLogTeffIndex;
	static const int _kLogRIndex;
	static const int _kLogSurfZIndex;
	static const int _kSurfaceH1Index;
	static const int _kSurfaceHe3Index;
	static const int _kLogCenterTIndex;
	static const int _kLogCenterRhoIndex;
	static const int _kPhaseIndex;
	static const int _kXIndex;
	static const int _kLifetimeIndex;

	static const int _kWdStarAgeIndex;
	static const int _kWdLogRIndex;
	static const int _kWdLogTeffIndex;
	static const int _kWdLogCenterTIndex;
	static const int _kWdLogCenterRhoIndex;

private:
	std::mt19937                                         _RandomEngine;
	std::array<Util::UniformRealDistribution<>, 8>       _MagneticGenerators;
	std::array<std::unique_ptr<Util::Distribution<>>, 4> _FeHGenerators;
	std::array<Util::UniformRealDistribution<>, 2>       _SpinGenerators;
	std::unique_ptr<Util::Distribution<>>                _LogMassGenerator;
	Util::UniformRealDistribution<>                      _AgeGenerator;
	Util::UniformRealDistribution<>                      _CommonGenerator;

	std::array<std::function<float(float)>, 2>     _MassPdfs;
	std::array<glm::vec2, 2>                       _MassMaxPdfs;

	std::function<float(glm::vec3, float, float)>  _AgePdf;
	glm::vec2                                      _AgeMaxPdf;

	float _UniverseAge;
	float _AgeLowerLimit;
	float _AgeUpperLimit;
	float _FeHLowerLimit;
	float _FeHUpperLimit;
	float _MassLowerLimit;
	float _MassUpperLimit;
	float _CoilTemperatureLimit;
	float _dEpdM;

	GenerateDistribution _AgeDistribution;
	GenerateDistribution _FeHDistribution;
	GenerateDistribution _MassDistribution;
	GenerateOption       _Option;

	static const std::vector<std::string> _kMistHeaders;
	static const std::vector<std::string> _kWdMistHeaders;
	static const std::vector<std::string> _kHrDiagramHeaders;
	static std::unordered_map<std::string, std::vector<float>> _kMassFileCache;
	static std::unordered_map<const MistData*, std::vector<std::vector<double>>> _kPhaseChangesCache;
	static std::shared_mutex _kCacheMutex;
	static bool _kbMistDataInitiated;
};

_MODULE_END
_NPGS_END

#include "StellarGenerator.inl"
