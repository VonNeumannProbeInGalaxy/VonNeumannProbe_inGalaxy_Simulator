#include "StellarGenerator.h"

#include <cmath>

#include <algorithm>
#include <filesystem>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#define ENABLE_LOGGER
#include "Engine/Core/Constants.h"
#include "Engine/Core/Logger.h"

_NPGS_BEGIN
_MODULES_BEGIN

// Tool macros
// -----------
#define GenerateDeathStarPlaceholder(Lifetime) {                                                                                                                                                                     \
    StellarClass::SpectralType DeathStarClass{ StellarClass::SpectralClass::kSpectral_Unknown, 0, false, StellarClass::SpectralClass::kSpectral_Unknown, 0, StellarClass::LuminosityClass::kLuminosity_Unknown, 0 }; \
    AstroObject::Star DeathStar;                                                                                                                                                                                     \
    DeathStar.SetStellarClass(StellarClass(StellarClass::StarType::kDeathStarPlaceholder, DeathStarClass));                                                                                                          \
    DeathStar.SetLifetime(Lifetime);                                                                                                                                                                                 \
    throw DeathStar;                                                                                                                                                                                                 \
}

// Tool functions
// --------------
static double DefaultAgePdf(double Age);
static double DefaultLogMassPdf(double Mass, bool bIsBinary);
static double CalcEvolutionProgress(std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& PhaseChanges, double TargetAge, double MassFactor);
static std::pair<double, std::pair<double, double>> FindSurroundingTimePoints(const std::vector<std::vector<double>>& PhaseChanges, double TargetAge);
static std::pair<double, std::size_t> FindSurroundingTimePoints(const std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& PhaseChanges, double TargetAge, double MassFactor);
static void AlignArrays(std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& Arrays);
static std::vector<double> InterpolateRows(const std::shared_ptr<StellarGenerator::HrDiagram>& Data, double BvColorIndex);
static std::vector<double> InterpolateRows(const std::shared_ptr<StellarGenerator::MistData>& Data, double EvolutionProgress);
static std::vector<double> InterpolateRows(const std::shared_ptr<StellarGenerator::WdMistData>& Data, double TargetAge);
static std::vector<double> InterpolateRows(const auto& Data, double Target, const std::string& Header, int Index, bool bIsWhiteDwarf);
static std::vector<double> InterpolateArray(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Factor);
static std::vector<double> InterpolateFinalData(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Factor, bool bIsWhiteDwarf);
static void ExpandMistData(std::vector<double>& StarData, double TargetMass);

// StellarGenerator implementations
// --------------------------------
StellarGenerator::StellarGenerator(int Seed, double MassLowerLimit, double MassUpperLimit, double AgeLowerLimit, double AgeUpperLimit, double FeHLowerLimit, double FeHUpperLimit) :
    _RandomEngine(Seed),
    _LogMassGenerator(std::log10(MassLowerLimit), std::log10(MassUpperLimit)),
    _AgeGenerator(AgeLowerLimit, AgeUpperLimit),
    _CommonGenerator(0.0, 1.0),
    _FeHGenerators({
        std::make_shared<LogNormalDistribution>(-0.3, 0.5),
        std::make_shared<NormalDistribution>(-0.3, 0.15),
        std::make_shared<NormalDistribution>(-0.08, 0.12),
        std::make_shared<NormalDistribution>(0.05, 0.16)
    }),
    _FeHLowerLimit(FeHLowerLimit), _FeHUpperLimit(FeHUpperLimit)
{
    InitMistData();
}

StellarGenerator::BasicProperties StellarGenerator::GenBasicProperties() {
    BasicProperties Properties;

    double PosX     = _CommonGenerator.Generate(_RandomEngine) * 1000.0;
    double PosY     = _CommonGenerator.Generate(_RandomEngine) * 1000.0;
    double PosZ     = _CommonGenerator.Generate(_RandomEngine) * 1000.0;
    double Distance = _CommonGenerator.Generate(_RandomEngine) * 10000.0;
    Properties.StarSys.Name     = "S-" + std::to_string(PosX) + "-" + std::to_string(PosY) + "-" + std::to_string(PosZ) + "-" + std::to_string(Distance);
    Properties.StarSys.Position = glm::dvec3(PosX, PosY, PosZ);
    Properties.StarSys.Distance = Distance;

    double Mass = GenerateMass(0.086, true);
    Properties.Mass = Mass;

    double Age = GenerateAge(2.6);
    Properties.Age = Age;

    std::shared_ptr<Distribution> FeHGenerator = nullptr;

    double FeHLowerLimit = _FeHLowerLimit;
    double FeHUpperLimit = _FeHUpperLimit;

    if (Age > 8e9) {
        FeHGenerator = _FeHGenerators[0];
        FeHLowerLimit = -_FeHUpperLimit;
        FeHUpperLimit = -_FeHLowerLimit;
    } else if (Age > 6e9) {
        FeHGenerator = _FeHGenerators[1];
    } else if (Age > 4e9) {
        FeHGenerator = _FeHGenerators[2];
    } else {
        FeHGenerator = _FeHGenerators[3];
    }

    double FeH = 0.0;
    do {
        FeH = FeHGenerator->Generate(_RandomEngine);
    } while (FeH > FeHUpperLimit || FeH < FeHLowerLimit);

    if (Age > 8e9) {
        FeH *= -1.0;
    }

    Properties.FeH = FeH;

    return Properties;
}

AstroObject::Star StellarGenerator::GenerateStar() {
    BasicProperties BasicData = GenBasicProperties();
    return GenerateStar(BasicData);
}

AstroObject::Star StellarGenerator::GenerateStar(const BasicProperties& Properties) {
    AstroObject::Star Star(Properties);
    std::vector<double> StarData;

    try {
        StarData = GetActuallyMistData(Properties, false, true);
    } catch (AstroObject::Star& DeathStar) {
        DeathStar.SetAge(Properties.Age);
        DeathStar.SetFeH(Properties.FeH);
        DeathStar.SetMass(Properties.Mass);
        ProcessDeathStar(DeathStar);
        if (DeathStar.GetEvolutionPhase() == AstroObject::Star::Phase::kNull) {
            DeathStar = GenerateStar();
        }

        return DeathStar;
    }

    if (StarData.empty()) {
        return {};
    }

    double Age               = StarData[_kStarAgeIndex];
    double MassSol           = StarData[_kStarMassIndex];
    double RadiusSol         = std::pow(10.0, StarData[_kLogRIndex]);
    double Teff              = std::pow(10.0, StarData[_kLogTeffIndex]);
    double SurfaceFeH        = std::pow(10.0, StarData[_kLogSurfZIndex]);
    double SurfaceH1         = StarData[_kSurfaceH1Index];
    double SurfaceHe3        = StarData[_kSurfaceHe3Index];
    double CoreTemp          = std::pow(10.0, StarData[_kLogCenterTIndex]);
    double CoreDensity       = std::pow(10.0, StarData[_kLogCenterRhoIndex]);
    double MassLossRate      = StarData[_kStarMdotIndex];
    double EvolutionProgress = StarData[_kXIndex];
    double Lifetime          = StarData[_kLifetimeIndex];

    double LuminositySol     = std::pow(RadiusSol, 2.0) * std::pow((Teff / kSolarTeff), 4.0);
    double AbsoluteMagnitude = kSolarAbsoluteMagnitude - 2.5 * std::log10(LuminositySol);
    double EscapeVelocity    = std::sqrt((2 * kGravityConstant * MassSol * kSolarMass) / (RadiusSol * kSolarRadius));

    double LifeProgress      = Age / Lifetime;
    double WindSpeedFactor   = 3.0 - LifeProgress;
    double StellarWindSpeed  = WindSpeedFactor * EscapeVelocity;

    double SurfaceEnergeticNuclide = (SurfaceH1 * 0.00002 + SurfaceHe3);
    double SurfaceVolatiles        = 1.0 - SurfaceFeH - SurfaceEnergeticNuclide;

    AstroObject::Star::Phase EvolutionPhase = static_cast<AstroObject::Star::Phase>(StarData[_kPhaseIndex]);

    Star.SetAge(Age);
    Star.SetMass(MassSol * kSolarMass);
    Star.SetRadius(RadiusSol * kSolarRadius);
    Star.SetEscapeVelocity(EscapeVelocity);
    Star.SetLuminosity(LuminositySol * kSolarLuminosity);
    Star.SetAbsoluteMagnitude(AbsoluteMagnitude);
    Star.SetTeff(Teff);
    Star.SetSurfaceFeH(SurfaceFeH);
    Star.SetSurfaceEnergeticNuclide(SurfaceEnergeticNuclide);
    Star.SetSurfaceVolatiles(SurfaceVolatiles);
    Star.SetCoreTemp(CoreTemp);
    Star.SetCoreDensity(CoreDensity * 1000);
    Star.SetStellarWindSpeed(StellarWindSpeed);
    Star.SetStellarWindMassLossRate(-(MassLossRate * kSolarMass / kYearInSeconds));
    Star.SetEvolutionProgress(EvolutionProgress);
    Star.SetEvolutionPhase(EvolutionPhase);
    Star.SetLifetime(Lifetime);

    CalcSpectralType(Star, SurfaceH1);
    GenerateMagnetic(Star);
    GenerateSpin(Star);

    return Star;
}

