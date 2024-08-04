#include "Npgs.h"

using namespace Npgs;

int main() {
    //using MesaCsv = Npgs::Assets::Csv<std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string>;

    //std::vector<std::string> StrVec{ "star_age", "star_mass", "star_mdot", "log_L", "log_Teff", "log_R", "log_surf_z", "v_wind_Km_per_s", "log_center_T", "log_center_Rho", "phase", "x" };
    //Npgs::Assets::Csv<double, 12> Test("Assets/Models/MESA/[Fe_H]=+0.0/001.000Ms_track.csv", StrVec);
    ////Npgs::Assets::AssetManager::AddAsset("MESA", std::make_shared<MesaCsv>(Test));
    ////auto Ptr = Npgs::Assets::AssetManager::GetAsset<MesaCsv>("MESA");

    //try {
    //    auto SurroundingRows = Test.FindSurroundingValues("star_age", 1.77636786e+03);
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

    Modules::StellarGenerator sg;
    sg.GetActuallyMesaData({
        {"Test", {0.0, 0.0, 0.0}, 0},
        3.90e+02,
        0.2,
        196.0
    });

    return 0;
}