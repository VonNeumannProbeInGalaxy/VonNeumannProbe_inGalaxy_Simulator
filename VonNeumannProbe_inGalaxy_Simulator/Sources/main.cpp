#include <chrono>
#include <print>
#include <random>
#include <thread>
#include <vector>

#include "Npgs.h"

int main() {
    Npgs::Logger::Init();

    std::println("Mass\tRadius\tAge\tClass\tFeH\tLum\tAbsMagn\tTeff\tCoreTemp\tCoreDensity\tMassLoss\tWindSpeed\tPhase\tLifetime");

    //auto Pool = Npgs::ThreadPool::GetInstance(std::thread::hardware_concurrency());

    //std::vector<Npgs::Modules::StellarGenerator> Generators;
    //std::random_device RandomDevice;
    //for (int i = 0; i != std::thread::hardware_concurrency(); ++i) {
    //    std::mt19937 RandomEngine(RandomDevice());
    //    std::uniform_int_distribution<int> UniformDistribution(1, 10000);
    //    Generators.emplace_back(UniformDistribution(RandomEngine));
    //}

    //auto Start = std::chrono::high_resolution_clock::now();

    //for (int i = 0; i != 200000; ++i) {
    //    Pool->Commit([i, &Generators]() -> void {
    //        int ThreadId = i % Generators.size();
    //        auto Star = Generators[ThreadId].GenStar();
    //        if (Star.GetMass() / Npgs::kSolarMass > 20.0) {
    //            std::println("{:.2f}\t{:.2f}\t{:.2E}\t{}\t{:.2f}\t{:.2f}\t{:.2f}\t{:.2f}\t{:.2E}\t{:.2f}\t{:.2E}\t{:.2f}\t{}\t{:.2E}",
    //                Star.GetMass() / Npgs::kSolarMass,
    //                Star.GetRadius() / Npgs::kSolarRadius,
    //                Star.GetAge(),
    //                Star.GetSpectralType(),
    //                Star.GetFeH(),
    //                Star.GetLuminosity() / Npgs::kSolarLuminosity,
    //                Star.GetAbsoluteMagnitude(),
    //                Star.GetTeff(),
    //                Star.GetCoreTemp(),
    //                Star.GetCoreDensity(),
    //                Star.GetStellarWindMassLossRate(),
    //                Star.GetStellarWindSpeed(),
    //                static_cast<int>(Star.GetEvolutionPhase()),
    //                Star.GetLifetime()
    //            );
    //        }
    //    });
    //}

    //Pool->Terminate();
    //Npgs::ThreadPool::Destroy();

    //auto End = std::chrono::high_resolution_clock::now();
    //std::chrono::duration<double> Duration = End - Start;

    //std::println("Benchmark completed in {} seconds.", Duration.count());

    Npgs::Modules::StellarGenerator Gen(42);
    //auto Star = Gen.GenStar({ {}, 2.16e6, 0.34, 52.75 });
    auto Star = Gen.GenStar({ {}, 9.5e6, 0.0, 20 });
    std::println("{:.2f}\t{:.2f}\t{:.2E} {}\t{:.2f}\t{:.2f}\t{:.2f}\t{:.2f}\t{:.2E}\t{:.2f}\t{:.2E}\t{:.2f}\t{}\t{:.2E}",
        Star.GetMass() / Npgs::kSolarMass,
        Star.GetRadius() / Npgs::kSolarRadius,
        Star.GetAge(),
        Star.GetSpectralType(),
        Star.GetFeH(),
        Star.GetLuminosity() / Npgs::kSolarLuminosity,
        Star.GetAbsoluteMagnitude(),
        Star.GetTeff(),
        Star.GetCoreTemp(),
        Star.GetCoreDensity(),
        Star.GetStellarWindMassLossRate(),
        Star.GetStellarWindSpeed(),
        static_cast<int>(Star.GetEvolutionPhase()),
        Star.GetLifetime()
    );

    return 0;
}