template<typename CsvType>
std::shared_ptr<CsvType> StellarGenerator::LoadCsvAsset(const std::string& Filename, const std::vector<std::string>& Headers) {
    {
        std::shared_lock Lock(_kCacheMutex);
        auto Asset = Assets::AssetManager::GetAsset<CsvType>(Filename);
        if (Asset != nullptr) {
            return Asset;
        }
    }

    std::unique_lock Lock(_kCacheMutex);
    auto CsvAsset = std::make_shared<CsvType>(Filename, Headers);
    Assets::AssetManager::AddAsset<CsvType>(Filename, CsvAsset);

    return CsvAsset;
}

void StellarGenerator::InitMistData() {
    if (_kbMistDataInitiated) {
        return;
    }

    const std::array<std::string, 10> kPresetPrefix{
        "Assets/Models/MIST/[Fe_H]=-4.0", 
        "Assets/Models/MIST/[Fe_H]=-3.0",
        "Assets/Models/MIST/[Fe_H]=-2.0",
        "Assets/Models/MIST/[Fe_H]=-1.5",
        "Assets/Models/MIST/[Fe_H]=-1.0",
        "Assets/Models/MIST/[Fe_H]=-0.5",
        "Assets/Models/MIST/[Fe_H]=+0.0",
        "Assets/Models/MIST/[Fe_H]=+0.5",
        "Assets/Models/MIST/WhiteDwarfs/Thin",
        "Assets/Models/MIST/WhiteDwarfs/Thick"
    };

    std::vector<double> Masses;

    for (const auto& PrefixDir : kPresetPrefix) {
        for (const auto& Entry : std::filesystem::directory_iterator(PrefixDir)) {
            std::string Filename = Entry.path().filename().string();
            double Mass = std::stod(Filename.substr(0, Filename.find("Ms_track.csv")));
            Masses.emplace_back(Mass);

            if (PrefixDir.find("WhiteDwarfs") != std::string::npos) {
                LoadCsvAsset<WdMistData>(PrefixDir + "/" + Filename, _kWdMistHeaders);
            } else {
                LoadCsvAsset<MistData>(PrefixDir + "/" + Filename, _kMistHeaders);
            }
        }

        _kMassFileCache.emplace(PrefixDir, Masses);
        Masses.clear();
    }

    _kbMistDataInitiated = true;
}

double StellarGenerator::GenerateAge(double MaxPdf) {
    double Age         = 0.0;
    double Probability = 0.0;
    do {
        Age = _AgeGenerator.Generate(_RandomEngine);
        Probability = DefaultAgePdf(Age / 1e9);
    } while (_CommonGenerator.Generate(_RandomEngine) * MaxPdf > Probability);

    return Age;
}

double StellarGenerator::GenerateMass(double MaxPdf, bool bIsBinary) {
    double LogMass     = 0.0;
    double Probability = 0.0;
    do {
        LogMass = _LogMassGenerator.Generate(_RandomEngine);
        Probability = DefaultLogMassPdf(LogMass, bIsBinary);
    } while (_CommonGenerator.Generate(_RandomEngine) * MaxPdf > Probability);

    return std::pow(10.0, LogMass);
}

std::vector<double> StellarGenerator::GetActuallyMistData(const BasicProperties& Properties, bool bIsWhiteDwarf, bool bIsSingleWd) {
    double TargetAge  = Properties.Age;
    double TargetFeH  = Properties.FeH;
    double TargetMass = Properties.Mass;

    std::string PrefixDir;
    std::string MassStr;
    std::stringstream MassStream;
    std::pair<std::string, std::string> Files;

    if (!bIsWhiteDwarf) {
        const std::array<double, 8> kPresetFeH{ -4.0, -3.0, -2.0, -1.5, -1.0, -0.5, 0.0, 0.5 };

        double ClosestFeH = *std::min_element(kPresetFeH.begin(), kPresetFeH.end(), [TargetFeH](double Lhs, double Rhs) -> bool {
            return std::abs(Lhs - TargetFeH) < std::abs(Rhs - TargetFeH);
        });

        TargetFeH = ClosestFeH;

        MassStream << std::fixed << std::setfill('0') << std::setw(6) << std::setprecision(2) << TargetMass;
        MassStr = MassStream.str() + "0";

        std::stringstream FeHStream;
        FeHStream << std::fixed << std::setprecision(1) << TargetFeH;
        PrefixDir = FeHStream.str();
        if (TargetFeH >= 0.0) {
            PrefixDir.insert(PrefixDir.begin(), '+');
        }
        PrefixDir.insert(0, "Assets/Models/MIST/[Fe_H]=");
    } else {
        if (bIsSingleWd) {
            PrefixDir = "Assets/Models/MIST/WhiteDwarfs/Thin";
        } else {
            PrefixDir = "Assets/Models/MIST/WhiteDwarfs/Thick";
        }
    }

    std::vector<double> Masses;
    {
        std::shared_lock Lock(_kCacheMutex);
        Masses = _kMassFileCache[PrefixDir];
    }

    auto it = std::lower_bound(Masses.begin(), Masses.end(), TargetMass);
    if (it == Masses.end()) {
        if (!bIsWhiteDwarf) {
            throw std::out_of_range("Mass value out of range.");
        } else {
            it = std::prev(Masses.end(), 1);
        }
    }

    double LowerMass = 0;
    double UpperMass = 0;

    if (*it == TargetMass) {
        LowerMass = UpperMass = *it;
    } else {
        LowerMass = it == Masses.begin() ? *it : *(it - 1);
        UpperMass = *it;
    }

    double MassFactor = (TargetMass - LowerMass) / (UpperMass - LowerMass);

    MassStr.clear();
    MassStream.str("");
    MassStream.clear();

    MassStream << std::fixed << std::setfill('0') << std::setw(6) << std::setprecision(2) << LowerMass;
    MassStr = MassStream.str() + "0";
    std::string LowerMassFile = PrefixDir + "/" + MassStr + "Ms_track.csv";

    MassStr.clear();
    MassStream.str("");
    MassStream.clear();

    MassStream << std::fixed << std::setfill('0') << std::setw(6) << std::setprecision(2) << UpperMass;
    MassStr = MassStream.str() + "0";
    std::string UpperMassFile = PrefixDir + "/" + MassStr + "Ms_track.csv";

    Files.first = LowerMassFile;
    Files.second = UpperMassFile;

    std::vector<double> Result = InterpolateMistData(Files, TargetAge, TargetMass, MassFactor);

    return Result;
}

