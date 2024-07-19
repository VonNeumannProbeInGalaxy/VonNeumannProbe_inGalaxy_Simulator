#include "StellarClass.h"

#include <cctype>
#include "Engine/Core/Assert.h"

enum class ParseState {
    kBegin,
    kEnd,
    // Parsing special star type
    // -------------------------
    kWolfRayetStar,
    kWhiteDwarf,
    kWhiteDwarfEx,
    kSubdwarfPerfix,
    // Parsing spectral type
    // ---------------------
    kSpectralClass,
    kSubclass,
    kSubclassDecimal,
    kSubclassDecimalNext,
    kLuminosityClass,
    kLuminosityClassI,
    kLuminosityClassIa,
    kLuminosityClassII,
    kLuminosityClassV
};

static ParseState ParseStarType(unsigned char Char, StellarClass::StarType& StarType, StellarClass::SpectralClass& SpectralClass, std::size_t& Index) {
    switch (Char) {
    case 'X':
        StarType = StellarClass::StarType::kBlackHole;
        return ParseState::kEnd;
    case 'Q':
        StarType = StellarClass::StarType::kNeutronStar;
        return ParseState::kEnd;
    case 'D':
        StarType = StellarClass::StarType::kWhiteDwarf;
        SpectralClass = StellarClass::SpectralClass::kSpectral_D;
        ++Index;
        return ParseState::kWhiteDwarf;
    case 's': // sd 前缀
        StarType = StellarClass::StarType::kNormalStar;
        ++Index;
        return ParseState::kSubdwarfPerfix;
    case '?':
        return ParseState::kEnd;
    default:
        return ParseState::kSpectralClass;
    }
}

static ParseState ParseSpectralClass(unsigned char Char, StellarClass::SpectralClass& SpectralClass, std::size_t& Index) {
    switch (Char) {
    case 'W':
        ++Index;
        return ParseState::kWolfRayetStar;
    case 'O':
        SpectralClass = StellarClass::SpectralClass::kSpectral_O;
        ++Index;
        return ParseState::kSubclass;
    case 'B':
        SpectralClass = StellarClass::SpectralClass::kSpectral_B;
        ++Index;
        return ParseState::kSubclass;
    case 'A':
        SpectralClass = StellarClass::SpectralClass::kSpectral_A;
        ++Index;
        return ParseState::kSubclass;
    case 'F':
        SpectralClass = StellarClass::SpectralClass::kSpectral_F;
        ++Index;
        return ParseState::kSubclass;
    case 'G':
        SpectralClass = StellarClass::SpectralClass::kSpectral_G;
        ++Index;
        return ParseState::kSubclass;
    case 'K':
        SpectralClass = StellarClass::SpectralClass::kSpectral_K;
        ++Index;
        return ParseState::kSubclass;
    case 'M':
        SpectralClass = StellarClass::SpectralClass::kSpectral_M;
        ++Index;
        return ParseState::kSubclass;
    case 'R':
        SpectralClass = StellarClass::SpectralClass::kSpectral_R;
        ++Index;
        return ParseState::kSubclass;
    case 'N':
        SpectralClass = StellarClass::SpectralClass::kSpectral_N;
        ++Index;
        return ParseState::kSubclass;
    case 'C':
        SpectralClass = StellarClass::SpectralClass::kSpectral_C;
        ++Index;
        return ParseState::kSubclass;
    case 'S':
        SpectralClass = StellarClass::SpectralClass::kSpectral_S;
        ++Index;
        return ParseState::kSubclass;
    case 'L':
        SpectralClass = StellarClass::SpectralClass::kSpectral_L;
        ++Index;
        return ParseState::kSubclass;
    case 'T':
        SpectralClass = StellarClass::SpectralClass::kSpectral_T;
        ++Index;
        return ParseState::kSubclass;
    case 'Y':
        SpectralClass = StellarClass::SpectralClass::kSpectral_Y;
        ++Index;
        return ParseState::kSubclass;
    default:
        return ParseState::kEnd;
    }
}

