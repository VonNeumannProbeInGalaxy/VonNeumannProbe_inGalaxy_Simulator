#include "Star.h"

_NPGS_BEGIN
_ASTROOBJECT_BEGIN

Star::Star(const CelestialBody::BasicProperties& StarBasicProperties, const ExtendedProperties& StarExtraProperties)
    : CelestialBody(StarBasicProperties), _StarExtraProperties(StarExtraProperties)
{}

const std::vector<std::pair<int, int>> Star::_kSpectralSubclassMap_O{ { 54000, 2 }, { 44900, 3 }, { 42900, 4 }, { 41400, 5 }, { 39500, 6 }, { 37100, 7 }, { 35100, 8 }, { 33300, 9 }, { 31400, 10 } };
const std::vector<std::pair<int, int>> Star::_kSpectralSubclassMap_B{ { 31400, 0 }, { 26000, 1 }, { 20600, 2 }, { 17000, 3 }, { 16400, 4 }, { 15700, 5 }, { 14500, 6 }, { 14000, 7 }, { 12300, 8 }, { 10700, 9 }, { 9700, 10 } };
const std::vector<std::pair<int, int>> Star::_kSpectralSubclassMap_A{ { 9700,  0 }, { 9300,  1 }, { 8800,  2 }, { 8600,  3 }, { 8250,  4 }, { 8100,  5 }, { 7910,  6 }, { 7760,  7 }, { 7590,  8 }, { 7400,  9 }, { 7220, 10 } };
const std::vector<std::pair<int, int>> Star::_kSpectralSubclassMap_F{ { 7220,  0 }, { 7020,  1 }, { 6820,  2 }, { 6750,  3 }, { 6670,  4 }, { 6550,  5 }, { 6350,  6 }, { 6280,  7 }, { 6180,  8 }, { 6050,  9 }, { 5930, 10 } };
const std::vector<std::pair<int, int>> Star::_kSpectralSubclassMap_G{ { 5930,  0 }, { 5860,  1 }, { 5770,  2 }, { 5720,  3 }, { 5680,  4 }, { 5660,  5 }, { 5600,  6 }, { 5550,  7 }, { 5480,  8 }, { 5380,  9 }, { 5270, 10 } };
const std::vector<std::pair<int, int>> Star::_kSpectralSubclassMap_K{ { 5270,  0 }, { 5170,  1 }, { 5100,  2 }, { 4830,  3 }, { 4600,  4 }, { 4440,  5 }, { 4300,  6 }, { 4100,  7 }, { 3990,  8 }, { 3930,  9 }, { 3850, 10 } };
const std::vector<std::pair<int, int>> Star::_kSpectralSubclassMap_M{ { 3850,  0 }, { 3660,  1 }, { 3560,  2 }, { 3430,  3 }, { 3210,  4 }, { 3060,  5 }, { 2810,  6 }, { 2680,  7 }, { 2570,  8 }, { 2380,  9 }, { 2000, 10 } };
const std::vector<std::pair<int, int>> Star::_kSpectralSubclassMap_WC{ { 117000, 4 }, { 83000, 5 }, { 78000, 6 }, { 71000, 7 }, { 60000, 8 }, { 44000, 9 }, { 40000, 10 } };
const std::vector<std::pair<int, int>> Star::_kSpectralSubclassMap_WN{ { 141000, 2 }, { 85000, 3 }, { 70000, 4 }, { 60000, 5 }, { 56000, 6 }, { 50000, 7 }, { 45000, 8 }, { 40000, 10 } };
const std::vector<std::pair<int, int>> Star::_kSpectralSubclassMap_WO{ { 200000, 2 }, { 100000, 3 }, { 80000, 4} };
const std::vector<std::pair<int, int>> Star::_kSpectralSubclassMap_WNxh{ { 50000, 5 }, { 45000, 6 }, { 43000, 7 }, { 40000, 8 }, { 35000, 9 }, { 30000, 10 } };

const std::vector<std::pair<int, std::vector<std::pair<int, int>>>> Star::_kInitialCommonMap{
    { 54000,  Star::_kSpectralSubclassMap_O },
    { 31400,  Star::_kSpectralSubclassMap_B },
    { 9700,   Star::_kSpectralSubclassMap_A },
    { 7220,   Star::_kSpectralSubclassMap_F },
    { 5930,   Star::_kSpectralSubclassMap_G },
    { 5270,   Star::_kSpectralSubclassMap_K },
    { 3850,   Star::_kSpectralSubclassMap_M },
    { 0, {} }
};

const std::vector<std::pair<int, std::vector<std::pair<int, int>>>> Star::_kInitialWolfRayetMap{
    { 200000, Star::_kSpectralSubclassMap_WO },
    { 141000, Star::_kSpectralSubclassMap_WN },
    { 117000, Star::_kSpectralSubclassMap_WC },
    { 0, {} }
};

const std::unordered_map<Star::Phase, ::Npgs::Modules::StellarClass::LuminosityClass> Star::_kLuminosity{
    { Star::Phase::kMainSequence,     ::Npgs::Modules::StellarClass::LuminosityClass::kLuminosity_V   },
    { Star::Phase::kRedGiant,         ::Npgs::Modules::StellarClass::LuminosityClass::kLuminosity_III },
    { Star::Phase::kCoreHeBurn,       ::Npgs::Modules::StellarClass::LuminosityClass::kLuminosity_IV  },
    { Star::Phase::kEarlyAgb,         ::Npgs::Modules::StellarClass::LuminosityClass::kLuminosity_II  },
    { Star::Phase::kThermalPulseAgb,  ::Npgs::Modules::StellarClass::LuminosityClass::kLuminosity_I   },
    { Star::Phase::kPostAgb,          ::Npgs::Modules::StellarClass::LuminosityClass::kLuminosity_I   }
};

_ASTROOBJECT_END
_NPGS_END