std::vector<double> StellarGenerator::InterpolateMistData(const std::pair<std::string, std::string>& Files, double TargetAge, double TargetMass, double MassFactor) {
    std::vector<double> Result;

    if (Files.first.find("WhiteDwarfs") == std::string::npos) {
        if (Files.first != Files.second) [[likely]] {
            std::shared_ptr<MistData> LowerData = LoadCsvAsset<MistData>(Files.first, _kMistHeaders);
            std::shared_ptr<MistData> UpperData = LoadCsvAsset<MistData>(Files.second, _kMistHeaders);

            auto LowerPhaseChanges = FindPhaseChanges(LowerData);
            auto UpperPhaseChanges = FindPhaseChanges(UpperData);

            std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>> PhaseChangePair{ LowerPhaseChanges, UpperPhaseChanges };
            double EvolutionProgress = CalcEvolutionProgress(PhaseChangePair, TargetAge, MassFactor);

            double LowerLifetime = PhaseChangePair.first.back()[_kStarAgeIndex];
            double UpperLifetime = PhaseChangePair.second.back()[_kStarAgeIndex];

            std::vector<double> LowerRows = InterpolateRows(LowerData, EvolutionProgress);
            std::vector<double> UpperRows = InterpolateRows(UpperData, EvolutionProgress);

            LowerRows.emplace_back(LowerLifetime);
            UpperRows.emplace_back(UpperLifetime);

            Result = InterpolateFinalData({ LowerRows, UpperRows }, MassFactor, false);
        } else [[unlikely]] {
            std::shared_ptr<MistData> StarData = LoadCsvAsset<MistData>(Files.first, _kMistHeaders);
            auto   PhaseChanges = FindPhaseChanges(StarData);
            double EvolutionProgress = 0.0;
            double Lifetime = 0.0;
            if (TargetMass >= 0.1) {
                std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>> PhaseChangePair{ PhaseChanges, {} };
                EvolutionProgress = CalcEvolutionProgress(PhaseChangePair, TargetAge, MassFactor);
                Lifetime = PhaseChanges.back()[_kStarAgeIndex];
                Result = InterpolateRows(StarData, EvolutionProgress);
                Result.emplace_back(Lifetime);
            } else {
                double OriginalLowerPhaseChangePoint = PhaseChanges[1][_kStarAgeIndex];
                double OriginalUpperPhaseChangePoint = PhaseChanges[2][_kStarAgeIndex];
                double LowerPhaseChangePoint = OriginalLowerPhaseChangePoint * std::pow(TargetMass / 0.1, -1.3);
                double UpperPhaseChangePoint = OriginalUpperPhaseChangePoint * std::pow(TargetMass / 0.1, -1.3);
                Lifetime = UpperPhaseChangePoint;
                if (TargetAge < LowerPhaseChangePoint) {
                    EvolutionProgress = TargetAge / LowerPhaseChangePoint - 1;
                } else if (LowerPhaseChangePoint <= TargetAge && TargetAge <= UpperPhaseChangePoint) {
                    EvolutionProgress = (TargetAge - LowerPhaseChangePoint) / (UpperPhaseChangePoint - LowerPhaseChangePoint);
                } else if (TargetAge > UpperPhaseChangePoint) {
                    GenerateDeathStarPlaceholder(Lifetime);
                }

                Result = InterpolateRows(StarData, EvolutionProgress);
                Result.emplace_back(Lifetime);
                ExpandMistData(Result, TargetMass);
            }
        }
    } else {
        if (Files.first != Files.second) [[likely]] {
            std::shared_ptr<WdMistData> LowerData = LoadCsvAsset<WdMistData>(Files.first, _kWdMistHeaders);
            std::shared_ptr<WdMistData> UpperData = LoadCsvAsset<WdMistData>(Files.second, _kWdMistHeaders);

            std::vector<double> LowerRows = InterpolateRows(LowerData, TargetAge);
            std::vector<double> UpperRows = InterpolateRows(UpperData, TargetAge);

            Result = InterpolateFinalData({ LowerRows, UpperRows }, MassFactor, true);
        } else [[unlikely]] {
            std::shared_ptr<WdMistData> StarData = LoadCsvAsset<WdMistData>(Files.first, _kWdMistHeaders);
            Result = InterpolateRows(StarData, TargetAge);
        }
    }

    return Result;
}

std::vector<std::vector<double>> StellarGenerator::FindPhaseChanges(const std::shared_ptr<MistData>& DataCsv) {
    std::vector<std::vector<double>> Result;

    _kCacheMutex.lock();
    if (_kPhaseChangesCache.contains(DataCsv)) {
        Result = _kPhaseChangesCache[DataCsv];
    } else {
        _kCacheMutex.unlock();
        auto* DataArray = DataCsv->Data();
        int CurrentPhase = -2;
        for (const auto& Row : *DataArray) {
            if (Row[_kPhaseIndex] != CurrentPhase || Row[_kXIndex] == 10.0) {
                CurrentPhase = static_cast<int>(Row[_kPhaseIndex]);
                Result.emplace_back(Row);
            }
        }

        _kCacheMutex.lock();
        if (!_kPhaseChangesCache.contains(DataCsv)) {
            _kPhaseChangesCache.emplace(DataCsv, Result);
        }
    }
    _kCacheMutex.unlock();

    return Result;
}

void StellarGenerator::CalcSpectralType(AstroObject::Star& StarData, double SurfaceH1) {
    double Teff = StarData.GetTeff();
    auto EvolutionPhase = StarData.GetEvolutionPhase();

    StellarClass::StarType StarType = StarData.GetStellarClass().GetStarType();
    StellarClass::SpectralType SpectralType{};
    SpectralType.bIsAmStar = false;

    std::vector<std::pair<int, int>> SpectralSubclassMap;
    double Subclass = 0.0;

    std::function<void(AstroObject::Star::Phase, double)> CalcSpectralSubclass = [&](AstroObject::Star::Phase Phase, double SurfaceH1) -> void {
        std::uint32_t SpectralClass = Phase == AstroObject::Star::Phase::kWolfRayet ? 11 : 0;

        if (Phase != AstroObject::Star::Phase::kWolfRayet) {
            if (Phase == AstroObject::Star::Phase::kMainSequence) {
                if (SurfaceH1 < 0.6) {
                    EvolutionPhase = AstroObject::Star::Phase::kWolfRayet;
                    CalcSpectralSubclass(EvolutionPhase, SurfaceH1);
                    return;
                }
            }

            const auto& InitialMap = AstroObject::Star::_kInitialCommonMap;
            for (auto it = InitialMap.begin(); it != InitialMap.end() - 1; ++it) {
                ++SpectralClass;
                if (it->first >= Teff && (it + 1)->first < Teff) {
                    SpectralSubclassMap = it->second;
                    break;
                }
            }
        } else {
            if (Teff >= 200000) {
                SpectralType.HSpectralClass = StellarClass::SpectralClass::kSpectral_WO;
                SpectralType.Subclass = 2;
                return;
            } else {
                if (SurfaceH1 >= 0.2) {
                    SpectralSubclassMap = AstroObject::Star::_kSpectralSubclassMap_WNxh;
                    SpectralClass = 13;
                    SpectralType.SpecialMark = static_cast<std::uint32_t>(StellarClass::SpecialPeculiarities::kCode_h);
                } else if (SurfaceH1 >= 0.1) {
                    SpectralSubclassMap = AstroObject::Star::_kSpectralSubclassMap_WN;
                    SpectralClass = 13;
                } else if (SurfaceH1 < 0.1 && SurfaceH1 > 0.05) {
                    SpectralSubclassMap = AstroObject::Star::_kSpectralSubclassMap_WC;
                    SpectralClass = 12;
                } else {
                    SpectralSubclassMap = AstroObject::Star::_kSpectralSubclassMap_WO;
                    SpectralClass = 14;
                }
            }
        }

        SpectralType.HSpectralClass = static_cast<StellarClass::SpectralClass>(SpectralClass);

        if (SpectralSubclassMap.empty()) {
            std::println("Age: {}, FeH: {}, Mass: {}, Teff: {}", StarData.GetAge(), StarData.GetFeH(), StarData.GetMass() / kSolarMass, StarData.GetTeff());
        }

        for (auto it = SpectralSubclassMap.begin(); it != SpectralSubclassMap.end() - 1; ++it) {
            if (it->first >= Teff && (it + 1)->first < Teff) {
                Subclass = it->second;
                break;
            }
        }

        if (SpectralType.HSpectralClass == StellarClass::SpectralClass::kSpectral_WN &&
            SpectralType.SpecialMark & static_cast<std::uint32_t>(StellarClass::SpecialPeculiarities::kCode_h)) {
            if (Subclass < 5) {
                Subclass = 5;
            }
        }

        SpectralType.Subclass = Subclass;
    };

    if (EvolutionPhase != AstroObject::Star::Phase::kWolfRayet) {
        switch (StarType) {
        case StellarClass::StarType::kNormalStar: {
            if (Teff < 54000) {
                CalcSpectralSubclass(EvolutionPhase, SurfaceH1);

                if (EvolutionPhase != AstroObject::Star::Phase::kWolfRayet) {
                    if (EvolutionPhase == AstroObject::Star::Phase::kPrevMainSequence) {
                        SpectralType.LuminosityClass = StellarClass::LuminosityClass::kLuminosity_V;
                    } else if (EvolutionPhase == AstroObject::Star::Phase::kMainSequence) {
                        if (SpectralType.HSpectralClass == StellarClass::SpectralClass::kSpectral_O && SurfaceH1 < 0.6) {
                            SpectralType.LuminosityClass = CalcLuminosityClass(StarData);
                        } else {
                            SpectralType.LuminosityClass = StellarClass::LuminosityClass::kLuminosity_V;
                        }
                    } else {
                        SpectralType.LuminosityClass = CalcLuminosityClass(StarData);
                    }
                } else {
                    SpectralType.LuminosityClass = StellarClass::LuminosityClass::kLuminosity_Unknown;
                }
            } else {
                if (SurfaceH1 > 0.6) {
                    SpectralType.HSpectralClass = StellarClass::SpectralClass::kSpectral_O;
                    SpectralType.Subclass = 2;
                    SpectralType.LuminosityClass = StellarClass::LuminosityClass::kLuminosity_V;
                } else {
                    CalcSpectralSubclass(AstroObject::Star::Phase::kWolfRayet, SurfaceH1);
                }
            }

            break;
        }
        case StellarClass::StarType::kWhiteDwarf: {
            double MassSol = StarData.GetMass() / kSolarMass;

            if (Teff >= 12000) {
                if (MassSol <= 0.5) {
                    SpectralType.HSpectralClass = StellarClass::SpectralClass::kSpectral_DA;
                } else {
                    if (Teff > 45000) {
                        SpectralType.HSpectralClass = StellarClass::SpectralClass::kSpectral_DO;
                    } else {
                        SpectralType.HSpectralClass = StellarClass::SpectralClass::kSpectral_DB;
                    }
                }
            } else {
                SpectralType.HSpectralClass = StellarClass::SpectralClass::kSpectral_DC;
            }

            Subclass = 50400.0 / Teff;
            if (Subclass > 9.5) {
                Subclass = 9.5;
            }

            SpectralType.Subclass = std::round(Subclass * 2.0) / 2.0;

            break;
        }
        case StellarClass::StarType::kNeutronStar: {
            SpectralType.HSpectralClass = StellarClass::SpectralClass::kSpectral_Q;
            break;
        }
        case StellarClass::StarType::kBlackHole: {
            SpectralType.HSpectralClass = StellarClass::SpectralClass::kSpectral_X;
            break;
        }
        case StellarClass::StarType::kDeathStarPlaceholder: {
            SpectralType.HSpectralClass = StellarClass::SpectralClass::kSpectral_Unknown;
            break;
        }
        default: {
            SpectralType.HSpectralClass = StellarClass::SpectralClass::kSpectral_Unknown;
            break;
        }
        }
    } else {
        CalcSpectralSubclass(AstroObject::Star::Phase::kWolfRayet, SurfaceH1);
        SpectralType.LuminosityClass = StellarClass::LuminosityClass::kLuminosity_Unknown;
    }

    StellarClass Class(StarType, SpectralType);
    StarData.SetStellarClass(Class);
}

