#pragma once

#include <cstdint>
#include <string_view>

class StellarClass {
public:
    enum class StarType : std::int16_t {
        kNormalStar         = 0,
        kWhiteDwarf         = 1,
        kNeutronStar        = 2,
        kBlackHole          = 3
    };

    enum class SpectralClass : std::int16_t {
        kSpectral_Unknown   = 0,
        // 常规光谱型
        // ---------
        kSpectral_O         = 1,
        kSpectral_B         = 2,
        kSpectral_A         = 3,
        kSpectral_F         = 4,
        kSpectral_G         = 5,
        kSpectral_K         = 6,
        kSpectral_M         = 7,
        kSpectral_R         = 8,
        kSpectral_N         = 9,
        kSpectral_C         = 10,
        kSpectral_S         = 11,
        // 沃尔夫—拉叶星
        // -----------
        kSpectral_WC        = 12,
        kSpectral_WN        = 13,
        kSpectral_WNxh      = 14,
        kSpectral_WO        = 15,
        // 棕矮星
        // -----
        kSpectral_L         = 16,
        kSpectral_T         = 17,
        kSpectral_Y         = 18,
        // 恒星残骸
        // -------
        kSpectral_D         = 19, // 白矮星
        kSpectral_DA        = 20,
        kSpectral_DB        = 21,
        kSpectral_DC        = 22,
        kSpectral_DO        = 23,
        kSpectral_DQ        = 24,
        kSpectral_DX        = 25,
        kSpectral_DZ        = 26,
        kSpectral_DAB       = 27,
        kSpectral_DAO       = 28,
        kSpectral_DAZ       = 29,
        kSpectral_DBZ       = 30,
        kSpectral_Q         = 31, // 中子星
        kSpectral_X         = 32, // 黑洞
    };

    enum class LuminosityClass : std::int16_t {
        kLuminosity_Unknown = 0,
        kLuminosity_0       = 1,
        kLuminosity_IaPlus  = 2,
        kLuminosity_Ia      = 3,
        kLuminosity_Ib      = 4,
        kLuminosity_Iab     = 5,
        kLuminosity_I       = 6,
        kLuminosity_II      = 7,
        kLuminosity_III     = 8,
        kLuminosity_IV      = 9,
        kLuminosity_V       = 10,
        kLuminosity_VI      = 11
    };

public:
    constexpr StellarClass() = default;
    StellarClass(StarType StarType, SpectralClass SpectralClass, double Subclass, LuminosityClass LuminosityClass);

public:
    static StellarClass Parse(const std::string_view StellarClassStr);
    std::uint16_t Data() const;
    bool Load(std::uint16_t PackagedSpectralType);

public:
    StarType GetStarType() const {
        return _StarType;
    }

    SpectralClass GetSpectralClass() const {
        return _SpectralClass;
    }

    double GetSubclass() const {
        return _Subclass;
    }

    LuminosityClass GetLuminosityClass() const {
        return _LuminosityClass;
    }

private:
    StarType        _StarType{ StarType::kNormalStar };
    SpectralClass   _SpectralClass{ SpectralClass::kSpectral_Unknown };
    double          _Subclass = 0.0;
    LuminosityClass _LuminosityClass{ LuminosityClass::kLuminosity_Unknown };
};