static ParseState ParseWolfRayetStar(unsigned char Char, StellarClass::SpectralClass& SpectralClass, std::size_t& Index) {
    switch (Char) {
    case 'C':
        SpectralClass = StellarClass::SpectralClass::kSpectral_WC;
        ++Index;
        return ParseState::kSubclass;
    case 'N':
        SpectralClass = StellarClass::SpectralClass::kSpectral_WN;
        ++Index;
        return ParseState::kSubclass;
    case 'O':
        SpectralClass = StellarClass::SpectralClass::kSpectral_WO;
        ++Index;
        return ParseState::kSubclass;
    default:
        return ParseState::kEnd;
    }
}

static ParseState ParseWhiteDwarf(unsigned char Char, StellarClass::SpectralClass& SpectralClass, std::size_t& Index) {
    ++Index;

    switch (Char) {
    case 'A':
        SpectralClass = StellarClass::SpectralClass::kSpectral_DA;
        return ParseState::kWhiteDwarfEx;
    case 'B':
        SpectralClass = StellarClass::SpectralClass::kSpectral_DB;
        return ParseState::kWhiteDwarfEx;
    case 'C':
        SpectralClass = StellarClass::SpectralClass::kSpectral_DC;
        return ParseState::kWhiteDwarfEx;
    case 'O':
        SpectralClass = StellarClass::SpectralClass::kSpectral_DO;
        return ParseState::kWhiteDwarfEx;
    case 'Q':
        SpectralClass = StellarClass::SpectralClass::kSpectral_DQ;
        return ParseState::kWhiteDwarfEx;
    case 'X':
        SpectralClass = StellarClass::SpectralClass::kSpectral_DX;
        return ParseState::kWhiteDwarfEx;
    case 'Z':
        SpectralClass = StellarClass::SpectralClass::kSpectral_DZ;
        return ParseState::kWhiteDwarfEx;
    default:
        SpectralClass = StellarClass::SpectralClass::kSpectral_D;
        return ParseState::kSubclass;
    }
}

static ParseState ParseWhiteDwarfEx(unsigned char Char, unsigned char PrevChar, StellarClass::SpectralClass& SpectralClass, std::size_t& Index) {
    if (Char == PrevChar) {
        NpgsAssert(false, "Invalid white dwarf extended type.");
    }

    switch (Char) {
    case 'A':
        ++Index;
        break;
    case 'B':
        ++Index;
        break;
    case 'C':
        ++Index;
        break;
    case 'O':
        ++Index;
        break;
    case 'Q':
        ++Index;
        break;
    case 'X':
        ++Index;
        break;
    case 'Z':
        ++Index;
        break;
    default:
        break;
    }

    return ParseState::kSubclass;
}

static ParseState ParseLuminosityClass(unsigned char Char, StellarClass::LuminosityClass& LuminosityClass, std::size_t& Index) {
    switch (Char) {
    case '0':
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_0;
        return ParseState::kEnd;
    case 'I':
        ++Index;
        return ParseState::kLuminosityClassI;
    case 'V':
        ++Index;
        return ParseState::kLuminosityClassV;
    case ' ':
        ++Index;
        return ParseState::kLuminosityClass;
    default:
        return ParseState::kEnd;
    }
}

static ParseState ParseLuminosityClassI(unsigned char Char, StellarClass::LuminosityClass& LuminosityClass, std::size_t& Index) {
    switch (Char) {
    case 'a':
        ++Index;
        return ParseState::kLuminosityClassIa;
    case 'b':
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_Ib;
        ++Index;
        return ParseState::kEnd;
    case 'I':
        ++Index;
        return ParseState::kLuminosityClassII;
    case 'V':
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_IV;
        ++Index;
        return ParseState::kEnd;
    default:
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_I;
        return ParseState::kEnd;
    }
}

static ParseState ParseLuminosityClassIa(unsigned char Char, StellarClass::LuminosityClass& LuminosityClass) {
    switch (Char) {
    case '+':
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_IaPlus;
        return ParseState::kEnd;
    case 'b':
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_Iab;
        return ParseState::kEnd;
    default:
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_Ia;
        return ParseState::kEnd;
    }
}

static ParseState ParseLuminosityClassII(unsigned char Char, StellarClass::LuminosityClass& LuminosityClass) {
    switch (Char) {
    case 'I':
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_III;
        return ParseState::kEnd;
    default:
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_II;
        return ParseState::kEnd;
    }
}

