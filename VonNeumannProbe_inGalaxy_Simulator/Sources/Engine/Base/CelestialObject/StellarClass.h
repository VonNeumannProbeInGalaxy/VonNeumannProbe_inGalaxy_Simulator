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
        // 常规光谱型
        // ---------
        kSpectral_O         = 0,
        kSpectral_B         = 1,
        kSpectral_A         = 2,
        kSpectral_F         = 3,
        kSpectral_G         = 4,
        kSpectral_K         = 5,
        kSpectral_M         = 6,
        kSpectral_R         = 7,
        kSpectral_N         = 8,
        kSpectral_C         = 9,
        kSpectral_S         = 10,
        // 沃尔夫—拉叶星
        // -----------
        kSpectral_WC        = 11,
        kSpectral_WN        = 12,
        kSpectral_WNxh      = 13,
        kSpectral_WO        = 14,
        // 棕矮星
        // -----
        kSpectral_L         = 15,
        kSpectral_T         = 16,
        kSpectral_Y         = 17,
        // 恒星残骸
        // -------
        kSpectral_D         = 18, // 白矮星
        kSpectral_DA        = 19,
        kSpectral_DB        = 20,
        kSpectral_DC        = 21,
        kSpectral_DO        = 22,
        kSpectral_DQ        = 23,
        kSpectral_DX        = 24,
        kSpectral_DZ        = 25,
        kSpectral_DAB       = 26,
        kSpectral_DAO       = 27,
        kSpectral_DAZ       = 28,
        kSpectral_DBZ       = 29,
        kSpectral_Q         = 30, // 中子星
        kSpectral_X         = 31, // 黑洞
        kSpectral_Unknown   = 32
    };

    enum class LuminosityClass : std::int16_t {
        kLuminosity_0       = 0,
        kLuminosity_IaPlus  = 1,
        kLuminosity_Ia      = 2,
        kLuminosity_Ib      = 3,
        kLuminosity_Iab     = 4,
        kLuminosity_I       = 5,
        kLuminosity_II      = 6,
        kLuminosity_III     = 7,
        kLuminosity_IV      = 8,
        kLuminosity_V       = 9,
        kLuminosity_VI      = 10,
        kLuminosity_Unknown = 11
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
    StarType        _StarType;
    SpectralClass   _SpectralClass;
    double          _Subclass;
    LuminosityClass _LuminosityClass;
};
