#include "Universe.h"

#include <cstdint>
#include <algorithm>
#include <array>
#include <format>
#include <future>
#include <iomanip>
#include <iterator>
#include <limits>
#include <print>
#include <sstream>
#include <string>
#include <thread>

#define ENABLE_LOGGER
// #define OUTPUT_DATA
#include "Engine/Core/Modules/OrbitalGenerator.h"
#include "Engine/Core/Modules/StellarClass.h"
#include "Engine/Core/Modules/StellarGenerator.h"
#include "Engine/Core/Constants.h"
#include "Engine/Core/Logger.h"
#include "Engine/Core/Random.hpp"

_NPGS_BEGIN

static void FillStellarSystem(StellarSystem& System);

Universe::Universe(unsigned Seed, std::size_t NumStars, std::size_t NumExtraGiants, std::size_t NumExtraMassiveStars, std::size_t NumExtraNeutronStars, std::size_t NumExtraBlackHoles, std::size_t NumExtraMergeStars, float UniverseAge)
    :
    _RandomEngine(Seed),
    _SeedGenerator(0ull, std::numeric_limits<std::uint32_t>::max()),
    _CommonGenerator(0.0f, 1.0f),
    _ThreadPool(ThreadPool::GetInstance()),

    _NumStars(NumStars),
    _NumExtraGiants(NumExtraGiants),
    _NumExtraMassiveStars(NumExtraMassiveStars),
    _NumExtraNeutronStars(NumExtraNeutronStars),
    _NumExtraBlackHoles(NumExtraBlackHoles),
    _NumExtraMergeStars(NumExtraMergeStars),
    _UniverseAge(UniverseAge)
{
    std::vector<std::uint32_t> Seeds(32);
    for (int i = 0; i != 32; ++i) {
        Seeds.emplace_back(_SeedGenerator.Generate(_RandomEngine));
    }

    std::shuffle(Seeds.begin(), Seeds.end(), _RandomEngine);
    std::seed_seq SeedSequence(Seeds.begin(), Seeds.end());
    _RandomEngine.seed(SeedSequence);
}

Universe::~Universe() {
    _ThreadPool->Destroy();
}

