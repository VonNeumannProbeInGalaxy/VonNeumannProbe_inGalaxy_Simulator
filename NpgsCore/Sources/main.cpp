#define ENABLE_LOGGER
#include "Npgs.h"

int main() {
    using namespace Npgs;
    using namespace Npgs::Modules;

    Logger::Init();

    //std::println("Enter the star count:");
    //std::size_t StarCount = 0;
    //std::cin >> StarCount;

    //std::println("Enter the seed:");
    //unsigned Seed = 0;
    //std::cin >> Seed;

    //Universe Space(Seed, StarCount);
    //Space.FillUniverse();
    //Space.CountStars();

    std::random_device rd;
    unsigned seed = 3473319233;//2325366524//396747571;//2643113656;//1388840952;//1179680896;// 1039595763;// 874872082;
    std::println("Seed: {}", seed);
    StellarGenerator sg({ seed });
    StellarGenerator::BasicProperties b{ 2e10, 0.0f, 1.0f };
    auto s = sg.GenerateStar(b);

    StellarSystem ss;
    ss.StarData().emplace_back(s);

    OrbitalGenerator og({ seed });
    try {
        og.GeneratePlanets(ss);;
    } catch (std::exception& e) {
        NpgsCoreError(e.what());
    }

    return 0;
}