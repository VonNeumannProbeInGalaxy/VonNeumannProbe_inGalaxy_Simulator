#include "Npgs.h"

using namespace Npgs;

int main() {
    //using MistCsv = Npgs::Assets::Csv<std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string>;

    //std::vector<std::string> StrVec{ "star_age", "star_mass", "star_mdot", "log_L", "log_Teff", "log_R", "log_surf_z", "v_wind_Km_per_s", "log_center_T", "log_center_Rho", "phase", "x" };
    //Npgs::Assets::Csv<double, 12> Test("Assets/Models/MIST/[Fe_H]=+0.0/001.000Ms_track.csv", StrVec);
    ////Npgs::Assets::AssetManager::AddAsset("MIST", std::make_shared<MistCsv>(Test));
    ////auto Ptr = Npgs::Assets::AssetManager::GetAsset<MistCsv>("MIST");

    //try {
    //    auto SurroundingRows = Test.FindSurroundingValues("star_age", 3.65541916e+03);
    //    const auto& LowerRow = SurroundingRows.first;
    //    const auto& UpperRow = SurroundingRows.second;

    //    std::cout << "Lower Row: ";
    //    for (const auto& Value : LowerRow) {
    //        std::cout << Value << " ";
    //    }
    //    std::cout << std::endl;

    //    std::cout << "Upper Row: ";
    //    for (const auto& Value : UpperRow) {
    //        std::cout << Value << " ";
    //    }
    //    std::cout << std::endl;
    //} catch (const std::out_of_range& e) {
    //    std::cerr << "Error: " << e.what() << std::endl;
    //}

    Npgs::Logger::Init();

    Modules::StellarGenerator sg(42);
    sg.GenStar();

    return 0;
}