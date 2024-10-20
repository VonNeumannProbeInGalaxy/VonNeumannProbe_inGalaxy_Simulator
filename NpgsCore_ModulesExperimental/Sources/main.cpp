#define ENABLE_LOGGER
// #include "Npgs.h"

#include <xstdafx.h>

import Core.Logger;
import Core.ThreadPool;
import Module.StellarGenerator;
import Universe;

int main() {
    using namespace Npgs;
    using namespace Npgs::Modules;

    Logger::Init();
    ThreadPool::Init();

    std::println("Enter the system count:");
    std::size_t StarCount = 0;
    std::cin >> StarCount;

    std::println("Enter the seed:");
    unsigned Seed = 0;
    std::cin >> Seed;

    Universe Space(Seed, StarCount);
    Space.FillUniverse();
    Space.CountStars();

    //std::random_device rd;
    //unsigned seed = 3141777642;//800323521;//472035744;//2442947453;
    //std::println("Seed: {}", seed);
    //StellarGenerator sg({ seed });
    //StellarGenerator::BasicProperties b{ 4.6e9f, 0.0f, 1.0f };
    //auto s = sg.GenerateStar(b);

    //OrbitalGenerator og({ seed });
    //StellarSystem ss;
    //ss.StarData().push_back(std::make_unique<Astro::Star>(s));
    //og.GenerateOrbitals(ss);

    //std::vector<StellarSystem> sss(200000);

    //auto start = std::chrono::high_resolution_clock::now();
    //for (int i = 0; i != 20000; ++i) {
    //    try {
    //        sss[i].StarData().emplace_back(std::make_unique<Astro::Star>(s));
    //        og.GeneratePlanets(sss[i]);
    //    } catch (std::exception& e) {
    //        NpgsCoreError(e.what());
    //    }
    //}
    //auto end = std::chrono::high_resolution_clock::now();
    //std::chrono::duration<double> elapsed = end - start;
    //std::println("Elapsed time: {}s", elapsed.count());

    //std::system("pause");

    return 0;
}