#include "Npgs.h"

#ifdef NDEBUG
#define MULTITHREAD
#endif

static void PrintTitle() {
    std::system("cls");
    std::println("{:>6} {:>8} {:>9} {:7} {:>6} {:>13} {:>10} {:>10} {:>10} {:>12} {:>10} {:>12} {:>5} {:>10} {:>10} {:>10}",
        "Mass", "Radius", "Age", "Class", "FeH", "Lum", "AbsMagn", "Teff", "CoreTemp", "CoreDensity", "MassLoss", "WindSpeed", "Phase", "SurfaceH1", "Magnetic", "Lifetime");
}

static void PrintInfo(const Npgs::AstroObject::Star& Star) {
    std::println("{:6.2f} {:8.2f} {:9.2E} {:7} {:6.2f} {:13.4f} {:10.2f} {:10} {:10.2E} {:12.2E} {:10.2E} {:12} {:5} {:10.2f} {:10.5f} {:10.2E}",
        Star.GetMass() / Npgs::kSolarMass,
        Star.GetRadius() / Npgs::kSolarRadius,
        Star.GetAge(),
        Star.GetStellarClass().ToString(),
        Star.GetFeH(),
        Star.GetLuminosity() / Npgs::kSolarLuminosity,
        Star.GetAbsoluteMagnitude(),
        static_cast<int>(std::round(Star.GetTeff())),
        Star.GetCoreTemp(),
        Star.GetCoreDensity(),
        Star.GetStellarWindMassLossRate(),
        static_cast<int>(std::round(Star.GetStellarWindSpeed())),
        static_cast<int>(Star.GetEvolutionPhase()),
        Star.GetSurfaceH1(),
        Star.GetMagneticField(),
        Star.GetLifetime()
    );
}

int main() {
    Npgs::Logger::Init();

#ifdef MULTITHREAD
    std::mutex Mutex;

    int MaxThread = std::thread::hardware_concurrency();
    auto Pool = Npgs::ThreadPool::GetInstance(MaxThread);

    int MaxStars = 0;
    std::println("Enter the star count:");
    std::cin >> MaxStars;

    std::vector<Npgs::Modules::StellarGenerator> Generators;
    std::random_device RandomDevice;
    for (int i = 0; i != MaxThread; ++i) {
        std::mt19937 RandomEngine(RandomDevice());
        std::uniform_int_distribution<int> UniformDistribution(1, 10000);
        Generators.emplace_back(UniformDistribution(RandomEngine), 0.1);
    }

    auto Start = std::chrono::high_resolution_clock::now();

    std::vector<std::future<Npgs::Modules::StellarGenerator::BasicProperties>> Futures;
    for (int i = 0; i != MaxStars; ++i) {
        Futures.emplace_back(Pool->Commit([i, &Generators]() -> Npgs::Modules::StellarGenerator::BasicProperties {
            int ThreadId = i % Generators.size();
            return Generators[ThreadId].GenBasicProperties();
        }));
    }

    for (auto& Future : Futures) {
        Future.wait();
    }

    auto End = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> Duration = End - Start;

    std::println("Star basic data generation completed in {} seconds.", Duration.count());

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

    //PrintTitle();

    //for (std::size_t i = 0; i < StarFutures.size(); ++i) {
    //    auto Star = StarFutures[i].get();
    //    if (Star.GetMass() / Npgs::kSolarMass >= 1)
    //        PrintInfo(Star);
    //}


    Pool->Terminate();
    Npgs::ThreadPool::Destroy();

    End = std::chrono::high_resolution_clock::now();
    Duration = End - Start;

    std::println("Interpolate completed in {} seconds.", Duration.count());
    // std::system("pause");
#else
    //PrintTitle();

    Npgs::Modules::StellarGenerator Generator(42, 0.075);
    for (int i = 0; i != 100; ++i) {
        auto Properties = Generator.GenBasicProperties();
        auto Star = Generator.GenerateStar(Properties);
        if (Star.GetMass() / Npgs::kSolarMass < 0.1) {
            std::println("Basic properties - Age: {}, FeH: {}, Mass: {}", Properties.Age, Properties.FeH, Properties.Mass);
            PrintInfo(Star);
        }
    }

    // auto Star = Generator.GenerateStar({ {}, 1.14e10, 0.114, 0.09 });
    // PrintInfo(Star);
#endif

    return 0;
}
