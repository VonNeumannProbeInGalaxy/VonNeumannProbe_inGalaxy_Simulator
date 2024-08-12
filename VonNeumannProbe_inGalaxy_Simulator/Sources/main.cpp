#include <cstdlib>
#include <chrono>
#include <fstream>
#include <print>
#include <random>
#include <thread>
#include <vector>

#include "Npgs.h"

#ifdef NDEBUG
#define MULTITHREAD
#endif

int main() {
    Npgs::Logger::Init();

#ifdef MULTITHREAD
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

    //std::ofstream Mass("Mass.txt", std::ios::out);
    //for (auto& Future : Futures) {
    //    Future.wait();
    //    auto Data = Future.get();
    //    Mass << Data.Mass << ",";
    //}

    auto End = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> Duration = End - Start;

    std::println("Star basic data generation completed in {} seconds.", Duration.count());

    Start = std::chrono::high_resolution_clock::now();

    std::vector<std::future<Npgs::AstroObject::Star>> StarFutures;
    for (int i = 0; i != MaxStars; ++i) {
        StarFutures.emplace_back(Pool->Commit([i, &Generators, &Futures]() -> Npgs::AstroObject::Star {
            int ThreadId = i % Generators.size();
            auto Properties = Futures[i].get();
            return Generators[ThreadId].GenerateStar(Properties);
        }));
    }

    for (auto& Future : StarFutures) {
        Future.wait(); // 确保所有的 future 都已经完成
    }

    std::system("cls");
    std::println("{:<10} {:<10} {:<10} {:<7} {:<5} {:<10} {:<10} {:<10} {:<10} {:<10} {:<10} {:<10} {:<5} {:<10} {:<10} {:<10}",
        "Mass", "Radius", "Age", "Class", "FeH", "Lum", "AbsMagn", "Teff", "CoreTemp", "CoreDensity", "MassLoss", "WindSpeed", "Phase", "SurfaceH1", "Magnetic", "Lifetime");

    for (auto& Future : StarFutures) {
        auto Star = Future.get();
        if (Star.GetMass() / Npgs::kSolarMass > 1) {
            std::println("{:<10.2f} {:<10.2f} {:<10.2E} {:<7} {:6.2f} {:<10.2f} {:10.2f} {:<10.2f} {:<10.2E} {:<12.2f} {:<10.2E} {:<12.2f} {:<5} {:<10.2f} {:<10.5f} {:<10.2E}",
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
                Star.GetStellarWindMassLossRate(),
                Star.GetStellarWindSpeed(),
                static_cast<int>(Star.GetEvolutionPhase()),
                Star.GetSurfaceH1(),
                Star.GetMagneticField(),
                Star.GetLifetime()
            );
        }
    }

    Pool->Terminate();
    Npgs::ThreadPool::Destroy();

    End = std::chrono::high_resolution_clock::now();
    Duration = End - Start;

    std::println("Interpolate completed in {} seconds.", Duration.count());
    std::system("pause");
#else
    std::println("{:<10} {:<10} {:<10} {:<7} {:<5} {:<10} {:<10} {:<10} {:<10} {:<10} {:<10} {:<10} {:<5} {:<10} {:<10} {:<10}",
        "Mass", "Radius", "Age", "Class", "FeH", "Lum", "AbsMagn", "Teff", "CoreTemp", "CoreDensity", "MassLoss", "WindSpeed", "Phase", "SurfaceH1", "Magnetic", "Lifetime");

    Npgs::Modules::StellarGenerator Gen(42, 0.1);
    for (int i = 0; i != 100; ++i) {
        auto Basic = Gen.GenBasicProperties();
        std::println("{}, {}, {}", Basic.Age, Basic.FeH, Basic.Mass);
        auto Star = Gen.GenerateStar(Basic);
        if (Star.GetSurfaceH1() < 0.6) {
            std::println("{:<10.2f} {:<10.2f} {:<10.2E} {:<7} {:<6.2f} {:<10.2f} {:<10.2f} {:<10.2f} {:<10.2E} {:<10.2f} {:<10.2E} {:<12.2f} {:<5} {:<10.2f} {:<10.5f} {:<10.2E}",
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
                Star.GetStellarWindMassLossRate(),
                Star.GetStellarWindSpeed(),
                static_cast<int>(Star.GetEvolutionPhase()),
                Star.GetSurfaceH1(),
                Star.GetMagneticField(),
                Star.GetLifetime()
            );
        }
    }

    //auto Star = Gen.GenerateStar({ {}, 2.40E+09, -0.71, 0.14585089078151559 });
#endif

    return 0;
}
