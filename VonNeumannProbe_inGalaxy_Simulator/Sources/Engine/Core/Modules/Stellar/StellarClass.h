#pragma once

#include <cstdint>
#include <string_view>

class StellarClass {
public:
    enum class StarType : std::uint32_t {
        kNormalStar         = 0,
        kWhiteDwarf         = 1,
        kNeutronStar        = 2,
        kBlackHole          = 3
    };

    enum class SpectralClass : std::uint32_t {
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
        kSpectral_Q         = 36, // 中子星
        kSpectral_X         = 27  // 黑洞
    };

    enum class LuminosityClass : std::uint32_t {
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

    enum class SpecialPeculiarities : std::uint32_t {
        kCode_Null,                   // 无
        kCode_Colon,                  // 不确定的光谱值
        kCode_Ellipsis,               // 未描述的光谱特殊性
        kCode_ExclamationMark,        // 特殊特殊性
        kCode_comp,                   // 复合光谱
        kCode_e,                      // 存在发射线
        kCode_eForbidden,             // 存在禁制发射线
        kCode_er,                     // “反向”发射线中心弱于边缘
        kCode_eq,                     // 带有 P Cygni 轮廓的发射线
        kCode_f,                      // N III 和 He II 发射线
        kCode_fStar,                  // N IV 4058Å 比 N III 4634Å, 4640Å 和 4642Å 线更强
        kCode_fPlus,                  // 除了 N III 线外，还发射 Si IV 4089Å 和 4116Å
        kCode_fQuestion,              // C III 4647–4650–4652Å 发射线与 N III 线强度相当
        kCode_fParentheses,           // N III 发射，He II 吸收弱或缺失
        kCode_fParenthesesPlus,       // 
        kCode_fDoubleParentheses,     // 显示强烈的 He II 吸收伴随着弱 N III 发射
        kCode_fDoubleParenthesesStar, // 
        kCode_h,                      // WR 星，带有氢发射线
        kCode_ha,                     // WR 星，氢在吸收和发射中都可见
        kCode_HeWk,                   // 弱氦线
        kCode_k,                      // 带有星际吸收特征的光谱
        kCode_m,                      // 增强的金属特征
        kCode_n,                      // 由于旋转导致的广泛（“星云状”）吸收
        kCode_nn,                     // 非常宽的吸收特征
        kCode_neb,                    // 混入了星云的光谱
        kCode_p,                      // 未指定的特殊性，特殊星
        kCode_pq,                     // 特殊光谱，类似于新星的光谱
        kCode_q,                      // P Cygni 轮廓
        kCode_s,                      // 狭窄（“锐利”）的吸收线
        kCode_ss,                     // 非常狭窄的线
        kCode_sh,                     // 壳层星特征
        kCode_var,                    // 变量光谱特征
        kCode_wl,                     // 弱线
        kCode_ElementSymbol,          // 指定元素的异常强光谱线
        kCode_z,                      // 表示在 468.6 nm 处异常强的电离氦线
    };

    struct SpectralType {
        SpectralClass SpectralClass;
        double Subclass;
        LuminosityClass LuminosityClass;
        SpecialPeculiarities SpecialMark;
    };

public:
    constexpr StellarClass() = default;
    StellarClass(StarType StarType, const SpectralType& SpectralType);

public:
    static StellarClass Parse(const std::string_view StellarClassStr);
    std::uint32_t Data() const;
    bool Load(std::uint32_t PackagedSpectralType);

public:
    StarType GetStarType() const {
        return _StarType;
    }

    SpectralClass GetSpectralClass() const {
        return _SpectralType.SpectralClass;
    }

    double GetSubclass() const {
        return _SpectralType.Subclass;
    }

    LuminosityClass GetLuminosityClass() const {
        return _SpectralType.LuminosityClass;
    }

private:
    StarType     _StarType{ StarType::kNormalStar };
    SpectralType _SpectralType{ SpectralClass::kSpectral_Unknown, 0.0, LuminosityClass::kLuminosity_Unknown };
};
