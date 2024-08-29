#include "Npgs.h"

static std::string FormatTitle() {
    return std::format("{:>6}, {:>8}, {:>8}, {:7}, {:>5}, {:>13}, {:>7}, {:>8}, {:>8}, {:>11}, {:>8}, {:>9}, {:>5}, {:>8}, {:>8}, {:>8}, {:>15}, {:>9}, {:>8}, {:>12}",
        "Mass", "Radius", "Age", "Class", "FeH", "Lum", "AbsMagn", "Teff", "CoreTemp", "CoreDensity", "Mdot", "WindSpeed", "Phase", "SurfZ", "SurfNuc", "SurfVol", "Magnetic", "Lifetime", "Spin", "MinCoilMass");
}

static std::string FormatInfo(const Npgs::AstroObject::Star& Star) {
    return std::format("{:6.2f}, {:8.2f}, {:8.2E}, {:7}, {:5.2f}, {:13.4f}, {:7.2f}, {:8.1f}, {:8.2E}, {:11.2E}, {:8.2E}, {:9}, {:5}, {:8.2E}, {:8.2E}, {:8.2E}, {:15.5f}, {:9.2E}, {:8.2E}, {:12.2E}",
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
        Star.GetSpin(),
        Star.GetMinCoilMass()
    );
}

static void PrintInfo(std::ofstream& Filename, const Npgs::AstroObject::Star& Star) {
    Filename << FormatInfo(Star) << std::endl;
}

int main() {
    Npgs::Logger::Init();
    std::ofstream Output("Output.csv", std::ios::out);

    std::println("Enter the star count.");

    std::size_t c = 0;
    std::cin >> c;

    unsigned s = 0;
    std::println("Enter the seed.");
    std::cin >> s;

    Npgs::Universe u(s, c);
    auto& st = u.FillUniverse();

    Output << FormatTitle() << std::endl;
    // Output << "Class," << "Lum" << std::endl;
    for (auto& Star : st) {
        //Output << Star.GetStellarClass().ToString() << "," << Star.GetLuminosity() / Npgs::kSolarLuminosity << std::endl;
        if (Star.GetStellarClass().ToString().find('M') != std::string::npos)
            PrintInfo(Output, Star);
    }

    //Npgs::Modules::StellarGenerator g(42);
    //Npgs::Modules::StellarGenerator::BasicProperties b{ {}, 5776762.9187316513, 0.0, 37.203746724061673 };
    //g.GenerateStar(b);

    return 0;
}