#include "Npgs.h"

static std::string FormatTitle() {
    return std::format(
        "{:>6} {:>8} {:>8} {:7} {:>5} {:>13} {:>7} {:>8} {:>8} {:>11} {:>8} {:>9} {:>5} {:>8} {:>8} {:>8} {:>15} {:>9} {:>8}",
        "Mass", "Radius", "Age", "Class", "FeH", "Lum", "AbsMagn", "Teff", "CoreTemp", "CoreDensity", "Mdot", "WindSpeed", "Phase", "SurfZ", "SurfNuc", "SurfVol", "Magnetic", "Lifetime", "Spin");
}

static std::string FormatInfo(const Npgs::AstroObject::Star& Star) {
    return std::format("{:6.2f} {:8.2f} {:8.2E} {:7} {:5.2f} {:13.4f} {:7.2f} {:8.1f} {:8.2E} {:11.2E} {:8.2E} {:9} {:5} {:8.2E} {:8.2E} {:8.2E} {:15.5f} {:9.2E} {:8.2E}",
        Star.GetMass() / Npgs::kSolarMass,
        Star.GetRadius() / Npgs::kSolarRadius,
        Star.GetAge(),
        Star.GetStellarClass().ToString(),
        Star.GetFeH(),
        Star.GetLuminosity() / Npgs::kSolarLuminosity,
        Npgs::kSolarAbsoluteMagnitude - 2.5 * std::log10(Star.GetLuminosity() / Npgs::kSolarLuminosity),
        Star.GetTeff(),
        Star.GetCoreTemp(),
        Star.GetCoreDensity(),
        Star.GetStellarWindMassLossRate() * Npgs::kYearInSeconds / Npgs::kSolarMass,
        static_cast<int>(std::round(Star.GetStellarWindSpeed())),
        static_cast<int>(Star.GetEvolutionPhase()),
        Star.GetSurfaceZ(),
        Star.GetSurfaceEnergeticNuclide(),
        Star.GetSurfaceVolatiles(),
        Star.GetMagneticField(),
        Star.GetLifetime(),
        Star.GetSpin()
    );
}

static void PrintInfo(std::ofstream& Filename, const Npgs::AstroObject::Star& Star) {
    Filename << FormatInfo(Star) << std::endl;
}