void Universe::FillUniverse() {
    int MaxThread = _ThreadPool->GetMaxThreadCount();

    NpgsCoreInfo("Initializating and generating basic properties...");
    // std::vector<std::future<Modules::StellarGenerator::BasicProperties>> Futures;
    std::vector<Modules::StellarGenerator> Generators;
    std::vector<Modules::StellarGenerator::BasicProperties> BasicProperties;

    using enum Modules::StellarGenerator::GenDistribution;
    using enum Modules::StellarGenerator::GenOption;
    auto CreateGenerators =
        [&, this](Modules::StellarGenerator::GenOption Option  = kNormal,
            float MassLowerLimit =  0.1f, float MassUpperLimit = 300.0f,   Modules::StellarGenerator::GenDistribution MassDistribution = kFromPdf,
            float AgeLowerLimit  =  0.0f, float AgeUpperLimit  = 1.26e10f, Modules::StellarGenerator::GenDistribution AgeDistribution  = kFromPdf,
            float FeHLowerLimit  = -4.0f, float FeHUpperLimit  = 0.5f,     Modules::StellarGenerator::GenDistribution FeHDistribution  = kFromPdf) -> void {
        for (int i = 0; i != MaxThread; ++i) {
            std::vector<std::uint32_t> Seeds(32);
            for (int i = 0; i != 32; ++i) {
                Seeds.emplace_back(_SeedGenerator.Generate(_RandomEngine));
            }

            std::shuffle(Seeds.begin(), Seeds.end(), _RandomEngine);
            std::seed_seq SeedSequence(Seeds.begin(), Seeds.end());
            Generators.emplace_back(SeedSequence, Option, _UniverseAge, MassLowerLimit, MassUpperLimit, MassDistribution, AgeLowerLimit, AgeUpperLimit, AgeDistribution, FeHLowerLimit, FeHUpperLimit, FeHDistribution);
        }
    };

    // auto GenerateBasicProperties = [&, this](std::size_t NumStars) -> void {
    //     for (std::size_t i = 0; i != NumStars; ++i) {
    //         Futures.emplace_back(_ThreadPool->Commit([&, i]() -> Npgs::Modules::StellarGenerator::BasicProperties {
    //             std::size_t ThreadId = i % Generators.size();
    //             return Generators[ThreadId].GenBasicProperties();
    //         }));
    //     }
    // };

    auto GenerateBasicProperties = [&, this](std::size_t NumStars) -> void {
        for (std::size_t i = 0; i != NumStars; ++i) {
            std::size_t ThreadId = i % Generators.size();
            BasicProperties.emplace_back(Generators[ThreadId].GenBasicProperties());
        }
    };

    if (_NumExtraGiants != 0) {
        Generators.clear();
        CreateGenerators(kGiant, 1.0, 30.0);
        GenerateBasicProperties(_NumExtraGiants);
    }

    if (_NumExtraMassiveStars != 0) {
        Generators.clear();
        CreateGenerators(Modules::StellarGenerator::GenOption::kNormal, 20.0f, 300.0f, Modules::StellarGenerator::GenDistribution::kUniform, 0.0f, 3.5e6f, Modules::StellarGenerator::GenDistribution::kUniform);
        GenerateBasicProperties(_NumExtraMassiveStars);
    }

    if (_NumExtraNeutronStars != 0) {
        Generators.clear();
        CreateGenerators(Modules::StellarGenerator::GenOption::kDeathStar, 10.0f, 20.0f, Modules::StellarGenerator::GenDistribution::kUniform, 1e7f, 1e8f, Modules::StellarGenerator::GenDistribution::kUniformByExponent);
        GenerateBasicProperties(_NumExtraNeutronStars);
    }

    if (_NumExtraBlackHoles != 0) {
        Generators.clear();
        CreateGenerators(Modules::StellarGenerator::GenOption::kNormal, 35.0f, 300.0f, Modules::StellarGenerator::GenDistribution::kUniform, 1e7f, 1.26e10f, Modules::StellarGenerator::GenDistribution::kFromPdf, -2.0, 0.5);
        GenerateBasicProperties(_NumExtraBlackHoles);
    }

    if (_NumExtraMergeStars != 0) {
        Generators.clear();
        CreateGenerators(Modules::StellarGenerator::GenOption::kMergeStar, 0.0f, 0.0f, Modules::StellarGenerator::GenDistribution::kUniform, 1e6f, 1e8f, Modules::StellarGenerator::GenDistribution::kUniformByExponent);
        GenerateBasicProperties(_NumExtraMergeStars);
    }

    std::size_t NumCommonStars = _NumStars - _NumExtraGiants - _NumExtraMassiveStars - _NumExtraNeutronStars - _NumExtraBlackHoles - _NumExtraMergeStars;

    Generators.clear();
    CreateGenerators(Modules::StellarGenerator::GenOption::kNormal, 0.075f);
    // CreateGenerators(Modules::StellarGenerator::GenOption::kNormal, 8.0f, 300.0f);
    GenerateBasicProperties(NumCommonStars);

    // for (auto& Future : Futures) {
    //     Future.wait();
    // }

    NpgsCoreInfo("Basic properties generation completed.");
    NpgsCoreInfo("Interpolating stellar data as {} physical cores...", MaxThread);

    std::vector<std::future<Npgs::Astro::Star>> StarFutures;
    for (std::size_t i = 0; i != _NumStars; ++i) {
        StarFutures.emplace_back(_ThreadPool->Commit([&, i]() -> Npgs::Astro::Star {
            std::size_t ThreadId = i % Generators.size();
            auto& Properties = BasicProperties[i]; 
            // auto Properties = Futures[i].get();
            return Generators[ThreadId].GenerateStar(Properties);
        }));
    }

    for (auto& Future : StarFutures) {
        Future.wait();
    }

    std::vector<Astro::Star> Stars;

    Stars.reserve(StarFutures.size());
    std::transform(
        std::make_move_iterator(StarFutures.begin()),
        std::make_move_iterator(StarFutures.end()),
        std::back_inserter(Stars),
        [](std::future<Astro::Star>&& Future) -> Astro::Star {
            return Future.get();
        }
    );

#ifdef OUTPUT_DATA
    NpgsCoreInfo("Outputing data...");
    return Stars;
#endif // OUTPUT_DATA

    NpgsCoreInfo("Star detail interpolation completed.");
    NpgsCoreInfo("Building stellar octree...");
    GenerateSlots(0.1f, _NumStars, 0.004f);
    NpgsCoreInfo("Stellar octree has been built.");

    NpgsCoreInfo("Linking positions in octree to stars...");
    _StellarSystems.reserve(_NumStars);
    std::shuffle(Stars.begin(), Stars.end(), _RandomEngine);
    std::vector<glm::vec3> Slots;
    OctreeLinkToStellarSystems(Stars, Slots);

    NpgsCoreInfo("Sorting...");
    std::sort(Slots.begin(), Slots.end(), [](const glm::vec3& Point1, const glm::vec3& Point2) {
        return glm::length(Point1) < glm::length(Point2);
    });

    NpgsCoreInfo("Assigning name...");
    std::string Name;
    std::ostringstream Stream;
    for (auto& System : _StellarSystems) {
        glm::vec3 Position = System.GetBaryPosition();
        auto it = std::lower_bound(Slots.begin(), Slots.end(), Position, [](const glm::vec3& Point1, const glm::vec3& Point2) -> bool {
            return glm::length(Point1) < glm::length(Point2);
        });
        std::ptrdiff_t Offset = it - Slots.begin();
        Stream << "S-" << std::setfill('0') << std::setw(8) << std::to_string(Offset);
        Name = Stream.str();
        System.SetBaryName(Name).SetBaryDistanceRank(Offset);
        Stream.str("");
        Stream.clear();

        auto& Stars = System.StarData();
        if (Stars.size() != 1) {
            std::sort(Stars.begin(), Stars.end(), [](const Astro::Star& Star1, const Astro::Star& Star2) -> bool {
                return Star1.GetMass() > Star2.GetMass();
            });

            char Rank = 'A';
            for (auto& Star : Stars) {
                Star.SetName(Star.GetName() + " " + Rank);
            }
        }
    }

    NpgsCoreInfo("Reset home stellar system...");
    NodeType* HomeNode = _Octree->Find(glm::vec3(0.0f), [](const NodeType& Node) -> bool {
        if (Node.IsLeafNode()) {
            auto& Points = Node.GetPoints();
            return std::find(Points.begin(), Points.end(), glm::vec3(0.0f)) != Points.end();
        } else {
            return false;
        }
    });

    auto* HomeSystem = HomeNode->GetLink([](StellarSystem* System) -> bool {
        return System->GetBaryPosition() == glm::vec3(0.0f);
    });
    HomeNode->RemoveStorage();
    HomeNode->AddPoint(glm::vec3(0.0f));
    HomeSystem->SetBaryNormal(glm::vec2(0.0f));

    NpgsCoreInfo("Star generation completed.");

    for (auto& System : _StellarSystems) {
        for (auto& Star : System.StarData()) {
            _StarPtrs.emplace_back(&Star);
        }
    }

    _ThreadPool->Terminate();
}