StellarClass::LuminosityClass StellarGenerator::CalcLuminosityClass(const AstroObject::Star& StarData) {
    double MassLossRateSolPerYear = StarData.GetStellarWindMassLossRate() * kYearInSeconds / kSolarMass;
    double MassSol = StarData.GetMass() / kSolarMass;
    if (MassLossRateSolPerYear > 1e-4 && MassSol >= 15) {
        return StellarClass::LuminosityClass::kLuminosity_IaPlus;
    }

    double LuminositySol = StarData.GetLuminosity() / kSolarLuminosity;
    if (LuminositySol > 650000) {
        return StellarClass::LuminosityClass::kLuminosity_0;
    }

    std::shared_ptr<HrDiagram> HrDiagramData = LoadCsvAsset<HrDiagram>("Assets/Models/H-R Diagram/H-R Diagram.csv", _kHrDiagramHeaders);

    double Teff = StarData.GetTeff();
    double BvColorIndex = 0.0;
    if (std::log10(Teff) < 3.691) {
        BvColorIndex = -3.684 * std::log10(Teff) + 14.551;
    } else {
        BvColorIndex = 0.344 * std::pow(std::log10(Teff), 2) - 3.402 * std::log10(Teff) + 8.037;
    }

    if (BvColorIndex < -0.3 || BvColorIndex > 1.8363636363636362) {
        if (LuminositySol > 100000) {
            return StellarClass::LuminosityClass::kLuminosity_Ia;
        } else if (LuminositySol > 50000) {
            return StellarClass::LuminosityClass::kLuminosity_Iab;
        } else if (LuminositySol > 10000) {
            return StellarClass::LuminosityClass::kLuminosity_Ib;
        } else if (LuminositySol > 1000) {
            return StellarClass::LuminosityClass::kLuminosity_II;
        } else if (LuminositySol > 100) {
            return StellarClass::LuminosityClass::kLuminosity_III;
        } else if (LuminositySol > 10) {
            return StellarClass::LuminosityClass::kLuminosity_IV;
        } else {
            return StellarClass::LuminosityClass::kLuminosity_V;
        }
    }

    std::vector<double> LuminosityData = InterpolateRows(HrDiagramData, BvColorIndex);
    if (LuminositySol > LuminosityData[1]) {
        return StellarClass::LuminosityClass::kLuminosity_Ia;
    }

    double ClosestValue = *std::min_element(LuminosityData.begin() + 1, LuminosityData.end(), [LuminositySol](double Lhs, double Rhs) -> bool {
        return std::abs(Lhs - LuminositySol) < std::abs(Rhs - LuminositySol);
    });

    while (LuminosityData.size() != 6) {
        LuminosityData.emplace_back(-1);
    }

    StellarClass::LuminosityClass LuminosityClass = StellarClass::LuminosityClass::kLuminosity_Unknown;
    if (LuminositySol <= LuminosityData[1] && (ClosestValue == LuminosityData[1] || ClosestValue == LuminosityData[2])) {
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_Iab;
    } else if (LuminositySol < LuminosityData[2]) {
        if (ClosestValue == LuminosityData[2]) {
            LuminosityClass = StellarClass::LuminosityClass::kLuminosity_Ib;
        } else if (ClosestValue == LuminosityData[3]) {
            LuminosityClass = StellarClass::LuminosityClass::kLuminosity_II;
        } else if (ClosestValue == LuminosityData[4]) {
            LuminosityClass = StellarClass::LuminosityClass::kLuminosity_III;
        } else if (ClosestValue == LuminosityData[5]) {
            LuminosityClass = StellarClass::LuminosityClass::kLuminosity_IV;
        }
    }

    return LuminosityClass;
}

