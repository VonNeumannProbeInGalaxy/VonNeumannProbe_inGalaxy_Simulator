#define ENABLE_LOGGER
// #include "Npgs.h"

#include <xstdafx.h>
#include "Npgs.h"

int main() {
    using namespace Npgs;
    using namespace Npgs::Module;

    Logger::Init();
    ThreadPool::Init();

    std::println("Enter the system count:");
    std::size_t StarCount = 0;
    std::cin >> StarCount;

    std::println("Enter the seed:");
    unsigned Seed = 0;
    std::cin >> Seed;

    try {
        Universe Space(Seed, StarCount);
        Space.FillUniverse();
        Space.CountStars();
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    //using enum StellarGenerator::GenerateOption;
    //StellarGenerator sg({ 42 });
    //StellarGenerator::BasicProperties bp;
    //bp.Age = 9.89589e+09;
    //bp.FeH = -1.26;
    //bp.InitialMassSol = 39.98;
    //bp.TypeOption = kGiant;
    //auto s = sg.GenerateStar(bp);

    //std::cout << s.GetAge() << " " << s.GetFeH() << " " << s.GetMass() << " " << s.GetStellarClass().ToString() << std::endl;


    //StellarGenerator sg({ Seed }, kNormal, 1.38e10f, 0.075f);
    //std::vector<StellarGenerator::BasicProperties> bp;
    //for (; --StarCount; bp.push_back(sg.GenerateBasicProperties()));

    //std::ofstream SingleStar("SingleStar.csv");
    //std::ofstream BinaryFirstStar("BinaryFirstStar.csv");
    //std::ofstream BinarySecondStar("BinarySecondStar.csv");

    //for (auto& e : bp) {
    //    if (e.bIsSingleStar) {
    //        SingleStar << e.InitialMassSol << ",";
    //    } else if (e.Option == kBinaryFirstStar) {
    //        BinaryFirstStar << e.InitialMassSol << ",";
    //    } else if (e.Option == kBinarySecondStar) {
    //        BinarySecondStar << e.InitialMassSol << ",";
    //    }
    //}

    //std::random_device rd;
    //unsigned seed = 3731860369;//3141777642;//800323521;//472035744;//2442947453;
    //std::println("Seed: {}", seed);
    //StellarGenerator sg({ seed });
    //StellarGenerator::BasicProperties b1{ 5e9f, 0.0f, 0.1f };
    //StellarGenerator::BasicProperties b2{ 5e9f, 0.0f, 0.3f };
    //auto s1 = sg.GenerateStar(b1);
    //auto s2 = sg.GenerateStar(b2);

    //s1.SetIsSingleStar(false);
    //s2.SetIsSingleStar(false);

    //OrbitalGenerator og({ seed });
    //StellarSystem ss;
    //ss.StarData().push_back(std::make_unique<Astro::Star>(s1));
    //ss.StarData().push_back(std::make_unique<Astro::Star>(s2));
    //og.GenerateOrbitals(ss);

    //std::vector<StellarSystem> sss(200000);

    //auto start = std::chrono::high_resolution_clock::now();
    //for (int i = 0; i != 20000; ++i) {
    //    try {
    //        sss[i].StarData().emplace_back(std::make_unique<Astro::Star>(s));
    //        og.GenerateOrbitals(sss[i]);
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