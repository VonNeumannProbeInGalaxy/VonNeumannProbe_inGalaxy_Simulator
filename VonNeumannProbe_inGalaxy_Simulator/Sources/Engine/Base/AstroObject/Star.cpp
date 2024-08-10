#include "Star.h"

_NPGS_BEGIN
_ASTROOBJECT_BEGIN

Star::Star(const CelestialBody::BasicProperties& StarBasicProperties, const ExtendedProperties& StarExtraProperties)
    : CelestialBody(StarBasicProperties), _StarExtraProperties(StarExtraProperties)
{}

const std::unordered_map<int, std::string> Star::_kSpectralTemp_O{ { 54000, "O2" }, { 44900, "O3" }, { 42900, "O4" }, { 41400, "O5" }, { 39500, "O6" }, { 37100, "O7" }, { 35100, "O8" }, { 33300, "O9" } };
const std::unordered_map<int, std::string> Star::_kSpectralTemp_B{ { 31400, "B0" }, { 26000, "B1" }, { 20600, "B2" }, { 17000, "B3" }, { 16400, "B4" }, { 15700, "B5" }, { 14500, "B6" }, { 14000, "B7" }, { 12300, "B8" }, { 10700, "B9" } };
const std::unordered_map<int, std::string> Star::_kSpectralTemp_A{ { 9700,  "A0" }, { 9300,  "A1" }, { 8800,  "A2" }, { 8600,  "A3" }, { 8250,  "A4" }, { 8100,  "A5" }, { 7910,  "A6" }, { 7760,  "A7" }, { 7590,  "A8" }, { 7400,  "A9" } };
const std::unordered_map<int, std::string> Star::_kSpectralTemp_F{ { 7220,  "F0" }, { 7020,  "F1" }, { 6820,  "F2" }, { 6750,  "F3" }, { 6670,  "F4" }, { 6550,  "F5" }, { 6350,  "F6" }, { 6280,  "F7" }, { 6180,  "F8" }, { 6050,  "F9" } };
const std::unordered_map<int, std::string> Star::_kSpectralTemp_G{ { 5930,  "G0" }, { 5860,  "G1" }, { 5770,  "G2" }, { 5720,  "G3" }, { 5680,  "G4" }, { 5660,  "G5" }, { 5600,  "G6" }, { 5550,  "G7" }, { 5480,  "G8" }, { 5380,  "G9" } };
const std::unordered_map<int, std::string> Star::_kSpectralTemp_K{ { 5270,  "K0" }, { 5170,  "K1" }, { 5100,  "K2" }, { 4830,  "K3" }, { 4600,  "K4" }, { 4440,  "K5" }, { 4300,  "K6" }, { 4100,  "K7" }, { 3990,  "K8" }, { 3930,  "K9" } };
const std::unordered_map<int, std::string> Star::_kSpectralTemp_M{ { 3850,  "M0" }, { 3660,  "M1" }, { 3560,  "M2" }, { 3430,  "M3" }, { 3210,  "M4" }, { 3060,  "M5" }, { 2810,  "M6" }, { 2680,  "M7" }, { 2570,  "M8" }, { 2380,  "M9" } };
const std::unordered_map<int, std::string> Star::_kSpectralTemp_WC{ { 117000, "WC4" }, { 83000, "WC5" }, { 78000, "WC6" }, { 71000, "WC7" }, { 60000, "WC8" }, { 44000, "WC9" } };
const std::unordered_map<int, std::string> Star::_kSpectralTemp_WN{ { 141000, "WN2" }, { 85000, "WN3" }, { 70000, "WN4" }, { 60000, "WN5" }, { 56000, "WN6" }, { 50000, "WN7" }, { 45000, "WN8" } };
const std::unordered_map<int, std::string> Star::_kSpectralTemp_WO{ { 200000, "WO2" } };
const std::unordered_map<int, std::string> Star::_kSpectralTemp_WNxh{ { 50000, "WN5h" }, { 45000, "WN6h" }, { 43000, "WN7h" }, { 40000, "WN8h" }, { 35000, "WN9h" } };

const std::unordered_map<int, std::unordered_map<int, std::string>> Star::_kInitialCommonTemp{
    { 54000,  Star::_kSpectralTemp_O },
    { 31400,  Star::_kSpectralTemp_B },
    { 9700,   Star::_kSpectralTemp_A },
    { 7220,   Star::_kSpectralTemp_F },
    { 5930,   Star::_kSpectralTemp_G },
    { 5270,   Star::_kSpectralTemp_K },
    { 3850,   Star::_kSpectralTemp_M }
};

const std::unordered_map<int, std::unordered_map<int, std::string>> Star::_kInitialWolfRayetTemp{
    { 141000, Star::_kSpectralTemp_WN },
    { 117000, Star::_kSpectralTemp_WC },
    { 200000, Star::_kSpectralTemp_WO },
    { 50000,  Star::_kSpectralTemp_WNxh }
};

const std::unordered_map<int, int> Star::_kLuminosity{

};

_ASTROOBJECT_END
_NPGS_END