void Universe::ReplaceStar(std::size_t DistanceRank, const Astro::Star& StarData) {
    for (auto& System : _StellarSystems) {
        if (DistanceRank == System.GetBaryDistanceRank()) {
            auto& Star = System.StarData();
            if (Star.size() != 1) {
                return;
            }

            Star.clear();
            Star.emplace_back(StarData);
        }
    }
}

void Universe::CountStars() const {
    constexpr int kTypeO = 0;
    constexpr int kTypeB = 1;
    constexpr int kTypeA = 2;
    constexpr int kTypeF = 3;
    constexpr int kTypeG = 4;
    constexpr int kTypeK = 5;
    constexpr int kTypeM = 6;

    std::array<std::size_t, 7> MainSequence{};
    std::array<std::size_t, 7> Subgiants{};
    std::array<std::size_t, 7> Giants{};
    std::array<std::size_t, 7> BrightGiants{};
    std::array<std::size_t, 7> Supergiants{};
    std::array<std::size_t, 7> Hypergiants{};

    std::size_t WolfRayet    = 0;
    std::size_t WhiteDwarfs  = 0;
    std::size_t NeutronStars = 0;
    std::size_t BlackHoles   = 0;

    struct MostLuminous {
        double LuminositySol{};
        const Astro::Star* Star = nullptr;
    };

    struct MostMassive {
        double MassSol{};
        const Astro::Star* Star = nullptr;
    };

    struct Largest {
        float RadiusSol{};
        const Astro::Star* Star = nullptr;
    };

    struct Hottest {
        float Teff{};
        const Astro::Star* Star = nullptr;
    };

    struct Oldest {
        float Age{};
        const Astro::Star* Star = nullptr;
    };

    auto CountClass = [](const Modules::StellarClass::SpectralType& SpectralType, std::array<std::size_t, 7>& Type) {
        switch (SpectralType.HSpectralClass) {
        case Modules::StellarClass::SpectralClass::kSpectral_O:
            ++Type[kTypeO];
            break;
        case Modules::StellarClass::SpectralClass::kSpectral_B:
            ++Type[kTypeB];
            break;
        case Modules::StellarClass::SpectralClass::kSpectral_A:
            ++Type[kTypeA];
            break;
        case Modules::StellarClass::SpectralClass::kSpectral_F:
            ++Type[kTypeF];
            break;
        case Modules::StellarClass::SpectralClass::kSpectral_G:
            ++Type[kTypeG];
            break;
        case Modules::StellarClass::SpectralClass::kSpectral_K:
            ++Type[kTypeK];
            break;
        case Modules::StellarClass::SpectralClass::kSpectral_M:
            ++Type[kTypeM];
            break;
        }
    };

    auto CountMostLuminous = [](const Astro::Star* Star, MostLuminous& MostLuminousStar) {
        double LuminositySol = 0.0;
        LuminositySol = Star->GetLuminosity() / kSolarLuminosity;
        if (MostLuminousStar.LuminositySol < LuminositySol) {
            MostLuminousStar.LuminositySol = LuminositySol;
            MostLuminousStar.Star = Star;
        }
    };

    auto CountMostMassive = [](const Astro::Star* Star, MostMassive& MostMassiveStar) {
        double MassSol = 0.0;
        MassSol = Star->GetMass() / kSolarMass;
        if (MostMassiveStar.MassSol < MassSol) {
            MostMassiveStar.MassSol = MassSol;
            MostMassiveStar.Star = Star;
        }
    };

    auto CountLargest = [](const Astro::Star* Star, Largest& LargestStar) {
        float RadiusSol = 0.0f;
        RadiusSol = Star->GetRadius() / kSolarRadius;
        if (LargestStar.RadiusSol < RadiusSol) {
            LargestStar.RadiusSol = RadiusSol;
            LargestStar.Star = Star;
        }
    };

    auto CountHottest = [](const Astro::Star* Star, Hottest& HottestStar) {
        float Teff = 0.0f;
        Teff = Star->GetTeff();
        if (HottestStar.Teff < Teff) {
            HottestStar.Teff = Teff;
            HottestStar.Star = Star;
        }
    };

    auto CountOldest = [](const Astro::Star* Star, Oldest& OldestStar) {
        float Age = 0.0f;
        Age = Star->GetAge();
        if (OldestStar.Age < Age) {
            OldestStar.Age = Age;
            OldestStar.Star = Star;
        }
    };

    MostLuminous MostLuminousMainSequence;
    MostLuminous MostLuminousSubgiant;
    MostLuminous MostLuminousGiant;
    MostLuminous MostLuminousBrightGiant;
    MostLuminous MostLuminousSupergiant;
    MostLuminous MostLuminousHypergiant;
    MostLuminous MostLuminousWolfRayet;

    MostMassive MostMassiveMainSequence;
    MostMassive MostMassiveSubgiant;
    MostMassive MostMassiveGiant;
    MostMassive MostMassiveBrightGiant;
    MostMassive MostMassiveSupergiant;
    MostMassive MostMassiveHypergiant;
    MostMassive MostMassiveWolfRayet;

    Largest LargestMainSequence;
    Largest LargestSubgiant;
    Largest LargestGiant;
    Largest LargestBrightGiant;
    Largest LargestSupergiant;
    Largest LargestHypergiant;
    Largest LargestWolfRayet;

    Hottest HottestMainSequence;
    Hottest HottestSubgiant;
    Hottest HottestGiant;
    Hottest HottestBrightGiant;
    Hottest HottestSupergiant;
    Hottest HottestHypergiant;
    Hottest HottestWolfRayet;

    Oldest OldestMainSequence;
    Oldest OldestSubgiant;
    Oldest OldestGiant;
    Oldest OldestBrightGiant;
    Oldest OldestSupergiant;
    Oldest OldestHypergiant;
    Oldest OldestWolfRayet;

    for (const auto* Star : _StarPtrs) {
        const Modules::StellarClass& Class = Star->GetStellarClass();
        Modules::StellarClass::StarType StarType = Class.GetStarType();
        if (StarType != Modules::StellarClass::StarType::kNormalStar) {
            switch (StarType) {
            case Modules::StellarClass::StarType::kBlackHole:
                ++BlackHoles;
                break;
            case Modules::StellarClass::StarType::kNeutronStar:
                ++NeutronStars;
                break;
            case Modules::StellarClass::StarType::kWhiteDwarf:
                ++WhiteDwarfs;
                break;
            default:
                break;
            }

            continue;
        }

        Modules::StellarClass::SpectralType SpectralType = Class.Data();
        if (SpectralType.LuminosityClass == Modules::StellarClass::LuminosityClass::kLuminosity_Unknown) {
            if (SpectralType.HSpectralClass == Modules::StellarClass::SpectralClass::kSpectral_WC ||
                SpectralType.HSpectralClass == Modules::StellarClass::SpectralClass::kSpectral_WN ||
                SpectralType.HSpectralClass == Modules::StellarClass::SpectralClass::kSpectral_WO) {
                ++WolfRayet;
                CountMostLuminous(Star, MostLuminousWolfRayet);
                CountMostMassive(Star, MostMassiveWolfRayet);
                CountLargest(Star, LargestWolfRayet);
                CountHottest(Star, HottestWolfRayet);
                CountOldest(Star, OldestWolfRayet);
                continue;
            }
        }

        if (SpectralType.LuminosityClass == Modules::StellarClass::LuminosityClass::kLuminosity_0 ||
            SpectralType.LuminosityClass == Modules::StellarClass::LuminosityClass::kLuminosity_IaPlus) {
            CountClass(SpectralType, Hypergiants);
            CountMostLuminous(Star, MostLuminousHypergiant);
            CountMostMassive(Star, MostMassiveHypergiant);
            CountLargest(Star, LargestHypergiant);
            CountHottest(Star, HottestHypergiant);
            CountOldest(Star, OldestHypergiant);
            continue;
        }

        if (SpectralType.LuminosityClass == Modules::StellarClass::LuminosityClass::kLuminosity_Ia ||
            SpectralType.LuminosityClass == Modules::StellarClass::LuminosityClass::kLuminosity_Iab ||
            SpectralType.LuminosityClass == Modules::StellarClass::LuminosityClass::kLuminosity_Ib) {
            CountClass(SpectralType, Supergiants);
            CountMostLuminous(Star, MostLuminousSupergiant);
            CountMostMassive(Star, MostMassiveSupergiant);
            CountLargest(Star, LargestSupergiant);
            CountHottest(Star, HottestSupergiant);
            CountOldest(Star, OldestSupergiant);
            continue;
        }

        if (SpectralType.LuminosityClass == Modules::StellarClass::LuminosityClass::kLuminosity_II) {
            CountClass(SpectralType, BrightGiants);
            CountMostLuminous(Star, MostLuminousBrightGiant);
            CountMostMassive(Star, MostMassiveBrightGiant);
            CountLargest(Star, LargestBrightGiant);
            CountHottest(Star, HottestBrightGiant);
            CountOldest(Star, OldestBrightGiant);
            continue;
        }

        if (SpectralType.LuminosityClass == Modules::StellarClass::LuminosityClass::kLuminosity_III) {
            CountClass(SpectralType, Giants);
            CountMostLuminous(Star, MostLuminousGiant);
            CountMostMassive(Star, MostMassiveGiant);
            CountLargest(Star, LargestGiant);
            CountHottest(Star, HottestGiant);
            CountOldest(Star, OldestGiant);
            continue;
        }

        if (SpectralType.LuminosityClass == Modules::StellarClass::LuminosityClass::kLuminosity_IV) {
            CountClass(SpectralType, Subgiants);
            CountMostLuminous(Star, MostLuminousSubgiant);
            CountMostMassive(Star, MostMassiveSubgiant);
            CountLargest(Star, LargestSubgiant);
            CountHottest(Star, HottestSubgiant);
            CountOldest(Star, OldestSubgiant);
            continue;
        }

        if (SpectralType.LuminosityClass == Modules::StellarClass::LuminosityClass::kLuminosity_V) {
            CountClass(SpectralType, MainSequence);
            CountMostLuminous(Star, MostLuminousMainSequence);
            CountMostMassive(Star, MostMassiveMainSequence);
            CountLargest(Star, LargestMainSequence);
            CountHottest(Star, HottestMainSequence);
            CountOldest(Star, OldestMainSequence);
            continue;
        }
    }

    auto FormatTitle = []() -> std::string {
        return std::format(
            "{:>6} {:>6} {:>8} {:>8} {:7} {:>5} {:>13} {:>7} {:>8} {:>8} {:>11} {:>8} {:>9} {:>5} {:>8} {:>8} {:>8} {:>15} {:>9} {:>8}",
            "InMass", "Mass", "Radius", "Age", "Class", "FeH", "Lum", "AbsMagn", "Teff", "CoreTemp", "CoreDensity", "Mdot", "WindSpeed", "Phase", "SurfZ", "SurfNuc", "SurfVol", "Magnetic", "Lifetime", "Spin");
    };

    auto FormatInfo = [](const Astro::Star* Star) -> std::string {
        if (Star == nullptr) {
            return "No star generated.";
        }

        return std::format("{:6.2f} {:6.2f} {:8.2f} {:8.2E} {:7} {:5.2f} {:13.4f} {:7.2f} {:8.1f} {:8.2E} {:11.2E} {:8.2E} {:9} {:5} {:8.2E} {:8.2E} {:8.2E} {:15.5f} {:9.2E} {:8.2E}",
            Star->GetInitialMass() / kSolarMass,
            Star->GetMass() / kSolarMass,
            Star->GetRadius() / kSolarRadius,
            Star->GetAge(),
            Star->GetStellarClass().ToString(),
            Star->GetFeH(),
            Star->GetLuminosity() / kSolarLuminosity,
            kSolarAbsoluteMagnitude - 2.5 * std::log10(Star->GetLuminosity() / kSolarLuminosity),
            Star->GetTeff(),
            Star->GetCoreTemp(),
            Star->GetCoreDensity(),
            Star->GetStellarWindMassLossRate() * kYearInSeconds / kSolarMass,
            static_cast<int>(std::round(Star->GetStellarWindSpeed())),
            static_cast<int>(Star->GetEvolutionPhase()),
            Star->GetSurfaceZ(),
            Star->GetSurfaceEnergeticNuclide(),
            Star->GetSurfaceVolatiles(),
            Star->GetMagneticField(),
            Star->GetLifetime(),
            Star->GetSpin()
        );
    };

    std::println("O type main sequence: {}\nB type main sequence: {}\nA type main sequence: {}\nF type main sequence: {}\nG type main sequence: {}\nK type main sequence: {}\nM type main sequence: {}",
        MainSequence[kTypeO], MainSequence[kTypeB], MainSequence[kTypeA], MainSequence[kTypeF], MainSequence[kTypeG], MainSequence[kTypeK], MainSequence[kTypeM]);
    std::println("O type subgiants: {}\nB type subgiants: {}\nA type subgiants: {}\nF type subgiants: {}\nG type subgiants: {}\nK type subgiants: {}\nM type subgiants: {}",
        Subgiants[kTypeO], Subgiants[kTypeB], Subgiants[kTypeA], Subgiants[kTypeF], Subgiants[kTypeG], Subgiants[kTypeK], Subgiants[kTypeM]);
    std::println("O type giants: {}\nB type giants: {}\nA type giants: {}\nF type giants: {}\nG type giants: {}\nK type giants: {}\nM type giants: {}",
        Giants[kTypeO], Giants[kTypeB], Giants[kTypeA], Giants[kTypeF], Giants[kTypeG], Giants[kTypeK], Giants[kTypeM]);
    std::println("O type bright giants: {}\nB type bright giants: {}\nA type bright giants: {}\nF type bright giants: {}\nG type bright giants: {}\nK type bright giants: {}\nM type bright giants: {}",
        BrightGiants[kTypeO], BrightGiants[kTypeB], BrightGiants[kTypeA], BrightGiants[kTypeA], BrightGiants[kTypeF], BrightGiants[kTypeG], BrightGiants[kTypeM]);
    std::println("O type supergiants: {}\nB type supergiants: {}\nA type supergiants: {}\nF type supergiants: {}\nG type supergiants: {}\nK type supergiants: {}\nM type supergiants: {}",
        Supergiants[kTypeO], Supergiants[kTypeB], Supergiants[kTypeA], Supergiants[kTypeF], Supergiants[kTypeG], Supergiants[kTypeK], Supergiants[kTypeM]);
    std::println("O type hypergiants: {}\nB type hypergiants: {}\nA type hypergiants: {}\nF type hypergiants: {}\nG type hypergiants: {}\nK type hypergiants: {}\nM type hypergiants: {}",
        Hypergiants[kTypeO], Hypergiants[kTypeB], Hypergiants[kTypeA], Hypergiants[kTypeF], Hypergiants[kTypeG], Hypergiants[kTypeK], Hypergiants[kTypeM]);
    std::println("Wolf-Rayet stars: {}", WolfRayet);
    std::println("White dwarfs: {}\nNeutron stars: {}\nBlack holes: {}", WhiteDwarfs, NeutronStars, BlackHoles);

    std::println("");
    std::println("{}", FormatTitle());

    std::println("Most luminous main sequence star: luminosity: {}", MostLuminousMainSequence.LuminositySol);
    std::println("{}", FormatInfo(MostLuminousMainSequence.Star));
    std::println("Most luminous Wolf-Rayet star: luminosity: {}", MostLuminousWolfRayet.LuminositySol);
    std::println("{}", FormatInfo(MostLuminousWolfRayet.Star));
    std::println("Most luminous subgiant star: luminosity: {}", MostLuminousSubgiant.LuminositySol);
    std::println("{}", FormatInfo(MostLuminousSubgiant.Star));
    std::println("Most luminous giant star: luminosity: {}", MostLuminousGiant.LuminositySol);
    std::println("{}", FormatInfo(MostLuminousGiant.Star));
    std::println("Most luminous bright giant star: luminosity: {}", MostLuminousBrightGiant.LuminositySol);
    std::println("{}", FormatInfo(MostLuminousBrightGiant.Star));
    std::println("Most luminous supergiant star: luminosity: {}", MostLuminousSupergiant.LuminositySol);
    std::println("{}", FormatInfo(MostLuminousSupergiant.Star));
    std::println("Most luminous hypergiant star: luminosity: {}", MostLuminousHypergiant.LuminositySol);
    std::println("{}", FormatInfo(MostLuminousHypergiant.Star));
    std::println("");
    std::println("Most massive main sequence star: mass: {}", MostMassiveMainSequence.MassSol);
    std::println("{}", FormatInfo(MostMassiveMainSequence.Star));
    std::println("Most massive Wolf-Rayet star: mass: {}", MostMassiveWolfRayet.MassSol);
    std::println("{}", FormatInfo(MostMassiveWolfRayet.Star));
    std::println("Most massive subgiant star: mass: {}", MostMassiveSubgiant.MassSol);
    std::println("{}", FormatInfo(MostMassiveSubgiant.Star));
    std::println("Most massive giant star: mass: {}", MostMassiveGiant.MassSol);
    std::println("{}", FormatInfo(MostMassiveGiant.Star));
    std::println("Most massive bright giant star: mass: {}", MostMassiveBrightGiant.MassSol);
    std::println("{}", FormatInfo(MostMassiveBrightGiant.Star));
    std::println("Most massive supergiant star: mass: {}", MostMassiveSupergiant.MassSol);
    std::println("{}", FormatInfo(MostMassiveSupergiant.Star));
    std::println("Most massive hypergiant star: mass: {}", MostMassiveHypergiant.MassSol);
    std::println("{}", FormatInfo(MostMassiveHypergiant.Star));
    std::println("");
    std::println("Largest main sequence star: radius: {}", LargestMainSequence.RadiusSol);
    std::println("{}", FormatInfo(LargestMainSequence.Star));
    std::println("Largest Wolf-Rayet star: radius: {}", LargestWolfRayet.RadiusSol);
    std::println("{}", FormatInfo(LargestWolfRayet.Star));
    std::println("Largest subgiant star: radius: {}", LargestSubgiant.RadiusSol);
    std::println("{}", FormatInfo(LargestSubgiant.Star));
    std::println("Largest giant star: radius: {}", LargestGiant.RadiusSol);
    std::println("{}", FormatInfo(LargestGiant.Star));
    std::println("Largest bright giant star: radius: {}", LargestBrightGiant.RadiusSol);
    std::println("{}", FormatInfo(LargestBrightGiant.Star));
    std::println("Largest supergiant star: radius: {}", LargestSupergiant.RadiusSol);
    std::println("{}", FormatInfo(LargestSupergiant.Star));
    std::println("Largest hypergiant star: radius: {}", LargestHypergiant.RadiusSol);
    std::println("{}", FormatInfo(LargestHypergiant.Star));
    std::println("");
    std::println("Hottest main sequence star: Teff: {}", HottestMainSequence.Teff);
    std::println("{}", FormatInfo(HottestMainSequence.Star));
    std::println("Hottest Wolf-Rayet star: Teff: {}", HottestWolfRayet.Teff);
    std::println("{}", FormatInfo(HottestWolfRayet.Star));
    std::println("Hottest subgiant star: Teff: {}", HottestSubgiant.Teff);
    std::println("{}", FormatInfo(HottestSubgiant.Star));
    std::println("Hottest giant star: Teff: {}", HottestGiant.Teff);
    std::println("{}", FormatInfo(HottestGiant.Star));
    std::println("Hottest bright giant star: Teff: {}", HottestBrightGiant.Teff);
    std::println("{}", FormatInfo(HottestBrightGiant.Star));
    std::println("Hottest supergiant star: Teff: {}", HottestSupergiant.Teff);
    std::println("{}", FormatInfo(HottestSupergiant.Star));
    std::println("Hottest hypergiant star: Teff: {}", HottestHypergiant.Teff);
    std::println("{}", FormatInfo(HottestHypergiant.Star));
    std::println("");
    std::println("Oldest main sequence star: Age: {}", OldestMainSequence.Age);
    std::println("{}", FormatInfo(OldestMainSequence.Star));
    std::println("Oldest Wolf-Rayet star: Age: {}", OldestWolfRayet.Age);
    std::println("{}", FormatInfo(OldestWolfRayet.Star));
    std::println("Oldest subgiant star: Age: {}", OldestSubgiant.Age);
    std::println("{}", FormatInfo(OldestSubgiant.Star));
    std::println("Oldest giant star: Age: {}", OldestGiant.Age);
    std::println("{}", FormatInfo(OldestGiant.Star));
    std::println("Oldest bright giant star: Age: {}", OldestBrightGiant.Age);
    std::println("{}", FormatInfo(OldestBrightGiant.Star));
    std::println("Oldest supergiant star: Age: {}", OldestSupergiant.Age);
    std::println("{}", FormatInfo(OldestSupergiant.Star));
    std::println("Oldest hypergiant star: Age: {}", OldestHypergiant.Age);
    std::println("{}", FormatInfo(OldestHypergiant.Star));

    std::size_t TotalMainSequence = 0;
    for (std::size_t Count : MainSequence) {
        TotalMainSequence += Count;
    }

    std::println("");
    std::println("Total main sequence: {}", TotalMainSequence);
    std::println("Total main sequence rate: {}", TotalMainSequence / static_cast<double>(_StarPtrs.size()));
    std::println("Total O type star rate: {}", static_cast<double>(MainSequence[kTypeO]) / static_cast<double>(TotalMainSequence));
    std::println("Total B type star rate: {}", static_cast<double>(MainSequence[kTypeB]) / static_cast<double>(TotalMainSequence));
    std::println("Total A type star rate: {}", static_cast<double>(MainSequence[kTypeA]) / static_cast<double>(TotalMainSequence));
    std::println("Total F type star rate: {}", static_cast<double>(MainSequence[kTypeF]) / static_cast<double>(TotalMainSequence));
    std::println("Total G type star rate: {}", static_cast<double>(MainSequence[kTypeG]) / static_cast<double>(TotalMainSequence));
    std::println("Total K type star rate: {}", static_cast<double>(MainSequence[kTypeK]) / static_cast<double>(TotalMainSequence));
    std::println("Total M type star rate: {}", static_cast<double>(MainSequence[kTypeM]) / static_cast<double>(TotalMainSequence));
    std::println("Total Wolf-Rayet / O main star rate: {}", static_cast<double>(WolfRayet) / static_cast<double>(MainSequence[kTypeO]));
}