void StellarGenerator::ProcessDeathStar(AstroObject::Star& DeathStar) {
    double InputAge  = DeathStar.GetAge();
    double InputFeH  = DeathStar.GetFeH();
    double InputMass = DeathStar.GetMass();

    AstroObject::Star::Phase   EvolutionPhase{};
    AstroObject::Star::Ending  EvolutionEnding{};
    StellarClass::StarType     DeathStarType{};
    StellarClass::SpectralType DeathStarClass{};

    double DeathStarAge  = InputAge - DeathStar.GetLifetime();
    double DeathStarMass = 0.0;

    if (InputFeH <= -2.0 && InputMass >= 140 && InputMass < 250) {
        EvolutionPhase  = AstroObject::Star::Phase::kNull;
        EvolutionEnding = AstroObject::Star::Ending::kPairInstabilitySupernova;
        DeathStarType   = StellarClass::StarType::kDeathStarPlaceholder;
        DeathStarClass  = { StellarClass::SpectralClass::kSpectral_Unknown, 0, false, StellarClass::SpectralClass::kSpectral_Unknown, 0, StellarClass::LuminosityClass::kLuminosity_Unknown, 0 };
    } else if (InputFeH <= -2.0 && InputMass >= 250) {
        EvolutionPhase  = AstroObject::Star::Phase::kBlackHole;
        EvolutionEnding = AstroObject::Star::Ending::kPhotondisintegration;
        DeathStarType   = StellarClass::StarType::kBlackHole;
        DeathStarClass  = { StellarClass::SpectralClass::kSpectral_X, 0, false, StellarClass::SpectralClass::kSpectral_Unknown, 0, StellarClass::LuminosityClass::kLuminosity_Unknown, 0 };
        DeathStarMass   = 0.5 * InputMass;
    } else {
        if (InputMass > -0.75 && InputMass < 0.8) {
            DeathStarMass = (0.9795 - 0.393 * InputMass) * InputMass;
        } else if (InputMass >= 0.8 && InputMass < 7.9) {
            DeathStarMass = -0.00012336 * std::pow(InputMass, 6) + 0.003160 * std::pow(InputMass, 5) - 0.02960 * std::pow(InputMass, 4) + 0.12350 * std::pow(InputMass, 3) - 0.21550 * std::pow(InputMass, 2) + 0.19022 * InputMass + 0.46575;
        } else if (InputMass >= 7.9 && InputMass < 10.0) {
            DeathStarMass = 1.301 + 0.008095 * InputMass;
        } else if (InputMass >= 10.0 && InputMass < 21.0) {
            DeathStarMass = 1.246 + 0.0136 * InputMass;
        } else if (InputMass >= 21.0 && InputMass < 23.3537) {
            DeathStarMass = std::pow(10, (1.334 - 0.009987 * InputMass));
        } else if (InputMass >= 23.3537 && InputMass < 33.75) {
            DeathStarMass = 12.1 - 0.763 * InputMass + 0.0137 * std::pow(InputMass, 2.0);
        } else if (InputMass >= 33.75 && InputMass < 40) {
            DeathStarMass = std::pow(10, (0.882 + 0.0105 * InputMass));
        } else {
            DeathStarMass = 0.5 * InputMass;
        }

        if (InputMass >= 0.075 && InputMass < 0.5) {
            EvolutionPhase  = AstroObject::Star::Phase::kHeliumWhiteDwarf;
            EvolutionEnding = AstroObject::Star::Ending::kSlowColdingDown;
            DeathStarType   = StellarClass::StarType::kWhiteDwarf;
            DeathStarClass  = { StellarClass::SpectralClass::kSpectral_Unknown, 0, false, StellarClass::SpectralClass::kSpectral_Unknown, 0, StellarClass::LuminosityClass::kLuminosity_Unknown, 0 };
        } else if (InputMass >= 0.5 && InputMass < 8.0) {
            EvolutionPhase  = AstroObject::Star::Phase::kCarbonOxygenWhiteDwarf;
            EvolutionEnding = AstroObject::Star::Ending::kEnvelopeDisperse;
            DeathStarType   = StellarClass::StarType::kWhiteDwarf;
            DeathStarClass  = { StellarClass::SpectralClass::kSpectral_Unknown, 0, false, StellarClass::SpectralClass::kSpectral_Unknown, 0, StellarClass::LuminosityClass::kLuminosity_Unknown, 0 };
        } else if (InputMass >= 8.0 && InputMass < 9.759) {
            EvolutionPhase  = AstroObject::Star::Phase::kOxygenNeonMagnWhiteDwarf;
            EvolutionEnding = AstroObject::Star::Ending::kEnvelopeDisperse;
            DeathStarType   = StellarClass::StarType::kWhiteDwarf;
            DeathStarClass  = { StellarClass::SpectralClass::kSpectral_Unknown, 0, false, StellarClass::SpectralClass::kSpectral_Unknown, 0, StellarClass::LuminosityClass::kLuminosity_Unknown, 0 };
        } else if (InputMass >= 9.759 && InputMass < 10.0) {
            EvolutionPhase  = AstroObject::Star::Phase::kNeutronStar;
            EvolutionEnding = AstroObject::Star::Ending::kElectronCaptureSupernova;
            DeathStarType   = StellarClass::StarType::kNeutronStar;
            DeathStarClass  = { StellarClass::SpectralClass::kSpectral_Q, 0, false, StellarClass::SpectralClass::kSpectral_Unknown, 0, StellarClass::LuminosityClass::kLuminosity_Unknown, 0 };
        } else if (InputMass >= 10.0 && InputMass < 21.0) {
            EvolutionPhase  = AstroObject::Star::Phase::kNeutronStar;
            EvolutionEnding = AstroObject::Star::Ending::kIronCoreCollapseSupernova;
            DeathStarType   = StellarClass::StarType::kNeutronStar;
            DeathStarClass  = { StellarClass::SpectralClass::kSpectral_Q, 0, false, StellarClass::SpectralClass::kSpectral_Unknown, 0, StellarClass::LuminosityClass::kLuminosity_Unknown, 0 };
        } else if (InputMass >= 21.0 && InputMass < 23.3537) {
            EvolutionPhase  = AstroObject::Star::Phase::kBlackHole;
            EvolutionEnding = AstroObject::Star::Ending::kIronCoreCollapseSupernova;
            DeathStarType   = StellarClass::StarType::kBlackHole;
            DeathStarClass  = { StellarClass::SpectralClass::kSpectral_X, 0, false, StellarClass::SpectralClass::kSpectral_Unknown, 0, StellarClass::LuminosityClass::kLuminosity_Unknown, 0 };
        } else if (InputMass >= 23.3537 && InputMass < 33.75) {
            EvolutionPhase  = AstroObject::Star::Phase::kNeutronStar;
            EvolutionEnding = AstroObject::Star::Ending::kIronCoreCollapseSupernova;
            DeathStarType   = StellarClass::StarType::kNeutronStar;
            DeathStarClass  = { StellarClass::SpectralClass::kSpectral_Q, 0, false, StellarClass::SpectralClass::kSpectral_Unknown, 0, StellarClass::LuminosityClass::kLuminosity_Unknown, 0 };
        } else {
            EvolutionPhase  = AstroObject::Star::Phase::kBlackHole;
            EvolutionEnding = AstroObject::Star::Ending::kRelativisticJetHypernova;
            DeathStarType   = StellarClass::StarType::kBlackHole;
            DeathStarClass  = { StellarClass::SpectralClass::kSpectral_Q, 0, false, StellarClass::SpectralClass::kSpectral_Unknown, 0, StellarClass::LuminosityClass::kLuminosity_Unknown, 0 };
        }
    }

    double StarAge                 = 0.0;
    double LogR                    = 0.0;
    double LogTeff                 = 0.0;
    double LogCenterT              = 0.0;
    double LogCenterRho            = 0.0;

    double SurfaceFeH              = 0.0;
    double SurfaceEnergeticNuclide = 0.0;
    double SurfaceVolatiles        = 0.0;

    switch (DeathStarType) {
    case StellarClass::StarType::kWhiteDwarf: {
        std::vector<double> WhiteDwarfData = GetActuallyMistData({ {}, DeathStarAge, 0.0, DeathStarMass }, true, true);

        StarAge      = WhiteDwarfData[_kWdStarAgeIndex];
        LogR         = WhiteDwarfData[_kWdLogRIndex];
        LogTeff      = WhiteDwarfData[_kWdLogTeffIndex];
        LogCenterT   = WhiteDwarfData[_kWdLogCenterTIndex];
        LogCenterRho = WhiteDwarfData[_kWdLogCenterRhoIndex];

        if (DeathStarMass < 0.2 || DeathStarMass > 1.3) {
            LogR         = std::log10(0.0323 - 0.021384 * DeathStarMass);
            LogCenterT   = std::numeric_limits<double>::min();
            LogCenterRho = std::numeric_limits<double>::min();
        }

        if (DeathStarAge > StarAge) {
            double T1  = std::pow(10.0, LogTeff);
            LogTeff    = std::log10(T1 * std::pow((20 * StarAge) / (DeathStarAge + 19 * StarAge), 7.0 / 4.0));
            LogCenterT = std::numeric_limits<double>::min();
        }

        SurfaceFeH              = 0.0;
        SurfaceEnergeticNuclide = 0.0;
        SurfaceVolatiles        = 1.0;

        break;
    }
    case StellarClass::StarType::kNeutronStar: {
        if (DeathStarAge < 1e5) {
            DeathStarAge += 1e5;
        }

        double Radius = 0.0;
        if (DeathStarMass <= 0.77711) {
            Radius = -4.783 + 2.565 / DeathStarMass + 42 * DeathStarMass - 55.4 * std::pow(DeathStarMass, 2) + 34.93 * std::pow(DeathStarMass, 3) - 8.4 * std::pow(DeathStarMass, 4);
        } else if (DeathStarMass <= 2.0181) {
            Radius = 11.302 - 0.35184 * DeathStarMass;
        } else {
            Radius = -31951.1 + 63121.8 * DeathStarMass - 46717.8 * std::pow(DeathStarMass, 2) + 15358.4 * std::pow(DeathStarMass, 3) - 1892.365 * std::pow(DeathStarMass, 4);
        }

        double RadiusSol = Radius * 1000 / kSolarRadius;
        LogR = std::log10(RadiusSol);

        double Teff = 1.5e8 * std::pow((DeathStarAge - 1e5) + 22000, -0.5);
        LogTeff = std::log10(Teff);

        SurfaceFeH              = std::numeric_limits<double>::quiet_NaN();
        SurfaceEnergeticNuclide = std::numeric_limits<double>::quiet_NaN();
        SurfaceVolatiles        = std::numeric_limits<double>::quiet_NaN();

        break;
    }
    case StellarClass::StarType::kBlackHole: {
        LogR                    = std::numeric_limits<double>::quiet_NaN();
        LogTeff                 = std::numeric_limits<double>::quiet_NaN();
        LogCenterT              = std::numeric_limits<double>::quiet_NaN();
        LogCenterRho            = std::numeric_limits<double>::quiet_NaN();
        SurfaceFeH              = std::numeric_limits<double>::quiet_NaN();
        SurfaceEnergeticNuclide = std::numeric_limits<double>::quiet_NaN();
        SurfaceVolatiles        = std::numeric_limits<double>::quiet_NaN();

        break;
    }
    default: {
        break;
    }
    }

    double Age               = DeathStarAge;
    double MassSol           = DeathStarMass;
    double RadiusSol         = std::pow(10.0, LogR);
    double Teff              = std::pow(10.0, LogTeff);
    double CoreTemp          = std::pow(10.0, LogCenterT);
    double CoreDensity       = std::pow(10.0, LogCenterRho);
    double EvolutionProgress = 11.0;

    double LuminositySol     = std::pow(RadiusSol, 2.0) * std::pow((Teff / kSolarTeff), 4.0);
    double AbsoluteMagnitude = kSolarAbsoluteMagnitude - 2.5 * std::log10(LuminositySol);
    double EscapeVelocity    = std::sqrt((2 * kGravityConstant * MassSol * kSolarMass) / (RadiusSol * kSolarRadius));

    DeathStar.SetAge(Age);
    DeathStar.SetMass(MassSol * kSolarMass);
    DeathStar.SetRadius(RadiusSol * kSolarRadius);
    DeathStar.SetEscapeVelocity(EscapeVelocity);
    DeathStar.SetLuminosity(LuminositySol * kSolarLuminosity);
    DeathStar.SetAbsoluteMagnitude(AbsoluteMagnitude);
    DeathStar.SetTeff(Teff);
    DeathStar.SetSurfaceFeH(SurfaceFeH);
    DeathStar.SetSurfaceEnergeticNuclide(SurfaceEnergeticNuclide);
    DeathStar.SetSurfaceVolatiles(SurfaceVolatiles);
    DeathStar.SetCoreTemp(CoreTemp);
    DeathStar.SetCoreDensity(CoreDensity * 1000);
    DeathStar.SetEvolutionProgress(EvolutionProgress);
    DeathStar.SetEvolutionPhase(EvolutionPhase);
    DeathStar.SetLifetime(std::numeric_limits<double>::max());
    DeathStar.SetEvolutionEnding(EvolutionEnding);
    DeathStar.SetStellarClass(StellarClass(DeathStarType, DeathStarClass));

    CalcSpectralType(DeathStar, 0.0);
    GenerateMagnetic(DeathStar);
    GenerateSpin(DeathStar);
}