static ParseState ParseLuminosityClassV(unsigned char Char, StellarClass::LuminosityClass& LuminosityClass) {
    switch (Char) {
    case 'I':
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_VI;
        return ParseState::kEnd;
    default:
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_V;
        return ParseState::kEnd;
    }
}

StellarClass::StellarClass(StarType StarType, const SpectralType& SpectralType) : _StarType(StarType), _SpectralType(SpectralType)
{}

StellarClass StellarClass::Parse(const std::string_view StellarClassStr) {
    NpgsAssert(!StellarClassStr.empty(), "StellarClassStr is empty.");

    StarType StarType = StarType::kNormalStar;
    SpectralClass SpectralClass = SpectralClass::kSpectral_Unknown;
    double Subclass = 0.0;
    LuminosityClass LuminosityClass = LuminosityClass::kLuminosity_Unknown;

    ParseState State = ParseState::kBegin;
    std::size_t Index = 0;

    while (State != ParseState::kEnd) {
        unsigned char Char = 0;
        if (Index == StellarClassStr.size()) {
            Char = '\0';
        } else {
            Char = StellarClassStr[Index];
        }

        switch (State) {
        case ParseState::kBegin:
            State = ParseStarType(Char, StarType, SpectralClass, Index);
            break;

        case ParseState::kSpectralClass:
            State = ParseSpectralClass(Char, SpectralClass, Index);
            break;

        case ParseState::kWolfRayetStar:
            State = ParseWolfRayetStar(Char, SpectralClass, Index);
            break;

        case ParseState::kWhiteDwarf:
            State = ParseWhiteDwarf(Char, SpectralClass, Index);
            break;

        case ParseState::kWhiteDwarfEx:
            State = ParseWhiteDwarfEx(Char, StellarClassStr[Index - 1], SpectralClass, Index);
            break;

        case ParseState::kSubdwarfPerfix:
            switch (Char) {
            case 'd':
                LuminosityClass = LuminosityClass::kLuminosity_VI;
                State = ParseState::kSpectralClass;
                ++Index;
                break;
            default:
                State = ParseState::kEnd;
                break;
            }

            break;

        case ParseState::kSubclass:
            if (std::isdigit(Char)) {
                Subclass = Char - '0';
                State = ParseState::kSubclassDecimal;
                ++Index;
            } else {
                State = ParseState::kLuminosityClass;
            }

            break;

        case ParseState::kSubclassDecimal:
            if (Char == '.') {
                State = ParseState::kSubclassDecimalNext;
                ++Index;
            } else {
                State = ParseState::kLuminosityClass;
            }

            break;

        case ParseState::kSubclassDecimalNext:
            if (std::isdigit(Char)) {
                Subclass += 0.1 * (Char - '0');
            }

            State = ParseState::kLuminosityClass;
            ++Index;
            break;

        case ParseState::kLuminosityClass:
            State = ParseLuminosityClass(Char, LuminosityClass, Index);
            break;

        case ParseState::kLuminosityClassI:
            State = ParseLuminosityClassI(Char, LuminosityClass, Index);
            break;

        case ParseState::kLuminosityClassIa:
            State = ParseLuminosityClassIa(Char, LuminosityClass);
            break;

        case ParseState::kLuminosityClassII:
            State = ParseLuminosityClassII(Char, LuminosityClass);
            break;

        case ParseState::kLuminosityClassV:
            State = ParseLuminosityClassV(Char, LuminosityClass);
            break;
        }
    }

    return { StarType, { SpectralClass, Subclass, LuminosityClass } };
}

std::uint32_t StellarClass::Data() const {
    // 光谱型位结构
    // ---------------------------------------------------
    // std::uint32_t
    // |----|------|------|------|------|----------------|
    // | 00 | 0000 | 0000 | 0000 | 0000 | 00000000000000 |
    // |----|------|------|------|------|----------------|
    // 恒星类型 光谱 亚型高位 亚型低位 光度级     特殊标识

    return 0;
}

bool StellarClass::Load(std::uint32_t PackagedSpectralType) {
    // TODO
    return false;
}
