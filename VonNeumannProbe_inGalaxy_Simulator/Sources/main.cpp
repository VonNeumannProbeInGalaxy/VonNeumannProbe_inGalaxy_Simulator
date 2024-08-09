#include <chrono>
#include <print>
#include <random>
#include <thread>
#include <vector>

#include "Npgs.h"

int main() {
    Npgs::Logger::Init();

    std::println("Mass Radius Age FeH Lum EffTemp CoreTemp CoreDensity WindSpeed MassLoss Phase");

    auto Pool = Npgs::ThreadPool::GetInstance(std::thread::hardware_concurrency());

    std::vector<Npgs::Modules::StellarGenerator> Generators;
    std::random_device RandomDevice;
    for (int i = 0; i != std::thread::hardware_concurrency(); ++i) {
        std::mt19937 RandomEngine(RandomDevice());
        std::uniform_int_distribution<int> UniformDistribution(1, 10000);
        Generators.emplace_back(UniformDistribution(RandomEngine));
    }

    auto Start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i != 200000; ++i) {
        Pool->Commit([i, &Generators]() -> void {
            int ThreadId = i % Generators.size();
            auto Star = Generators[ThreadId].GenStar();
            if (Star.GetMass() > 50.0) {
                std::println("{:.2f}\t{:.2f}\t{:.2E}\t{:.2f}\t{:.2f}\t{:.2f}\t{:.2E}\t{:.2f}\t{:.2f}\t{:.2E}\t{}",
                    Star.GetMass(),
                    Star.GetRadius(),
                    Star.GetAge(),
                    Star.GetFeH(),
                    Star.GetLuminosity(),
                    Star.GetEffectiveTemp(),
                    Star.GetCoreTemp(),
                    Star.GetCoreDensity(),
                    Star.GetStellarWindSpeed(),
                    Star.GetStellarWindMassLossRate(),
                    static_cast<int>(Star.GetEvolutionPhase()));
            }
        });
    }

    Pool->Terminate();
    Npgs::ThreadPool::Destroy();

    auto End = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> Duration = End - Start;

    std::println("Benchmark completed in {} seconds.", Duration.count());

    return 0;
}
