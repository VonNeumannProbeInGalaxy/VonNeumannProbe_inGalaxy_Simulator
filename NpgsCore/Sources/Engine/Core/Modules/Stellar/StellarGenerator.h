﻿#pragma once

#include <memory>
#include <random>
#include <shared_mutex>
#include <unordered_map>
#include <utility>

#include "Engine/Base/AstroObject/Star.h"
#include "Engine/Core/AssetLoader/AssetManager.h"
#include "Engine/Core/Base.h"
#include "Engine/Core/Random.hpp"

_NPGS_BEGIN
_MODULES_BEGIN

class NPGS_API StellarGenerator {
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
    StellarGenerator(int Seed, double MassLowerLimit, double AgeLowerLimit = 0.0, double AgeUpperLimit = 1.26e10, double FeHLowerLimit = -4.0, double FeHUpperLimit = 0.5);
    ~StellarGenerator() = default;

public:
    BasicProperties GenBasicProperties();
    AstroObject::Star GenerateStar();
    AstroObject::Star GenerateStar(const BasicProperties& Properties);
    
private:
    template <typename CsvType>
    static std::shared_ptr<CsvType> LoadCsvAsset(const std::string& Filename, const std::vector<std::string>& Headers);

    double GenerateAge(double MaxPdf);
    double GenerateMass(double MaxPdf, bool bIsBinary);
    std::vector<double> GetActuallyMistData(const BasicProperties& Properties);
    std::vector<double> InterpolateMistData(const std::pair<std::string, std::string>& Files, double TargetAge, double TargetMass, double MassFactor);
    std::vector<std::vector<double>> FindPhaseChanges(const std::shared_ptr<MistData>& DataCsv);
    void CalcSpectralType(AstroObject::Star& StarData, double SurfaceH1);
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
    std::mt19937                                 _RandomEngine;
    UniformRealDistribution                      _LogMassGenerator;
    UniformRealDistribution                      _AgeGenerator;
    UniformRealDistribution                      _CommonGenerator;
    std::array<std::shared_ptr<Distribution>, 4> _FeHGenerators;

    double _FeHLowerLimit;
    double _FeHUpperLimit;

    static const std::vector<std::string>                                                   _kMistHeaders;
    static const std::vector<std::string>                                                   _kHrDiagramHeaders;
    static std::unordered_map<std::string, std::vector<double>>                             _MassFileCache;
    static std::unordered_map<std::shared_ptr<MistData>, std::vector<std::vector<double>>>  _PhaseChangesCache;
    static std::shared_mutex                                                                _CacheMutex;
};

_MODULES_END
_NPGS_END