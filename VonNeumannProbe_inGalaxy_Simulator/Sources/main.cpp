#include "Engine/Core/AssetLoader/Csv.hpp"

int main() {
    Npgs::Csv<std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string> t("test.csv", "star_age" ,"star_mass", "star_mdot", "log_L", "log_Teff", "log_R", "log_surf_z", "v_wind_Km_per_s", "log_center_T", "log_center_Rho", "phase");
    std::vector<std::string> v = t.GetDataArray("star_age", "1.76636786e+03");
    for (const auto& i : v) {
        std::println("{}", i);
    }

    return 0;
}