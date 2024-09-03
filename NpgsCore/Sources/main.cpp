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
    unsigned seed = rd();
    StellarGenerator sg({ seed });
    StellarGenerator::BasicProperties b{ 1e9f, 0.5f, 0.3f };
    auto s = sg.GenerateStar(b);

    StellarSystem ss;
    ss.StarData().emplace_back(s);

    OrbitalGenerator og({ seed });
    og.GeneratePlanets(ss);

    return 0;
}