void StellarGenerator::GenerateMagnetic(AstroObject::Star& StarData) {
    std::unique_ptr<Distribution> MagneticGenerator = nullptr;

    StellarClass::StarType StarType = StarData.GetStellarClass().GetStarType();
    double MassSol = StarData.GetMass() / kSolarMass;
    AstroObject::Star::Phase EvolutionPhase = StarData.GetEvolutionPhase();

    double MagneticField = 0.0;

    switch (StarType) {
    case StellarClass::StarType::kNormalStar: {
        if (MassSol >= 0.075 && MassSol < 0.33) {
            MagneticGenerator = std::make_unique<UniformRealDistribution>(500.0, 3000.0);
        } else if (MassSol >= 0.33 && MassSol < 0.6) {
            MagneticGenerator = std::make_unique<UniformRealDistribution>(100.0, 1000.0);
        } else if (MassSol >= 0.6 && MassSol < 1.5) {
            MagneticGenerator = std::make_unique<UniformRealDistribution>(1.0, 10.0);
        } else if (MassSol >= 1.5 && MassSol < 20) {
            auto SpectralType = StarData.GetStellarClass().Data();
            if (EvolutionPhase == AstroObject::Star::Phase::kMainSequence &&
                (SpectralType.HSpectralClass == StellarClass::SpectralClass::kSpectral_A ||
                 SpectralType.HSpectralClass == StellarClass::SpectralClass::kSpectral_B)) {
                BernoulliDistribution ProbabilityGenerator(0.15);
                if (ProbabilityGenerator.Generate(_RandomEngine)) {
                    MagneticGenerator = std::make_unique<UniformRealDistribution>(1000.0, 10000.0);
                    SpectralType.SpecialMark |= static_cast<std::uint32_t>(StellarClass::SpecialPeculiarities::kCode_p);
                    StarData.SetStellarClass(StellarClass(StellarClass::StarType::kNormalStar, SpectralType));
                } else {
                    MagneticGenerator = std::make_unique<UniformRealDistribution>(0.1, 1.0);
                }
            } else {
                MagneticGenerator = std::make_unique<UniformRealDistribution>(0.1, 1.0);
            }
        } else {
            MagneticGenerator = std::make_unique<UniformRealDistribution>(100.0, 1000.0);
        }

        MagneticField = MagneticGenerator->Generate(_RandomEngine) / 10000;

        break;
    }
    case StellarClass::StarType::kWhiteDwarf: {
        MagneticGenerator = std::make_unique<UniformRealDistribution>(0.5, 4.5);
        MagneticField     = std::pow(10.0, MagneticGenerator->Generate(_RandomEngine));
        break;
    }
    case StellarClass::StarType::kNeutronStar: {
        MagneticGenerator = std::make_unique<UniformRealDistribution>(1e9, 1e11);
        double B0         = MagneticGenerator->Generate(_RandomEngine);
        double B          = B0 / (std::pow((0.034 * StarData.GetAge() / 1e4), 1.17) + 0.84);
        MagneticField     = B;
        break;
    }
    case StellarClass::StarType::kBlackHole: {
        MagneticField = 0.0;
        break;
    }
    case StellarClass::StarType::kDeathStarPlaceholder: {
        break;
    }
    default: {
        break;
    }
    }

    StarData.SetMagneticField(MagneticField);
}

void StellarGenerator::GenerateSpin(AstroObject::Star& StarData) {
    std::unique_ptr<Distribution> SpinGenerator = nullptr;

    StellarClass::StarType StarType = StarData.GetStellarClass().GetStarType();
    double StarAge   = StarData.GetAge();
    double MassSol   = StarData.GetMass() / kSolarMass;
    double RadiusSol = StarData.GetRadius() / kSolarRadius;
    double Spin      = 0.0;

    switch (StarType) {
    case StellarClass::StarType::kNormalStar: {
        double Base = 1.0;
        if (StarData.GetStellarClass().Data().SpecialMark & static_cast<std::uint32_t>(StellarClass::SpecialPeculiarities::kCode_p)) {
            UniformRealDistribution Dist(2.0, 9.0);
            Base = Dist.Generate(_RandomEngine);
        }

        double LgMass = std::log10(MassSol);
        double Term1  = std::pow(10, 4.81438 + 0.27978 * std::exp(LgMass) - 1.21782 * LgMass + 0.21678 * std::pow(LgMass, 2));
        double Term2  = 0.0;
        double Term3  = std::pow(2, std::sqrt(Base * StarAge * 1e-9));
        if (MassSol <= 1.4) {
            Term2 = std::pow(RadiusSol / std::pow(MassSol, 0.9), 1.5);
        } else {
            Term2 = std::pow(RadiusSol / (1.1062 * std::pow(MassSol, 0.6)), 1.5);
        }

        Spin = Term1 * Term2 * Term3;

        break;
    }
    case StellarClass::StarType::kWhiteDwarf: {
        SpinGenerator = std::make_unique<UniformRealDistribution>(3.0, 5.0);
        Spin = std::pow(10, SpinGenerator->Generate(_RandomEngine));
        break;
    }
    case StellarClass::StarType::kNeutronStar: {
        Spin = (StarAge * 3 * 1e-9) + 1e-3;
        break;
    }
    case StellarClass::StarType::kBlackHole: {
        SpinGenerator = std::make_unique<UniformRealDistribution>(0.001, 0.998);
        Spin = SpinGenerator->Generate(_RandomEngine);
        break;
    }
    default: {
        break;
    }
    }

    StarData.SetSpin(Spin);
}

const int StellarGenerator::_kStarAgeIndex        = 0;
const int StellarGenerator::_kStarMassIndex       = 1;
const int StellarGenerator::_kStarMdotIndex       = 2;
const int StellarGenerator::_kLogTeffIndex        = 3;
const int StellarGenerator::_kLogRIndex           = 4;
const int StellarGenerator::_kLogSurfZIndex       = 5;
const int StellarGenerator::_kSurfaceH1Index      = 6;
const int StellarGenerator::_kSurfaceHe3Index     = 7;
const int StellarGenerator::_kLogCenterTIndex     = 8;
const int StellarGenerator::_kLogCenterRhoIndex   = 9;
const int StellarGenerator::_kPhaseIndex          = 10;
const int StellarGenerator::_kXIndex              = 11;
const int StellarGenerator::_kLifetimeIndex       = 12;

