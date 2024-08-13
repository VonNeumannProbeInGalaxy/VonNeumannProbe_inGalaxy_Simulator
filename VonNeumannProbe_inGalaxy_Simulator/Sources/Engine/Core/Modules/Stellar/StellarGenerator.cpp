#include "StellarGenerator.h"

#include <cmath>

#include <algorithm>
#include <filesystem>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <glm/glm.hpp>

//#define ENABLE_LOGGER
#include "Engine/Core/Constants.h"
#include "Engine/Core/Logger.h"

_NPGS_BEGIN
_MODULES_BEGIN

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
static std::vector<double> InterpolateArray(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Factor);
static std::vector<double> InterpolateFinalData(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Factor);

// StellarGenerator implementations
// --------------------------------
StellarGenerator::StellarGenerator(int Seed, double MassLowerLimit, double AgeLowerLimit, double AgeUpperLimit) :
    _RandomEngine(Seed),
    _LogMassGenerator(std::log10(MassLowerLimit), std::log10(300.0)),
    _AgeGenerator(AgeLowerLimit, AgeUpperLimit),
    _CommonGenerator(0.0, 1.0),
    _FeHGenerator(-0.1, 0.3)
{}

StellarGenerator::BasicProperties StellarGenerator::GenBasicProperties() {
    BasicProperties Properties;

    double PosX     = _CommonGenerator(_RandomEngine) * 1000.0;
    double PosY     = _CommonGenerator(_RandomEngine) * 1000.0;
    double PosZ     = _CommonGenerator(_RandomEngine) * 1000.0;
    double Distance = _CommonGenerator(_RandomEngine) * 10000.0;
    Properties.StarSys.Name     = "S-" + std::to_string(PosX) + "-" + std::to_string(PosY) + "-" + std::to_string(PosZ) + "-" + std::to_string(Distance);
    Properties.StarSys.Position = glm::dvec3(PosX, PosY, PosZ);
    Properties.StarSys.Distance = Distance;

    double FeH = 0.0;
    do {
        FeH = _FeHGenerator(_RandomEngine);
    } while (FeH > 0.5 || FeH < -4.0);

    Properties.FeH = FeH;

    double Mass = GenerateMass(0.086, true);
    Properties.Mass = Mass;

    double Age = GenerateAge(2.6);
    Properties.Age = Age;

    return Properties;
}

AstroObject::Star StellarGenerator::GenerateStar() {
    BasicProperties BasicData = GenBasicProperties();
    // std::println("{}, {}, {}", BasicData.Age, BasicData.FeH, BasicData.Mass);
    return GenerateStar(BasicData);
}

