#include <chrono>
#include <iostream>
#include <random>
#include <vector>

#include "Npgs.h"

int main() {
    Npgs::Logger::Init();

    auto Pool = Npgs::ThreadPool::GetInstance(std::thread::hardware_concurrency());

    std::vector<Npgs::Modules::StellarGenerator> Generators;
    std::random_device RandomDevice;
    for (int i = 0; i != std::thread::hardware_concurrency(); ++i) {
        std::mt19937 RandomEngine(RandomDevice());
        std::uniform_int_distribution<int> UniformDistribution(1, 10000);
        Generators.emplace_back(UniformDistribution(RandomEngine));
    }

    auto Start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i != 10000; ++i) {
        Pool->Commit([i, &Generators]() -> void {
            int ThreadId = i % Generators.size();
            Generators[ThreadId].GenStar();
        });
    }

    Pool->Terminate();
    Npgs::ThreadPool::Destroy();

    auto End = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> Duration = End - Start;

    std::println("Benchmark completed in {} seconds.", Duration.count());

    return 0;
}