const int StellarGenerator::_kWdStarAgeIndex      = 0;
const int StellarGenerator::_kWdLogRIndex         = 1;
const int StellarGenerator::_kWdLogTeffIndex      = 2;
const int StellarGenerator::_kWdLogCenterTIndex   = 3;
const int StellarGenerator::_kWdLogCenterRhoIndex = 4;

const std::vector<std::string> StellarGenerator::_kMistHeaders{ "star_age", "star_mass", "star_mdot", "log_Teff", "log_R", "log_surf_z", "surface_h1", "surface_he3", "log_center_T", "log_center_Rho", "phase", "x" };
const std::vector<std::string> StellarGenerator::_kWdMistHeaders{ "star_age", "log_R", "log_Teff", "log_center_T", "log_center_Rho" };
const std::vector<std::string> StellarGenerator::_kHrDiagramHeaders{ "B-V", "Ia", "Ib", "II", "III", "IV" };
bool StellarGenerator::_kbMistDataInitiated = false;
std::unordered_map<std::string, std::vector<double>> StellarGenerator::_kMassFileCache;
std::unordered_map<std::shared_ptr<StellarGenerator::MistData>, std::vector<std::vector<double>>> StellarGenerator::_kPhaseChangesCache;
std::shared_mutex StellarGenerator::_kCacheMutex;

// Tool functions implementations
// ------------------------------
double DefaultAgePdf(double Fuck) {
    double pt = 0.0;
    if (Fuck < 8.0) {
        pt = std::exp(Fuck / 8.4);
    } else {
        pt = 2.6 * std::exp((-0.5 * std::pow(Fuck - 8.0, 2.0)) / (std::pow(1.5, 2.0)));
    }

    return pt;
}

static double DefaultLogMassPdf(double Fuck, bool bIsBinary) {
    double g = 0.0;
    if (!bIsBinary) {
        if (std::pow(10.0, Fuck) <= 1.0) {
            g = (0.158 / (std::pow(10.0, Fuck) * std::log(10.0))) * std::exp(-1.0 * (std::pow(Fuck - std::log10(0.08), 2.0)) / (2.0 * std::pow(0.69, 2.0))) * std::pow(10.0, Fuck) * std::log(10.0);
        } else {
            double n01 = 0.0193937;
            g = n01 * std::pow(std::pow(10.0, Fuck), -2.35) * std::pow(10.0, Fuck) * std::log(10.0);
        }
    } else {
        if (std::pow(10.0, Fuck) <= 1.0) {
            g = (0.086 / (std::pow(10.0, Fuck) * std::log(10.0))) * std::exp(-1.0 * (std::pow(Fuck - std::log10(0.22), 2.0)) / (2.0 * std::pow(0.57, 2.0))) * std::pow(10.0, Fuck) * std::log(10.0);
        } else {
            double n02 = 0.0191992;
            g = n02 * std::pow(std::pow(10.0, Fuck), -2.35) * std::pow(10.0, Fuck) * std::log(10.0);
        }
    }

    return g;
}

double CalcEvolutionProgress(std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& PhaseChanges, double TargetAge, double MassFactor) {
    double Result = 0.0;
    double Phase  = 0.0;

    if (PhaseChanges.second.empty()) [[unlikely]] {
        const auto& TimePointResults = FindSurroundingTimePoints(PhaseChanges.first, TargetAge);
        Phase = TimePointResults.first;
        const auto& TimePoints = TimePointResults.second;
        if (TargetAge > TimePoints.second) {
            GenerateDeathStarPlaceholder(TimePoints.second);
        }

        Result = (TargetAge - TimePoints.first) / (TimePoints.second - TimePoints.first) + Phase;
    } else [[likely]] {
        if (PhaseChanges.first.size() == PhaseChanges.second.size() && (*std::prev(PhaseChanges.first.end(), 2))[StellarGenerator::_kPhaseIndex] == (*std::prev(PhaseChanges.second.end(), 2))[StellarGenerator::_kPhaseIndex]) {
            const auto& TimePointResults = FindSurroundingTimePoints(PhaseChanges, TargetAge, MassFactor);

            Phase = TimePointResults.first;
            std::size_t Index = TimePointResults.second;

            if (Index + 1 != PhaseChanges.first.size()) {
                std::pair<double, double> LowerTimePoints = { PhaseChanges.first[Index][StellarGenerator::_kStarAgeIndex], PhaseChanges.first[Index + 1][StellarGenerator::_kStarAgeIndex] };
                std::pair<double, double> UpperTimePoints = { PhaseChanges.second[Index][StellarGenerator::_kStarAgeIndex], PhaseChanges.second[Index + 1][StellarGenerator::_kStarAgeIndex] };

                const auto& [LowerLowerTimePoint, LowerUpperTimePoint] = LowerTimePoints;
                const auto& [UpperLowerTimePoint, UpperUpperTimePoint] = UpperTimePoints;

                double LowerTimePoint = LowerLowerTimePoint + (UpperLowerTimePoint - LowerLowerTimePoint) * MassFactor;
                double UpperTimePoint = LowerUpperTimePoint + (UpperUpperTimePoint - LowerUpperTimePoint) * MassFactor;

                Result = (TargetAge - LowerTimePoint) / (UpperTimePoint - LowerTimePoint) + Phase;

                if (Result > PhaseChanges.first.back()[StellarGenerator::_kPhaseIndex] + 1) {
                    return 0.0;
                }
            } else {
                Result = 0.0;
            }
        } else {
            if (PhaseChanges.first.back()[StellarGenerator::_kPhaseIndex] == PhaseChanges.second.back()[StellarGenerator::_kPhaseIndex]) {
                double FirstDiscardTimePoint = 0.0;
                double FirstCommonTimePoint = (*std::prev(PhaseChanges.first.end(), 2))[StellarGenerator::_kStarAgeIndex];

                std::size_t MinSize = std::min(PhaseChanges.first.size(), PhaseChanges.second.size());
                for (std::size_t i = 0; i != MinSize - 1; ++i) {
                    if (PhaseChanges.first[i][StellarGenerator::_kPhaseIndex] != PhaseChanges.second[i][StellarGenerator::_kPhaseIndex]) {
                        FirstDiscardTimePoint = PhaseChanges.first[i][StellarGenerator::_kStarAgeIndex];
                        break;
                    }
                }

                double DeltaTimePoint = FirstCommonTimePoint - FirstDiscardTimePoint;
                (*std::prev(PhaseChanges.first.end(), 2))[StellarGenerator::_kStarAgeIndex] -= DeltaTimePoint;
                PhaseChanges.first.back()[StellarGenerator::_kStarAgeIndex] -= DeltaTimePoint;
            }

            AlignArrays(PhaseChanges);

            Result = CalcEvolutionProgress(PhaseChanges, TargetAge, MassFactor);
            double IntegerPart = 0.0;
            double FractionalPart = std::modf(Result, &IntegerPart);
            if (PhaseChanges.second.back()[StellarGenerator::_kPhaseIndex] == 9 && FractionalPart > 0.99 && Result < 9.0 && IntegerPart >= (*std::prev(PhaseChanges.first.end(), 3))[StellarGenerator::_kPhaseIndex]) {
                Result = 9.0;
            }
        }
    }

    return Result;
}

