#define ENABLE_LOGGER
#include "Npgs.h"

int main() {
    using namespace Npgs;
    using namespace Npgs::Modules;

    Logger::Init();
    ThreadPool::Init();

    std::println("Enter the star count:");
    std::size_t StarCount = 0;
    std::cin >> StarCount;

    std::println("Enter the seed:");
    unsigned Seed = 0;
    std::cin >> Seed;

    Universe Space(Seed, StarCount);
    Space.FillUniverse();
    Space.CountStars();

    //std::random_device rd;
    //unsigned seed = rd();//147895346;//153597433;//2652587822;//2325366524//396747571;//2643113656;//1388840952;//1179680896;// 1039595763;// 874872082;
    //std::println("Seed: {}", seed);
    //StellarGenerator sg({ seed }, Modules::StellarGenerator::GenOption::kNormal);
    //StellarGenerator::BasicProperties b{ 4.6e9f, 0.0f, 0.08f };
    //auto s = sg.GenerateStar(b);

    //OrbitalGenerator og({ seed }, 1e21f, 1.0f);
    //// og.GeneratePlanets(ss);
    //std::vector<StellarSystem> sss(200000);

    //auto start = std::chrono::high_resolution_clock::now();
    //for (int i = 0; i != 200000; ++i) {
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