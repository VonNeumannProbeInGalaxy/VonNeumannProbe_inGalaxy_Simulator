#include <chrono>
#include <print>
#include <random>
#include <thread>
#include <vector>

#include "Npgs.h"

//#define MULTITHREAD

int main() {
    Npgs::Logger::Init();

    std::println("Mass\tRadius\tAge\tClass\tFeH\tLum\tAbsMagn\tTeff\tCoreTemp\tCoreDensity\tMassLoss\tWindSpeed\tPhase\tProgress\tLifetime");
#ifdef MULTITHREAD
    int MaxThread = std::thread::hardware_concurrency();
    auto Pool = Npgs::ThreadPool::GetInstance(MaxThread);

    std::vector<Npgs::Modules::StellarGenerator> Generators;
    std::random_device RandomDevice;
    for (int i = 0; i != MaxThread; ++i) {
        std::mt19937 RandomEngine(RandomDevice());
        std::uniform_int_distribution<int> UniformDistribution(1, 10000);
        Generators.emplace_back(UniformDistribution(RandomEngine));
    }

    auto Start = std::chrono::high_resolution_clock::now();

    std::vector<std::future<Npgs::Modules::StellarGenerator::BasicProperties>> Futures;
    for (int i = 0; i != 1000000; ++i) {
        Futures.emplace_back(Pool->Commit([i, &Generators]() -> Npgs::Modules::StellarGenerator::BasicProperties {
            int ThreadId = i % Generators.size();
            return Generators[ThreadId].GenBasicProperties();
        }));
    }

    auto End = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> Duration = End - Start;

    std::println("Benchmark completed in {} seconds.", Duration.count());

    Start = std::chrono::high_resolution_clock::now();

    std::vector<std::future<Npgs::AstroObject::Star>> StarFutures;
    for (int i = 0; i != 1000000; ++i) {
        StarFutures.emplace_back(Pool->Commit([i, &Generators, &Futures]() -> Npgs::AstroObject::Star {
            int ThreadId = i % Generators.size();
            auto Properties = Futures[i].get();
            return Generators[ThreadId].GenStar(Properties);
        }));
    }

    //for (auto& Future : StarFutures) {
    //    Future.get();
    //}

    //for (int i = 0; i != 200000; ++i) {
    //    Pool->Commit([i, &Generators]() -> void {
    //        int ThreadId = i % Generators.size();
    //        /*auto Star = */Generators[ThreadId].GenStar();
    //        //if (Star.GetMass() / Npgs::kSolarMass > 20.0) {
    //        //    std::println("{:.2f}\t{:.2f}\t{:.2E}\t{}\t{:.2f}\t{:.2f}\t{:.2f}\t{:.2f}\t{:.2E}\t{:.2f}\t{:.2E}\t{:.2f}\t{}\t{:.2E}",
    //        //        Star.GetMass() / Npgs::kSolarMass,
    //        //        Star.GetRadius() / Npgs::kSolarRadius,
    //        //        Star.GetAge(),
    //        //        Star.GetSpectralType(),
    //        //        Star.GetFeH(),
    //        //        Star.GetLuminosity() / Npgs::kSolarLuminosity,
    //        //        Star.GetAbsoluteMagnitude(),
    //        //        Star.GetTeff(),
    //        //        Star.GetCoreTemp(),
    //        //        Star.GetCoreDensity(),
    //        //        Star.GetStellarWindMassLossRate(),
    //        //        Star.GetStellarWindSpeed(),
    //        //        static_cast<int>(Star.GetEvolutionPhase()),
    //        //        Star.GetLifetime()
    //        //    );
    //        //}
    //    });
    //}

    Pool->Terminate();
    Npgs::ThreadPool::Destroy();

    End = std::chrono::high_resolution_clock::now();
    Duration = End - Start;

    std::println("Benchmark completed in {} seconds.", Duration.count());
#else
    Npgs::Modules::StellarGenerator Gen(42);
    for (int i = 0; i != 100; ++i) {
        auto Star = Gen.GenStar();
        //if (Star.GetMass() / Npgs::kSolarMass > 10.0) {
            std::println("{:.2f}\t{:.2f}\t{:.2E} {}\t{:.2f}\t{:.2f}\t{:.2f}\t{:.2f}\t{:.2E}\t{:.2f}\t{:.2E}\t{:.2f}\t{} \t{:.5f}\t{:.2E}",
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
                Star.GetEvolutionProgress(),
                Star.GetLifetime()
            );
        //}
    }
    //auto Star = Gen.GenStar({ {}, 9.5e6, 0.0, 20 });
    //auto Star = Gen.GenStar({ {}, 2.5e7, 0.0, 10 });
#endif

    return 0;
}