int main() {
    using namespace Npgs;
    using namespace Npgs::Modules;

    Logger::Init();

    std::println("Enter the star count:");
    std::size_t StarCount = 0;
    std::cin >> StarCount;

    std::println("Enter the seed:");
    unsigned Seed = 0;
    std::cin >> Seed;

    Universe Space(Seed, StarCount);
    auto& Stars = Space.FillUniverse();

    std::size_t MMainSequence = 0;
    std::size_t KMainSequence = 0;
    std::size_t GMainSequence = 0;
    std::size_t FMainSequence = 0;
    std::size_t AMainSequence = 0;
    std::size_t BMainSequence = 0;
    std::size_t OMainSequence = 0;

    std::size_t MSubgiants = 0;
    std::size_t KSubgiants = 0;
    std::size_t GSubgiants = 0;
    std::size_t FSubgiants = 0;
    std::size_t ASubgiants = 0;
    std::size_t BSubgiants = 0;
    std::size_t OSubgiants = 0;

    std::size_t MGiants = 0;
    std::size_t KGiants = 0;
    std::size_t GGiants = 0;
    std::size_t FGiants = 0;
    std::size_t AGiants = 0;
    std::size_t BGiants = 0;
    std::size_t OGiants = 0;

    std::size_t MBrightGiants = 0;
    std::size_t KBrightGiants = 0;
    std::size_t GBrightGiants = 0;
    std::size_t FBrightGiants = 0;
    std::size_t ABrightGiants = 0;
    std::size_t BBrightGiants = 0;
    std::size_t OBrightGiants = 0;

    std::size_t MSupergiants = 0;
    std::size_t KSupergiants = 0;
    std::size_t GSupergiants = 0;
    std::size_t FSupergiants = 0;
    std::size_t ASupergiants = 0;
    std::size_t BSupergiants = 0;
    std::size_t OSupergiants = 0;

    std::size_t MHypergiants = 0;
    std::size_t KHypergiants = 0;
    std::size_t GHypergiants = 0;
    std::size_t FHypergiants = 0;
    std::size_t AHypergiants = 0;
    std::size_t BHypergiants = 0;
    std::size_t OHypergiants = 0;

    std::size_t WolfRayet    = 0;
    std::size_t WhiteDwarfs  = 0;
    std::size_t NeutronStars = 0;
    std::size_t BlackHoles   = 0;

    struct MostLuminosity {
        double Luminosity{};
        AstroObject::Star Star;
    };

    MostLuminosity MostLumOBStar;
    MostLuminosity MostLumWolfRayet;
    MostLuminosity MostLumSubgiant;
    MostLuminosity MostLumGiant;
    MostLuminosity MostLumBrightGiant;
    MostLuminosity MostLumSupergiant;
    MostLuminosity MostLumHypergiant;

    for (auto& Star : Stars) {
        const StellarClass& Class = Star.GetStellarClass();
        double Lum = 0.0;
        StellarClass::StarType StarType = Class.GetStarType();
        if (StarType != StellarClass::StarType::kNormalStar) {
            switch (StarType) {
            case StellarClass::StarType::kBlackHole:
                ++BlackHoles;
                break;
            case StellarClass::StarType::kNeutronStar:
                ++NeutronStars;
                break;
            case StellarClass::StarType::kWhiteDwarf:
                ++WhiteDwarfs;
                break;
            default:
                break;
            }

            continue;
        }

        StellarClass::SpectralType SpectralType = Class.Data();
        if (SpectralType.LuminosityClass == StellarClass::LuminosityClass::kLuminosity_Unknown) {
            if (SpectralType.HSpectralClass == StellarClass::SpectralClass::kSpectral_WC ||
                SpectralType.HSpectralClass == StellarClass::SpectralClass::kSpectral_WN ||
                SpectralType.HSpectralClass == StellarClass::SpectralClass::kSpectral_WO) {
                ++WolfRayet;
                Lum = Star.GetLuminosity() / kSolarLuminosity;
                if (MostLumWolfRayet.Luminosity < Lum) {
                    MostLumWolfRayet.Luminosity = Lum;
                    MostLumWolfRayet.Star = Star;
                }

                continue;
            }
        }

        if (SpectralType.LuminosityClass == StellarClass::LuminosityClass::kLuminosity_0 ||
            SpectralType.LuminosityClass == StellarClass::LuminosityClass::kLuminosity_IaPlus) {

            switch (SpectralType.HSpectralClass) {
            case StellarClass::SpectralClass::kSpectral_O:
                ++OHypergiants;
                break;
            case StellarClass::SpectralClass::kSpectral_B:
                ++BHypergiants;
                break;
            case StellarClass::SpectralClass::kSpectral_A:
                ++AHypergiants;
                break;
            case StellarClass::SpectralClass::kSpectral_F:
                ++FHypergiants;
                break;
            case StellarClass::SpectralClass::kSpectral_G:
                ++GHypergiants;
                break;
            case StellarClass::SpectralClass::kSpectral_K:
                ++KHypergiants;
                break;
            case StellarClass::SpectralClass::kSpectral_M:
                ++MHypergiants;
                break;
            }

            Lum = Star.GetLuminosity() / kSolarLuminosity;
            if (MostLumHypergiant.Luminosity < Lum) {
                MostLumHypergiant.Luminosity = Lum;
                MostLumHypergiant.Star = Star;
            }

            continue;
        }

        if (SpectralType.LuminosityClass == StellarClass::LuminosityClass::kLuminosity_Ia ||
            SpectralType.LuminosityClass == StellarClass::LuminosityClass::kLuminosity_Iab ||
            SpectralType.LuminosityClass == StellarClass::LuminosityClass::kLuminosity_Ib) {

            switch (SpectralType.HSpectralClass) {
            case StellarClass::SpectralClass::kSpectral_O:
                ++OSupergiants;
                break;
            case StellarClass::SpectralClass::kSpectral_B:
                ++BSupergiants;
                break;
            case StellarClass::SpectralClass::kSpectral_A:
                ++ASupergiants;
                break;
            case StellarClass::SpectralClass::kSpectral_F:
                ++FSupergiants;
                break;
            case StellarClass::SpectralClass::kSpectral_G:
                ++GSupergiants;
                break;
            case StellarClass::SpectralClass::kSpectral_K:
                ++KSupergiants;
                break;
            case StellarClass::SpectralClass::kSpectral_M:
                ++MSupergiants;
                break;
            }

            Lum = Star.GetLuminosity() / kSolarLuminosity;
            if (MostLumSupergiant.Luminosity < Lum) {
                MostLumSupergiant.Luminosity = Lum;
                MostLumSupergiant.Star = Star;
            }

            continue;
        }

        if (SpectralType.LuminosityClass == StellarClass::LuminosityClass::kLuminosity_II) {

            switch (SpectralType.HSpectralClass) {
            case StellarClass::SpectralClass::kSpectral_O:
                ++OBrightGiants;
                break;
            case StellarClass::SpectralClass::kSpectral_B:
                ++BBrightGiants;
                break;
            case StellarClass::SpectralClass::kSpectral_A:
                ++ABrightGiants;
                break;
            case StellarClass::SpectralClass::kSpectral_F:
                ++FBrightGiants;
                break;
            case StellarClass::SpectralClass::kSpectral_G:
                ++GBrightGiants;
                break;
            case StellarClass::SpectralClass::kSpectral_K:
                ++KBrightGiants;
                break;
            case StellarClass::SpectralClass::kSpectral_M:
                ++MBrightGiants;
                break;
            }

            Lum = Star.GetLuminosity() / kSolarLuminosity;
            if (MostLumBrightGiant.Luminosity < Lum) {
                MostLumBrightGiant.Luminosity = Lum;
                MostLumBrightGiant.Star = Star;
            }

            continue;
        }

        if (SpectralType.LuminosityClass == StellarClass::LuminosityClass::kLuminosity_III) {

            switch (SpectralType.HSpectralClass) {
            case StellarClass::SpectralClass::kSpectral_O:
                ++OGiants;
                break;
            case StellarClass::SpectralClass::kSpectral_B:
                ++BGiants;
                break;
            case StellarClass::SpectralClass::kSpectral_A:
                ++AGiants;
                break;
            case StellarClass::SpectralClass::kSpectral_F:
                ++FGiants;
                break;
            case StellarClass::SpectralClass::kSpectral_G:
                ++GGiants;
                break;
            case StellarClass::SpectralClass::kSpectral_K:
                ++KGiants;
                break;
            case StellarClass::SpectralClass::kSpectral_M:
                ++MGiants;
                break;
            }

            Lum = Star.GetLuminosity() / kSolarLuminosity;
            if (MostLumGiant.Luminosity < Lum) {
                MostLumGiant.Luminosity = Lum;
                MostLumGiant.Star = Star;
            }

            continue;
        }

        if (SpectralType.LuminosityClass == StellarClass::LuminosityClass::kLuminosity_IV) {

            switch (SpectralType.HSpectralClass) {
            case StellarClass::SpectralClass::kSpectral_O:
                ++OSubgiants;
                break;
            case StellarClass::SpectralClass::kSpectral_B:
                ++BSubgiants;
                break;
            case StellarClass::SpectralClass::kSpectral_A:
                ++ASubgiants;
                break;
            case StellarClass::SpectralClass::kSpectral_F:
                ++FSubgiants;
                break;
            case StellarClass::SpectralClass::kSpectral_G:
                ++GSubgiants;
                break;
            case StellarClass::SpectralClass::kSpectral_K:
                ++KSubgiants;
                break;
            case StellarClass::SpectralClass::kSpectral_M:
                ++MSubgiants;
                break;
            }

            Lum = Star.GetLuminosity() / kSolarLuminosity;
            if (MostLumSubgiant.Luminosity < Lum) {
                MostLumSubgiant.Luminosity = Lum;
                MostLumSubgiant.Star = Star;
            }

            continue;
        }

        if (SpectralType.LuminosityClass == StellarClass::LuminosityClass::kLuminosity_V) {
            switch (SpectralType.HSpectralClass) {
            case StellarClass::SpectralClass::kSpectral_O:
                ++OMainSequence;
                Lum = Star.GetLuminosity() / kSolarLuminosity;
                if (MostLumOBStar.Luminosity < Lum) {
                    MostLumOBStar.Luminosity = Lum;
                    MostLumOBStar.Star = Star;
                }
                break;
            case StellarClass::SpectralClass::kSpectral_B:
                ++BMainSequence;
                Lum = Star.GetLuminosity() / kSolarLuminosity;
                if (MostLumOBStar.Luminosity < Lum) {
                    MostLumOBStar.Luminosity = Lum;
                    MostLumOBStar.Star = Star;
                }
                break;
            case StellarClass::SpectralClass::kSpectral_A:
                ++AMainSequence;
                break;
            case StellarClass::SpectralClass::kSpectral_F:
                ++FMainSequence;
                break;
            case StellarClass::SpectralClass::kSpectral_G:
                ++GMainSequence;
                break;
            case StellarClass::SpectralClass::kSpectral_K:
                ++KMainSequence;
                break;
            case StellarClass::SpectralClass::kSpectral_M:
                ++MMainSequence;
                break;
            }

            continue;
        }
    }

    std::println("O type main sequence: {}\nB type main sequence: {}\nA type main sequence: {}\nF type main sequence: {}\nG type main sequence: {}\nK type main sequence: {}\nM type main sequence: {}",
        OMainSequence, BMainSequence, AMainSequence, FMainSequence, GMainSequence, KMainSequence, MMainSequence);
    std::println("O type subgiants: {}\nB type subgiants: {}\nA type subgiants: {}\nF type subgiants: {}\nG type subgiants: {}\nK type subgiants: {}\nM type subgiants: {}",
        OSubgiants, BSubgiants, ASubgiants, FSubgiants, GSubgiants, KSubgiants, MSubgiants);
    std::println("O type giants: {}\nB type giants: {}\nA type giants: {}\nF type giants: {}\nG type giants: {}\nK type giants: {}\nM type giants: {}",
        OGiants, BGiants, AGiants, FGiants, GGiants, KGiants, MGiants);
    std::println("O type bright giants: {}\nB type bright giants: {}\nA type bright giants: {}\nF type bright giants: {}\nG type bright giants: {}\nK type bright giants: {}\nM type bright giants: {}",
        OBrightGiants, BBrightGiants, ABrightGiants, FBrightGiants, GBrightGiants, KBrightGiants, MBrightGiants);
    std::println("O type supergiants: {}\nB type supergiants: {}\nA type supergiants: {}\nF type supergiants: {}\nG type supergiants: {}\nK type supergiants: {}\nM type supergiants: {}",
        OSupergiants, BSupergiants, ASupergiants, FSupergiants, GSupergiants, KSupergiants, MSupergiants);
    std::println("O type hypergiants: {}\nB type hypergiants: {}\nA type hypergiants: {}\nF type hypergiants: {}\nG type hypergiants: {}\nK type hypergiants: {}\nM type hypergiants: {}",
        OHypergiants, BHypergiants, AHypergiants, FHypergiants, GHypergiants, KHypergiants, MHypergiants);
    std::println("Wolf-Rayet stars: {}", WolfRayet);
    std::println("White dwarfs: {}\nNeutron stars: {}\nBlack holes: {}", WhiteDwarfs, NeutronStars, BlackHoles);
    std::println("Most luminosity type O B main sequence star: Lum: {}", MostLumOBStar.Luminosity);
    std::println("{}\n{}", FormatTitle(), FormatInfo(MostLumOBStar.Star));
    std::println("Most luminosity Wolf-Rayet star:Lum: {}", MostLumWolfRayet.Luminosity);
    std::println("{}\n{}", FormatTitle(), FormatInfo(MostLumWolfRayet.Star));
    std::println("Most luminosity subgiant star:Lum: {}", MostLumSubgiant.Luminosity);
    std::println("{}\n{}", FormatTitle(), FormatInfo(MostLumSubgiant.Star));
    std::println("Most luminosity giant star:Lum: {}", MostLumGiant.Luminosity);
    std::println("{}\n{}", FormatTitle(), FormatInfo(MostLumGiant.Star));
    std::println("Most luminosity bright giant star:Lum: {}", MostLumBrightGiant.Luminosity);
    std::println("{}\n{}", FormatTitle(), FormatInfo(MostLumBrightGiant.Star));
    std::println("Most luminosity supergiant star:Lum: {}", MostLumSupergiant.Luminosity);
    std::println("{}\n{}", FormatTitle(), FormatInfo(MostLumSupergiant.Star));
    std::println("Most luminosity hypergiant star:Lum: {}", MostLumHypergiant.Luminosity);
    std::println("{}\n{}", FormatTitle(), FormatInfo(MostLumHypergiant.Star));

    double TotalMainSequence = OMainSequence + BMainSequence + AMainSequence + FMainSequence + GMainSequence + KMainSequence + MMainSequence;
    std::println("Total main sequence: {}", OMainSequence + BMainSequence + AMainSequence + FMainSequence + GMainSequence + KMainSequence + MMainSequence);
    std::println("Total main sequence rate: {}", TotalMainSequence / static_cast<double>(StarCount));
    std::println("Total O type star rate: {}", static_cast<double>(OMainSequence) / static_cast<double>(TotalMainSequence));
    std::println("Total B type star rate: {}", static_cast<double>(BMainSequence) / static_cast<double>(TotalMainSequence));
    std::println("Total A type star rate: {}", static_cast<double>(AMainSequence) / static_cast<double>(TotalMainSequence));
    std::println("Total F type star rate: {}", static_cast<double>(FMainSequence) / static_cast<double>(TotalMainSequence));
    std::println("Total G type star rate: {}", static_cast<double>(GMainSequence) / static_cast<double>(TotalMainSequence));
    std::println("Total K type star rate: {}", static_cast<double>(KMainSequence) / static_cast<double>(TotalMainSequence));
    std::println("Total M type star rate: {}", static_cast<double>(MMainSequence) / static_cast<double>(TotalMainSequence));
    std::println("Total Wolf-Rayet / O main star rate: {}", static_cast<double>(WolfRayet) / static_cast<double>(OMainSequence));

    std::system("pause");

    return 0;
}