std::pair<double, std::pair<double, double>> FindSurroundingTimePoints(const std::vector<std::vector<double>>& PhaseChanges, double TargetAge) {
    std::vector<std::vector<double>>::const_iterator LowerTimePoint;
    std::vector<std::vector<double>>::const_iterator UpperTimePoint;

    if (PhaseChanges.size() != 2 || PhaseChanges.front()[StellarGenerator::_kPhaseIndex] != PhaseChanges.back()[StellarGenerator::_kPhaseIndex]) {
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

    return { (*LowerTimePoint)[StellarGenerator::_kXIndex], { (*LowerTimePoint)[StellarGenerator::_kStarAgeIndex], (*UpperTimePoint)[StellarGenerator::_kStarAgeIndex] } };
}

std::pair<double, std::size_t> FindSurroundingTimePoints(const std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& PhaseChanges, double TargetAge, double MassFactor) {
    std::vector<double> LowerPhaseChangeTimePoints;
    std::vector<double> UpperPhaseChangeTimePoints;
    for (std::size_t i = 0; i != PhaseChanges.first.size(); ++i) {
        LowerPhaseChangeTimePoints.emplace_back(PhaseChanges.first[i][StellarGenerator::_kStarAgeIndex]);
        UpperPhaseChangeTimePoints.emplace_back(PhaseChanges.second[i][StellarGenerator::_kStarAgeIndex]);
    }

    std::vector<double> PhaseChangeTimePoints = InterpolateArray({ LowerPhaseChangeTimePoints, UpperPhaseChangeTimePoints }, MassFactor);

    if (TargetAge > PhaseChangeTimePoints.back()) {
        double Lifetime = LowerPhaseChangeTimePoints.back() + (UpperPhaseChangeTimePoints.back() - LowerPhaseChangeTimePoints.back()) * MassFactor;
        GenerateDeathStarPlaceholder(Lifetime);
    }

    std::vector<std::pair<double, double>> TimePointPairs;
    for (std::size_t i = 0; i != PhaseChanges.first.size(); ++i) {
        TimePointPairs.emplace_back(PhaseChanges.first[i][StellarGenerator::_kPhaseIndex], PhaseChangeTimePoints[i]);
    }

    std::pair<double, std::size_t> Result;
    for (std::size_t i = 0; i != TimePointPairs.size(); ++i) {
        if (TimePointPairs[i].second >= TargetAge) {
            Result.first = TimePointPairs[i == 0 ? 0 : i - 1].first;
            Result.second = i == 0 ? 0 : i - 1;
            break;
        }
    }

    return Result;
}

void AlignArrays(std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& Arrays) {
    auto TrimArray = [](std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& Arrays) -> void {
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
    };

    if (Arrays.first.back()[StellarGenerator::_kPhaseIndex] != 9 && Arrays.second.back()[StellarGenerator::_kPhaseIndex] != 9) {
        std::size_t MinSize = std::min(Arrays.first.size(), Arrays.second.size());
        Arrays.first.resize(MinSize);
        Arrays.second.resize(MinSize);
    } else if (Arrays.first.back()[StellarGenerator::_kPhaseIndex] != 9 && Arrays.second.back()[StellarGenerator::_kPhaseIndex] == 9) {
        if (Arrays.first.size() + 1 == Arrays.second.size()) {
            Arrays.first.back()[StellarGenerator::_kPhaseIndex] = 9;
            Arrays.first.back()[StellarGenerator::_kXIndex] = 9.0;
            Arrays.second.pop_back();
        } else {
            std::size_t MinSize = std::min(Arrays.first.size(), Arrays.second.size());
            Arrays.first.resize(MinSize - 1);
            Arrays.second.resize(MinSize - 1);
            Arrays.first.back()[StellarGenerator::_kPhaseIndex] = Arrays.second.back()[StellarGenerator::_kPhaseIndex];
            Arrays.first.back()[StellarGenerator::_kXIndex] = Arrays.second.back()[StellarGenerator::_kXIndex];
        }
    } else if (Arrays.first.back()[StellarGenerator::_kPhaseIndex] == 9 && Arrays.second.back()[StellarGenerator::_kPhaseIndex] == 9) {
        TrimArray(Arrays);
    } else {
        auto LastArray1 = Arrays.first.back();
        auto LastArray2 = Arrays.second.back();
        std::size_t MinSize = std::min(Arrays.first.size(), Arrays.second.size());
        Arrays.first.resize(MinSize - 1);
        Arrays.second.resize(MinSize - 1);
        Arrays.first.emplace_back(LastArray1);
        Arrays.second.emplace_back(LastArray2);
    }
}

std::vector<double> InterpolateRows(const std::shared_ptr<StellarGenerator::HrDiagram>& Data, double BvColorIndex) {
    std::vector<double> Result;

    std::pair<std::vector<double>, std::vector<double>> SurroundingRows;
    try {
        SurroundingRows = Data->FindSurroundingValues("B-V", BvColorIndex);
    } catch (std::out_of_range& e) {
        NpgsCoreError(std::string(e.what()));
    }

    double Factor = (BvColorIndex - SurroundingRows.first[0]) / (SurroundingRows.second[0] - SurroundingRows.first[0]);

    auto& Array1 = SurroundingRows.first;
    auto& Array2 = SurroundingRows.second;

    while (!Array1.empty() && !Array2.empty() && (Array1.back() == -1 || Array2.back() == -1)) {
        Array1.pop_back();
        Array2.pop_back();
    }

    Result = InterpolateArray(SurroundingRows, Factor);

    return Result;
}

std::vector<double> InterpolateRows(const std::shared_ptr<StellarGenerator::MistData>& Data, double EvolutionProgress) {
    return InterpolateRows(Data, EvolutionProgress, "x", StellarGenerator::_kXIndex, false);
}

std::vector<double> InterpolateRows(const std::shared_ptr<StellarGenerator::WdMistData>& Data, double TargetAge) {
    return InterpolateRows(Data, TargetAge, "star_age", StellarGenerator::_kWdStarAgeIndex, true);
}

std::vector<double> InterpolateRows(const auto& Data, double Target, const std::string& Header, int Index, bool bIsWhiteDwarf) {
    std::vector<double> Result;

    std::pair<std::vector<double>, std::vector<double>> SurroundingRows;
    try {
        SurroundingRows = Data->FindSurroundingValues(Header, Target);
    } catch (std::out_of_range& e) {
        if (!bIsWhiteDwarf) {
            NpgsCoreError(std::string(e.what()));
        } else {
            SurroundingRows.first  = Data->Data()->back();
            SurroundingRows.second = Data->Data()->back();
        }
    }

    if (SurroundingRows.first != SurroundingRows.second) {
        if (!bIsWhiteDwarf) {
            int LowerPhase = static_cast<int>(SurroundingRows.first[Index]);
            int UpperPhase = static_cast<int>(SurroundingRows.second[Index]);
            if (LowerPhase != UpperPhase) {
                SurroundingRows.second[Index] = LowerPhase + 1;
            }
        }

        double Factor = (Target - SurroundingRows.first[Index]) / (SurroundingRows.second[Index] - SurroundingRows.first[Index]);
        Result = InterpolateFinalData(SurroundingRows, Factor, bIsWhiteDwarf);
    } else {
        Result = SurroundingRows.first;
    }

    return Result;
}

std::vector<double> InterpolateArray(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Factor) {
    if (DataArrays.first.size() != DataArrays.second.size()) {
        throw std::runtime_error("Data arrays size mismatch.");
    }

    std::size_t Size = DataArrays.first.size();
    std::vector<double> Result(Size);
    for (std::size_t i = 0; i != Size; ++i) {
        Result[i] = DataArrays.first[i] + (DataArrays.second[i] - DataArrays.first[i]) * Factor;
    }

    return Result;
}

std::vector<double> InterpolateFinalData(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Factor, bool bIsWhiteDwarf) {
    if (DataArrays.first.size() != DataArrays.second.size()) {
        throw std::runtime_error("Data arrays size mismatch.");
    }

    std::vector<double> Result = InterpolateArray(DataArrays, Factor);

    if (!bIsWhiteDwarf) {
        Result[StellarGenerator::_kPhaseIndex] = DataArrays.first[StellarGenerator::_kPhaseIndex];
    }

    return Result;
}

void ExpandMistData(std::vector<double>& StarData, double TargetMass) {
    double RadiusSol     = std::pow(10.0, StarData[StellarGenerator::_kLogRIndex]);
    double Teff          = std::pow(10.0, StarData[StellarGenerator::_kLogTeffIndex]);
    double LuminositySol = std::pow(RadiusSol, 2.0) * std::pow((Teff / kSolarTeff), 4.0);

    double& StarMass = StarData[StellarGenerator::_kStarMassIndex];
    double& StarMdot = StarData[StellarGenerator::_kStarMdotIndex];
    double& LogR     = StarData[StellarGenerator::_kLogRIndex];
    double& LogTeff  = StarData[StellarGenerator::_kLogTeffIndex];

    double LogL = std::log10(LuminositySol);

    StarMass = TargetMass * (StarMass / 0.1);
    StarMdot = TargetMass * (StarMdot / 0.1);

    RadiusSol     = std::pow(10.0, LogR) * std::pow(TargetMass / 0.1, 2.3);
    LuminositySol = std::pow(10.0, LogL) * std::pow(TargetMass / 0.1, 2.3);

    Teff    = kSolarTeff * std::pow((LuminositySol / std::pow(RadiusSol, 2.0)), 0.25);
    LogTeff = std::log10(Teff);

    LogR = std::log10(RadiusSol);
}

_MODULES_END
_NPGS_END
