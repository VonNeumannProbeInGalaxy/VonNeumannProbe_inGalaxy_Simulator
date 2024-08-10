#include <chrono>
#include <print>
#include <random>
#include <thread>
#include <vector>

#include "Npgs.h"

int main() {
    Npgs::Logger::Init();

    //std::println("Mass\tRadius\tAge\tClass\tFeH\tLum\tAbsMagn\tTeff\tCoreTemp\tCoreDensity\tMassLoss\tWindSpeed\tPhase\tLifetime");

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

    for (int i = 0; i != 200000; ++i) {
        Pool->Commit([i, &Generators]() -> void {
            int ThreadId = i % Generators.size();
            /*auto Star = */Generators[ThreadId].GenStar();
            //if (Star.GetMass() / Npgs::kSolarMass > 20.0) {
            //    std::println("{:.2f}\t{:.2f}\t{:.2E}\t{}\t{:.2f}\t{:.2f}\t{:.2f}\t{:.2f}\t{:.2E}\t{:.2f}\t{:.2E}\t{:.2f}\t{}\t{:.2E}",
            //        Star.GetMass() / Npgs::kSolarMass,
            //        Star.GetRadius() / Npgs::kSolarRadius,
            //        Star.GetAge(),
            //        Star.GetSpectralType(),
            //        Star.GetFeH(),
            //        Star.GetLuminosity() / Npgs::kSolarLuminosity,
            //        Star.GetAbsoluteMagnitude(),
            //        Star.GetTeff(),
            //        Star.GetCoreTemp(),
            //        Star.GetCoreDensity(),
            //        Star.GetStellarWindMassLossRate(),
            //        Star.GetStellarWindSpeed(),
            //        static_cast<int>(Star.GetEvolutionPhase()),
            //        Star.GetLifetime()
            //    );
            //}
        });
    }

    Pool->Terminate();
    Npgs::ThreadPool::Destroy();

    Npgs::Modules::StellarGenerator Gen1(42);
    Npgs::Modules::StellarGenerator Gen2(24);

    //// 创建两个线程，分别调用Gen1和Gen2的GenStar方法
    //std::thread Thread1([&Gen1]() {
    //    for (int i = 0; i != 10000; ++i) {
    //        auto Star = Gen1.GenStar();
    //        // 这里可以添加对Star的处理代码
    //    }
    //});

    //std::thread Thread2([&Gen2]() {
    //    for (int i = 0; i != 10000; ++i) {
    //        auto Star = Gen2.GenStar();
    //        // 这里可以添加对Star的处理代码
    //    }
    //});

    //// 等待两个线程完成
    //Thread1.join();
    //Thread2.join();

    //for (int i = 0; i != 10000; ++i) {
    //    Gen.GenStar();
    //}
    //auto Star = Gen.GenStar({ {}, 7604115.213191229,  0.005356931361485406, 46.48935841607761 });
    //auto Star = Gen.GenStar({ {}, 9.5e6, 0.0, 20 });
    //std::println("{:.2f}\t{:.2f}\t{:.2E} {}\t{:.2f}\t{:.2f}\t{:.2f}\t{:.2f}\t{:.2E}\t{:.2f}\t{:.2E}\t{:.2f}\t{}\t{:.2E}",
    //    Star.GetMass() / Npgs::kSolarMass,
    //    Star.GetRadius() / Npgs::kSolarRadius,
    //    Star.GetAge(),
    //    Star.GetSpectralType(),
    //    Star.GetFeH(),
    //    Star.GetLuminosity() / Npgs::kSolarLuminosity,
    //    Star.GetAbsoluteMagnitude(),
    //    Star.GetTeff(),
    //    Star.GetCoreTemp(),
    //    Star.GetCoreDensity(),
    //    Star.GetStellarWindMassLossRate(),
    //    Star.GetStellarWindSpeed(),
    //    static_cast<int>(Star.GetEvolutionPhase()),
    //    Star.GetLifetime()
    //);

    auto End = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> Duration = End - Start;

    std::println("Benchmark completed in {} seconds.", Duration.count());

    return 0;
}
