#include "Npgs.h"

int main() {
    using namespace Npgs;
    using namespace Npgs::Modules;

    Logger::Init();

    std::println("Enter the star count:");
    std::size_t StarCount = 0;
    std::cin >> StarCount;

    std::println("Enter the seed:");
    unsigned Seed = 0;
    std::cin >> Seed;

    Universe Space(Seed, StarCount);
    Space.FillUniverse();
    Space.CountStars();

    std::system("pause");

    return 0;
}