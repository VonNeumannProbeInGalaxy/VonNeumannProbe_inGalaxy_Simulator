#pragma once

#include <memory>
#include <mutex>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "Engine/Base/AstroObject/Star.h"
#include "Engine/Core/AssetLoader/AssetManager.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_MODULES_BEGIN

class StellarGenerator {
public:
    using BaryCenter = AstroObject::CelestialBody::BaryCenter;
    using MistData   = Assets::Csv<double, 12>;
    using HrDiagram  = Assets::Csv<double, 6>;

    struct BasicProperties {
        BaryCenter StarSys;
        double     Age;
        double     FeH;
        double     Mass;

        explicit operator AstroObject::Star() const {
            AstroObject::Star Star;
            Star.SetParentBody(StarSys);
            Star.SetMass(Mass);
            Star.SetAge(Age);
            Star.SetFeH(FeH);

            return Star;
        }
    };

public:
    StellarGenerator() = default;
    StellarGenerator(int Seed);
    ~StellarGenerator() = default;

public:
    BasicProperties GenBasicProperties();
    AstroObject::Star GenStar();
    AstroObject::Star GenStar(const BasicProperties& Properties);
    
private:
    template <typename CsvType>
    static std::shared_ptr<CsvType> LoadCsvAsset(const std::string& Filename, const std::vector<std::string>& Headers);

    double GenAge(double MaxPdf);
    double GenMass(double LowerLimit);
    std::vector<double> GetActuallyMistData(const BasicProperties& Properties);
    std::vector<double> InterpolateMistData(const std::pair<std::string, std::string>& Files, double TargetAge, double MassFactor);
    std::vector<std::vector<double>> FindPhaseChanges(const std::shared_ptr<MistData>& DataCsv);
    void GenSpectralType(AstroObject::Star& StarData);
    StellarClass::LuminosityClass CalcLuminosityClass(const AstroObject::Star& StarData);

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

private:
    std::mt19937 _RandomEngine;

    static const std::vector<std::string>                                                   _kMistHeaders;
    static const std::vector<std::string>                                                   _kHrDiagramHeaders;
    static const std::unordered_set<double>                                                 _kPresetFeH;
    static std::unordered_map<std::string, std::vector<double>>                             _MassFileCache;
    static std::unordered_map<std::shared_ptr<MistData>, std::vector<std::vector<double>>>  _PhaseChangesCache;
    static std::mutex                                                                       _CacheMutex;
};

_MODULES_END
_NPGS_END
