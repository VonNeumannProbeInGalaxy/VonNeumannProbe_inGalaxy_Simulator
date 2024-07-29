#include "Engine/Core/AssetLoader/AssetManager.h"

int main() {
    using MesaCsv = Npgs::Csv<std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string>;

    MesaCsv Test("Assets/Models/MESA/[Fe_H]=+0.0/001.000Ms_track.csv", "star_age", "star_mass", "star_mdot", "log_L", "log_Teff", "log_R", "log_surf_z", "v_wind_Km_per_s", "log_center_T", "log_center_Rho", "phase");
    Npgs::AssetManager::AddAsset("MESA", std::make_shared<MesaCsv>(Test));
    auto Ptr = Npgs::AssetManager::GetAsset<MesaCsv>("MESA");

    auto Vec = Ptr->GetDataArray("star_age", "5.04303174e+09");
    for (auto& e : Vec) {
        println("{}", e);
    }

    return 0;
}