void Universe::GenerateSlots(float DistMin, std::size_t NumSamples, float Density) {
    float Radius     = std::pow((3.0f * NumSamples / (4 * kPi * Density)), (1.0f / 3.0f));
    float LeafSize   = std::pow((1.0f / Density), (1.0f / 3.0f));
    int   Exponent   = static_cast<int>(std::ceil(std::log2(Radius / LeafSize)));
    float LeafRadius = LeafSize * 0.5f;
    float RootRadius = LeafSize * static_cast<float>(std::pow(2, Exponent));

    _Octree = std::make_unique<Octree<StellarSystem>>(glm::vec3(0.0), RootRadius);
    _Octree->BuildEmptyTree(LeafRadius);

    _Octree->Traverse([Radius](NodeType& Node) -> void {
        if (Node.IsLeafNode() && glm::length(Node.GetCenter()) > Radius) {
            Node.SetValidation(false);
        }
    });

    std::size_t ValidLeafCount = _Octree->GetCapacity();
    std::vector<NodeType*> LeafNodes;

    auto CollectLeafNodes = [&LeafNodes](NodeType& Node) -> void {
        if (Node.IsLeafNode()) {
            LeafNodes.emplace_back(&Node);
        }
    };

    while (ValidLeafCount != NumSamples) {
        LeafNodes.clear();
        _Octree->Traverse(CollectLeafNodes);
        std::shuffle(LeafNodes.begin(), LeafNodes.end(), _RandomEngine);

        if (ValidLeafCount < NumSamples) {
            for (auto* Node : LeafNodes) {
                glm::vec3 Center = Node->GetCenter();
                float Distance = glm::length(Center);
                if (!Node->GetValidation() && Distance >= Radius && Distance <= Radius + LeafRadius) {
                    Node->SetValidation(true);
                    if (++ValidLeafCount == NumSamples) {
                        break;
                    }
                }
            }
        } else {
            for (auto* Node : LeafNodes) {
                glm::vec3 Center = Node->GetCenter();
                float Distance = glm::length(Center);
                if (Node->GetValidation() && Distance >= Radius - LeafRadius && Distance <= Radius) {
                    Node->SetValidation(false);
                    if (--ValidLeafCount == NumSamples) {
                        break;
                    }
                }
            }
        }
    }

    UniformRealDistribution<float> Dist(-LeafRadius, LeafRadius - DistMin);

    _Octree->Traverse([&Dist, LeafRadius, DistMin, this](NodeType& Node) -> void {
        if (Node.IsLeafNode() && Node.GetValidation()) {
            glm::vec3 Center(Node.GetCenter());
            glm::vec3 StellarSlot(
                Center.x + Dist.Generate(_RandomEngine),
                Center.y + Dist.Generate(_RandomEngine),
                Center.z + Dist.Generate(_RandomEngine)
            );
            Node.AddPoint(StellarSlot);
        }
    });

    NodeType* Node = _Octree->Find(glm::vec3(LeafRadius), [](const NodeType& Node) -> bool {
        return (Node.IsLeafNode());
    });

    Node->RemoveStorage();
    Node->AddPoint(glm::vec3(0.0f));
}

void Universe::OctreeLinkToStellarSystems(std::vector<Astro::Star>& Stars, std::vector<glm::vec3>& Slots) {
    std::size_t Index = 0;
    _Octree->Traverse([&](NodeType& Node) -> void {
        if (Node.IsLeafNode() && Node.GetValidation()) {
            for (const auto& Point : Node.GetPoints()) {
                float Theta = _CommonGenerator.Generate(_RandomEngine) * 2.0f * kPi;
                float Phi   = _CommonGenerator.Generate(_RandomEngine) * kPi;
                StellarSystem NewSystem;
                NewSystem.SetBaryPosition(Point);
                NewSystem.SetBaryNormal(glm::vec2(Theta, Phi));
                NewSystem.SetBaryDistanceRank(0);
                NewSystem.SetBaryName("");
                NewSystem.StarData().emplace_back(std::move(Stars[Index]));
                _StellarSystems.emplace_back(std::move(NewSystem));
                Node.AddLink(&_StellarSystems[Index]);
                Slots.emplace_back(Point);
                ++Index;
            }
        }
    });
}

void FillStellarSystem(StellarSystem& System) {}

_NPGS_END