AstroObject::Star StellarGenerator::GenerateStar(const BasicProperties& Properties) {
    AstroObject::Star Star(Properties);
    // std::println("Age: {:.2E}\tFeH: {:.2f}\tMass: {}", Properties.Age, Properties.FeH, Properties.Mass);
    std::vector<double> StarData;
    
    try {
        StarData = GetActuallyMistData(Properties);
    } catch (AstroObject::Star&) {
        NpgsCoreError("Star dead - Age: {}, FeH: {}, Mass: {}", Properties.Age, Properties.FeH, Properties.Mass);
    }

    if (StarData.empty()) {
        return {};
    }

    double Age               = StarData[_kStarAgeIndex];
    double MassSol           = StarData[_kStarMassIndex];
    double RadiusSol         = std::pow(10.0, StarData[_kLogRIndex]);
    double Teff              = std::pow(10.0, StarData[_kLogTeffIndex]);
    double SurfaceFeH        = StarData[_kLogSurfZIndex];
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

    AstroObject::Star::Phase EvolutionPhase = static_cast<AstroObject::Star::Phase>(StarData[10]);

    Star.SetAge(Age).SetMass(MassSol * kSolarMass).SetRadius(RadiusSol * kSolarRadius).SetEscapeVelocity(EscapeVelocity);
    Star.SetLuminosity(LuminositySol * kSolarLuminosity)
        .SetAbsoluteMagnitude(AbsoluteMagnitude)
        .SetTeff(Teff)
        .SetSurfaceFeH(SurfaceFeH)
        .SetSurfaceH1(SurfaceH1)
        .SetSurfaceHe3(SurfaceHe3)
        .SetCoreTemp(CoreTemp)
        .SetCoreDensity(CoreDensity * 1000)
        .SetStellarWindSpeed(StellarWindSpeed)
        .SetStellarWindMassLossRate(-(MassLossRate * kSolarMass / kYearInSeconds))
        .SetEvolutionProgress(EvolutionProgress)
        .SetEvolutionPhase(EvolutionPhase)
        .SetLifetime(Lifetime);

    CalcSpectralType(Star);

    if (MassSol >= 0.075 && MassSol < 0.33) {
        std::uniform_real_distribution<double> MagneticGenerator(500.0, 3000.0);
        Star.SetMagneticField(MagneticGenerator(_RandomEngine) / 10000);
    } else if (MassSol >= 0.33 && MassSol < 0.6) {
        std::uniform_real_distribution<double> MagneticGenerator(100.0, 1000.0);
        Star.SetMagneticField(MagneticGenerator(_RandomEngine) / 10000);
    } else if (MassSol >= 0.6 && MassSol < 1.5) {
        std::uniform_real_distribution<double> MagneticGenerator(1.0, 10.0);
        Star.SetMagneticField(MagneticGenerator(_RandomEngine) / 10000);
    } else if (MassSol >= 1.5 && MassSol < 20) {
        auto SpectralType = Star.GetStellarClass().Data();
        if (EvolutionPhase == AstroObject::Star::Phase::kMainSequence &&
            (SpectralType.HSpectralClass == StellarClass::SpectralClass::kSpectral_A ||
             SpectralType.HSpectralClass == StellarClass::SpectralClass::kSpectral_B)) {
            std::bernoulli_distribution ProbabilityGenerator(0.15);
            if (ProbabilityGenerator(_RandomEngine)) {
                std::uniform_real_distribution<double> MagneticGenerator(1000.0, 10000.0);
                Star.SetMagneticField(MagneticGenerator(_RandomEngine) / 10000);
                SpectralType.SpecialMark |= static_cast<std::uint32_t>(StellarClass::SpecialPeculiarities::kCode_p);
                Star.SetStellarClass(StellarClass(StellarClass::StarType::kNormalStar, SpectralType));
            }
        } else {
            std::uniform_real_distribution<double> MagneticGenerator(0.1, 1.0);
            Star.SetMagneticField(MagneticGenerator(_RandomEngine) / 10000);
        }
    } else {
        std::uniform_real_distribution<double> MagneticGenerator(100.0, 1000.0);
        Star.SetMagneticField(MagneticGenerator(_RandomEngine) / 10000);
    }

    return Star;
}

template<typename CsvType>
std::shared_ptr<CsvType> StellarGenerator::LoadCsvAsset(const std::string& Filename, const std::vector<std::string>& Headers) {
    std::lock_guard<std::mutex> Lock(_CacheMutex);
    if (Assets::AssetManager::GetAsset<CsvType>(Filename) == nullptr) {
        auto CsvAsset = std::make_shared<CsvType>(Filename, Headers);
        Assets::AssetManager::AddAsset<CsvType>(Filename, CsvAsset);
    }

    return Assets::AssetManager::GetAsset<CsvType>(Filename);
}

double StellarGenerator::GenerateAge(double MaxPdf) {
    double Age         = 0.0;
    double Probability = 0.0;
    do {
        Age = _AgeGenerator(_RandomEngine);
        Probability = DefaultAgePdf(Age / 1e9);
    } while (_CommonGenerator(_RandomEngine) * MaxPdf > Probability);

    return Age;
}

double StellarGenerator::GenerateMass(double MaxPdf, bool bIsBinary) {
    double LogMass     = 0.0;
    double Probability = 0.0;
    do {
        LogMass = _LogMassGenerator(_RandomEngine);
        Probability = DefaultLogMassPdf(LogMass, bIsBinary);
    } while (_CommonGenerator(_RandomEngine) * MaxPdf > Probability);

    return std::pow(10.0, LogMass);
}

