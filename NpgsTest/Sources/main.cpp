#include "Npgs.h"

#ifdef NDEBUG
#define MULTITHREAD
#endif

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
        Star.GetAbsoluteMagnitude(),
        Star.GetTeff(),
        Star.GetCoreTemp(),
        Star.GetCoreDensity(),
        Star.GetStellarWindMassLossRate() * Npgs::kYearInSeconds / Npgs::kSolarMass,
        static_cast<int>(std::round(Star.GetStellarWindSpeed())),
        static_cast<int>(Star.GetEvolutionPhase()),
        Star.GetSurfaceFeH(),
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
    Output << FormatTitle() << std::endl;

#ifdef MULTITHREAD
    int MaxThread = std::thread::hardware_concurrency();
    auto Pool = Npgs::ThreadPool::GetInstance(MaxThread);

    std::println("Npgs run with {} threads.", MaxThread);
    std::println("Current work directory: {}.", std::filesystem::current_path().string());

    int MaxStars = 0;
    std::println("Enter the star count:");
    std::cin >> MaxStars;

    auto Start = std::chrono::high_resolution_clock::now();

    std::vector<Npgs::Modules::StellarGenerator> Generators;
    std::random_device RandomDevice;
    for (int i = 0; i != MaxThread; ++i) {
        Generators.emplace_back(RandomDevice(), 0.075);
        // Generators.emplace_back(i * 42, 0.075);
    }

    auto End = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> Duration = End - Start;

    std::println("MIST init completed in {} seconds.", Duration.count());

    std::vector<std::future<Npgs::Modules::StellarGenerator::BasicProperties>> Futures;
    for (int i = 0; i != MaxStars; ++i) {
        Futures.emplace_back(Pool->Commit([&, i]() -> Npgs::Modules::StellarGenerator::BasicProperties {
            int ThreadId = i % Generators.size();
            return Generators[ThreadId].GenBasicProperties();
        }));
    }

    for (auto& Future : Futures) {
        Future.wait();
    }

    Start = std::chrono::high_resolution_clock::now();

    std::vector<std::future<Npgs::AstroObject::Star>> StarFutures;
    for (int i = 0; i != MaxStars; ++i) {
        StarFutures.emplace_back(Pool->Commit([&, i]() -> Npgs::AstroObject::Star {
            int ThreadId = i % Generators.size();
            auto Properties = Futures[i].get();
            return Generators[ThreadId].GenerateStar(Properties);
        }));
    }

    for (auto& Future : StarFutures) {
        Future.wait();
    }

    for (auto& Future : StarFutures) {
        auto Star = Future.get();
        //if (!std::isnan(Star.GetRadius()) && Star.GetMass() / Npgs::kSolarMass > 30) {
            //PrintInfo(Output, Star);
        //}
    }

    Pool->Terminate();
    Npgs::ThreadPool::Destroy();

    End = std::chrono::high_resolution_clock::now();
    Duration = End - Start;

    std::println("Benchmark completed in {} seconds.", Duration.count());
    std::system("pause");
#else
    Npgs::Modules::StellarGenerator Generator(42, 0.075);
    for (int i = 0; i != 1000; ++i) {
        auto Properties = Generator.GenBasicProperties();
        auto Star = Generator.GenerateStar(Properties);
        std::println("Basic properties - Age: {}, FeH: {}, Mass: {}", Properties.Age, Properties.FeH, Properties.Mass);
        PrintInfo(Output, Star);
    }
#endif

    Output.close();

    return 0;
}