std::vector<double> StellarGenerator::GetActuallyMistData(const BasicProperties& Properties) {
    double TargetAge  = Properties.Age;
    double TargetFeH  = Properties.FeH;
    double TargetMass = Properties.Mass;

    double ClosestFeH = *std::min_element(_kPresetFeH.begin(), _kPresetFeH.end(), [TargetFeH](double Lhs, double Rhs) -> bool {
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
    _CacheMutex.lock();
    if (_MassFileCache.contains(FeHStr)) {
        Masses = _MassFileCache[FeHStr];
    } else {
        _CacheMutex.unlock();
        for (const auto& Entry : std::filesystem::directory_iterator(FeHStr)) {
            std::string Filename = Entry.path().filename().string();
            double Mass = std::stod(Filename.substr(0, Filename.find("Ms_track.csv")));
            Masses.emplace_back(Mass);
        }

        _CacheMutex.lock();
        _MassFileCache.emplace(FeHStr, Masses);
    }
    _CacheMutex.unlock();

    auto it = std::lower_bound(Masses.begin(), Masses.end(), TargetMass);
    if (it == Masses.end()) {
        throw std::out_of_range("Mass value out of range.");
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
    std::string LowerMassFile = FeHStr + "/" + MassStr + "Ms_track.csv";

    MassStr.clear();
    MassStream.str("");
    MassStream.clear();

    MassStream << std::fixed << std::setfill('0') << std::setw(6) << std::setprecision(2) << UpperMass;
    MassStr = MassStream.str() + "0";
    std::string UpperMassFile = FeHStr + "/" + MassStr + "Ms_track.csv";

    Files.first = LowerMassFile;
    Files.second = UpperMassFile;

    std::vector<double> Result = InterpolateMistData(Files, TargetAge, MassFactor);

    return Result;
}

std::vector<double> StellarGenerator::InterpolateMistData(const std::pair<std::string, std::string>& Files, double TargetAge, double MassFactor) {
    std::vector<double> Result;

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

        Result = InterpolateFinalData({ LowerRows, UpperRows }, MassFactor);
    } else [[unlikely]] {
        std::shared_ptr<MistData> StarData = LoadCsvAsset<MistData>(Files.first, _kMistHeaders);
        auto PhaseChanges = FindPhaseChanges(StarData);
        std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>> PhaseChangePair{ PhaseChanges, {} };
        double EvolutionProgress = CalcEvolutionProgress(PhaseChangePair, TargetAge, MassFactor);
        double Lifetime = PhaseChanges.back()[_kStarAgeIndex];
        Result = InterpolateRows(StarData, EvolutionProgress);
        Result.emplace_back(Lifetime);
    }

    return Result;
}

std::vector<std::vector<double>> StellarGenerator::FindPhaseChanges(const std::shared_ptr<MistData>& DataCsv) {
    std::vector<std::vector<double>> Result;

    _CacheMutex.lock();
    if (_PhaseChangesCache.contains(DataCsv)) {
        Result = _PhaseChangesCache[DataCsv];
    } else {
        _CacheMutex.unlock();
        auto* DataArray = DataCsv->Data();
        int CurrentPhase = -2;
        for (const auto& Row : *DataArray) {
            if (Row[_kPhaseIndex] != CurrentPhase || Row[_kXIndex] == 10.0) {
                CurrentPhase = static_cast<int>(Row[_kPhaseIndex]);
                Result.emplace_back(Row);
            }
        }

        _CacheMutex.lock();
        _PhaseChangesCache.emplace(DataCsv, Result);
    }
    _CacheMutex.unlock();

    return Result;
}

void StellarGenerator::CalcSpectralType(AstroObject::Star& StarData) {
    double Teff = StarData.GetTeff();
    auto EvolutionPhase = StarData.GetEvolutionPhase();

    StellarClass::SpectralType SpectralType{};
    SpectralType.bIsAmStar = false;

    std::vector<std::pair<int, int>> SpecialSubclassMap;
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
                    SpecialSubclassMap = it->second;
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
                    SpecialSubclassMap = AstroObject::Star::_kSpectralSubclassMap_WNxh;
                    SpectralClass = 13;
                    SpectralType.SpecialMark = static_cast<std::uint32_t>(StellarClass::SpecialPeculiarities::kCode_h);
                } else if (SurfaceH1 >= 0.1) {
                    SpecialSubclassMap = AstroObject::Star::_kSpectralSubclassMap_WN;
                    SpectralClass = 13;
                } else if (SurfaceH1 < 0.1 && SurfaceH1 > 0.05) {
                    SpecialSubclassMap = AstroObject::Star::_kSpectralSubclassMap_WC;
                    SpectralClass = 12;
                } else {
                    SpecialSubclassMap = AstroObject::Star::_kSpectralSubclassMap_WO;
                    SpectralClass = 14;
                }
            }
        }

        SpectralType.HSpectralClass = static_cast<StellarClass::SpectralClass>(SpectralClass);

        for (auto it = SpecialSubclassMap.begin(); it != SpecialSubclassMap.end() - 1; ++it) {
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
        double SurfaceH1 = StarData.GetSurfaceH1();
        if (Teff < 54000) {
            CalcSpectralSubclass(EvolutionPhase, SurfaceH1);

            if (EvolutionPhase != AstroObject::Star::Phase::kWolfRayet) {
                if (EvolutionPhase == AstroObject::Star::Phase::kPrevMainSequence) {
                    SpectralType.LuminosityClass = StellarClass::LuminosityClass::kLuminosity_V;
                } else if (EvolutionPhase == AstroObject::Star::Phase::kMainSequence) {
                    if (SpectralType.HSpectralClass == StellarClass::SpectralClass::kSpectral_O) {
                        if (SurfaceH1 > 0.6) {
                            SpectralType.LuminosityClass = StellarClass::LuminosityClass::kLuminosity_V;
                        } else {
                            SpectralType.LuminosityClass = CalcLuminosityClass(StarData);
                        }
                    } else {
                        SpectralType.LuminosityClass = StellarClass::LuminosityClass::kLuminosity_V;
                    }
                } else if (EvolutionPhase == AstroObject::Star::Phase::kRedGiant) {
                    SpectralType.LuminosityClass = StellarClass::LuminosityClass::kLuminosity_III;
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
    } else {
        CalcSpectralSubclass(AstroObject::Star::Phase::kWolfRayet, StarData.GetSurfaceH1());
        SpectralType.LuminosityClass = StellarClass::LuminosityClass::kLuminosity_Unknown;
    }

    StellarClass Class(StellarClass::StarType::kNormalStar, SpectralType);
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

    StellarClass::LuminosityClass LuminosityClass = StellarClass::LuminosityClass::kLuminosity_Unknown;
    if (LuminositySol > LuminosityData[2] && (ClosestValue == LuminosityData[1] || ClosestValue == LuminosityData[2])) {
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_Iab;
    } else if (LuminositySol < LuminosityData[2] && ClosestValue == LuminosityData[2]) {
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_Ib;
    } else if (LuminositySol < LuminosityData[2] && ClosestValue == LuminosityData[3]) {
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_II;
    } else if (LuminosityData[3] != -1) {
        if (LuminositySol < LuminosityData[3] && ClosestValue == LuminosityData[4]) {
            LuminosityClass = StellarClass::LuminosityClass::kLuminosity_III;
        }
    } else if (LuminosityData[4] != -1) {
        if (LuminositySol < LuminosityData[4] && ClosestValue == LuminosityData[5]) {
            LuminosityClass = StellarClass::LuminosityClass::kLuminosity_IV;
        }
    }

    return LuminosityClass;
}

const int StellarGenerator::_kStarAgeIndex      = 0;
const int StellarGenerator::_kStarMassIndex     = 1;
const int StellarGenerator::_kStarMdotIndex     = 2;
const int StellarGenerator::_kLogTeffIndex      = 3;
const int StellarGenerator::_kLogRIndex         = 4;
const int StellarGenerator::_kLogSurfZIndex     = 5;
const int StellarGenerator::_kSurfaceH1Index    = 6;
const int StellarGenerator::_kSurfaceHe3Index   = 7;
const int StellarGenerator::_kLogCenterTIndex   = 8;
const int StellarGenerator::_kLogCenterRhoIndex = 9;
const int StellarGenerator::_kPhaseIndex        = 10;
const int StellarGenerator::_kXIndex            = 11;
const int StellarGenerator::_kLifetimeIndex     = 12;

const std::vector<std::string> StellarGenerator::_kMistHeaders{ "star_age", "star_mass", "star_mdot", "log_Teff", "log_R", "log_surf_z", "surface_h1", "surface_he3", "log_center_T", "log_center_Rho", "phase", "x" };
const std::vector<std::string> StellarGenerator::_kHrDiagramHeaders{ "B-V", "Ia", "Ib", "II", "III", "IV" };
const std::unordered_set<double> StellarGenerator::_kPresetFeH{ -4.0, -3.0, -2.0, -1.5, -1.0, -0.5, 0.0, 0.5 };
std::unordered_map<std::string, std::vector<double>> StellarGenerator::_MassFileCache;
std::unordered_map<std::shared_ptr<StellarGenerator::MistData>, std::vector<std::vector<double>>> StellarGenerator::_PhaseChangesCache;
std::mutex StellarGenerator::_CacheMutex;

// Tool functions implementations
// ------------------------------
static double DefaultAgePdf(double Fuck) {
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

static double CalcEvolutionProgress(std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& PhaseChanges, double TargetAge, double MassFactor) {
    double Result = 0.0;
    double Phase  = 0.0;

    if (PhaseChanges.second.empty()) [[unlikely]] {
        const auto& TimePointResults = FindSurroundingTimePoints(PhaseChanges.first, TargetAge);
        Phase = TimePointResults.first;
        const auto& TimePoints = TimePointResults.second;
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

static std::pair<double, std::pair<double, double>> FindSurroundingTimePoints(const std::vector<std::vector<double>>& PhaseChanges, double TargetAge) {
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

static std::pair<double, std::size_t> FindSurroundingTimePoints(const std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& PhaseChanges, double TargetAge, double MassFactor) {
    std::vector<double> LowerPhaseChangeTimePoints;
    std::vector<double> UpperPhaseChangeTimePoints;
    for (std::size_t i = 0; i != PhaseChanges.first.size(); ++i) {
        LowerPhaseChangeTimePoints.emplace_back(PhaseChanges.first[i][StellarGenerator::_kStarAgeIndex]);
        UpperPhaseChangeTimePoints.emplace_back(PhaseChanges.second[i][StellarGenerator::_kStarAgeIndex]);
    }

    std::vector<double> PhaseChangeTimePoints = InterpolateArray({ LowerPhaseChangeTimePoints, UpperPhaseChangeTimePoints }, MassFactor);

    if (TargetAge > PhaseChangeTimePoints.back()) {
        StellarClass::SpectralType DeathStarClass{ StellarClass::SpectralClass::kSpectral_Unknown, 0, false, StellarClass::SpectralClass::kSpectral_Unknown, 0, StellarClass::LuminosityClass::kLuminosity_Unknown, 0 };
        AstroObject::Star DeathStar;
        DeathStar.SetStellarClass(StellarClass(StellarClass::StarType::kDeathStar, DeathStarClass));
        throw DeathStar;
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

static void AlignArrays(std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& Arrays) {
    auto TrimArray = [](std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>& Arrays) {
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

static std::vector<double> InterpolateRows(const std::shared_ptr<StellarGenerator::HrDiagram>& Data, double BvColorIndex) {
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

    while (Result.size() <= 6) {
        Result.emplace_back(-1);
    }

    return Result;
}

static std::vector<double> InterpolateRows(const std::shared_ptr<StellarGenerator::MistData>& Data, double EvolutionProgress) {
    std::vector<double> Result;

    std::pair<std::vector<double>, std::vector<double>> SurroundingRows;
    try {
        SurroundingRows = Data->FindSurroundingValues("x", EvolutionProgress);
    } catch (std::out_of_range& e) {
        NpgsCoreError(std::string(e.what()));
    }

    if (SurroundingRows.first != SurroundingRows.second) {
        int LowerTempPhase = static_cast<int>(SurroundingRows.first[StellarGenerator::_kPhaseIndex]);
        int UpperTempPhase = static_cast<int>(SurroundingRows.second[StellarGenerator::_kPhaseIndex]);
        if (LowerTempPhase != UpperTempPhase) {
            SurroundingRows.second[StellarGenerator::_kXIndex] = LowerTempPhase + 1;
        }
        double ProgressFactor = (EvolutionProgress - SurroundingRows.first[StellarGenerator::_kXIndex]) / (SurroundingRows.second[StellarGenerator::_kXIndex] - SurroundingRows.first[StellarGenerator::_kXIndex]);
        Result = InterpolateFinalData(SurroundingRows, ProgressFactor);
    } else {
        Result = SurroundingRows.first;
    }

    return Result;
}

static std::vector<double> InterpolateArray(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Factor) {
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

static std::vector<double> InterpolateFinalData(const std::pair<std::vector<double>, std::vector<double>>& DataArrays, double Factor) {
    if (DataArrays.first.size() != DataArrays.second.size()) {
        throw std::runtime_error("Data arrays size mismatch.");
    }

    std::vector<double> Result = InterpolateArray(DataArrays, Factor);
    Result[StellarGenerator::_kPhaseIndex] = DataArrays.first[StellarGenerator::_kPhaseIndex];

    return Result;
}

_MODULES_END
_NPGS_END
