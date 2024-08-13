#include "StellarClass.h"

#include <cctype>
#include <cmath>
#include <iomanip>
#include <sstream>

#include "Engine/Core/Assert.h"

_NPGS_BEGIN
_MODULES_BEGIN

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
    kSubclassDecimalFinal,
    kLuminosityClass,
    kLuminosityClassI,
    kLuminosityClassIa,
    kLuminosityClassII,
    kLuminosityClassV,
    kSpecialMark
};

// Tool functions
// --------------
static ParseState ParseStarType(unsigned char Char, StellarClass::StarType& StarType, StellarClass::SpectralClass& HSpectralClass, std::size_t& Index);
static ParseState ParseSpectralClass(unsigned char Char, StellarClass::SpectralClass& SpectralClass, std::size_t& Index);
static ParseState ParseWolfRayetStar(unsigned char Char, StellarClass::SpectralClass& SpectralClass, std::size_t& Index);
static ParseState ParseWhiteDwarf(unsigned char Char, StellarClass::SpectralClass& SpectralClass, std::size_t& Index);
static ParseState ParseWhiteDwarfEx(unsigned char Char, unsigned char PrevChar, StellarClass::SpectralClass& SpectralClass, std::size_t& Index);
static ParseState ParseLuminosityClass(unsigned char Char, StellarClass::LuminosityClass& LuminosityClass, std::size_t& Index);
static ParseState ParseLuminosityClassI(unsigned char Char, StellarClass::LuminosityClass& LuminosityClass, std::size_t& Index);
static ParseState ParseLuminosityClassIa(unsigned char Char, StellarClass::LuminosityClass& LuminosityClass);
static ParseState ParseLuminosityClassII(unsigned char Char, StellarClass::LuminosityClass& LuminosityClass);
static ParseState ParseLuminosityClassV(unsigned char Char, StellarClass::LuminosityClass& LuminosityClass);
static ParseState ParseSpecialMark(unsigned char Char, unsigned char NextChar, StellarClass::SpecialPeculiarity& SpecialMark, std::size_t& Index);
static std::string SpectralToStr(StellarClass::SpectralClass Spectral, double Subclass);
static std::string LuminosityClassToStr(StellarClass::LuminosityClass Luminosity);
static std::string SpecialMarkToStr(StellarClass::SpecialPeculiarities SpecialMark);

// StellarClass implementations
// ----------------------------
StellarClass::StellarClass() : _StarType(StarType::kNormalStar), _SpectralType(0) {}

StellarClass::StellarClass(StarType StarType, const SpectralType& SpectralType)
    : _StarType(StarType), _SpectralType(0)
{
    Load(SpectralType);
}

StellarClass StellarClass::Parse(const std::string& StellarClassStr) {
    NpgsAssert(!StellarClassStr.empty(), "StellarClassStr is empty.");

    StarType StarType = StarType::kNormalStar;
    SpectralClass HSpectralClass = SpectralClass::kSpectral_Unknown;
    double Subclass = 0.0;
    bool bIsAmStar = false;
    SpectralClass MSpectralClass = SpectralClass::kSpectral_Unknown;
    double AmSubclass = 0.0;
    LuminosityClass LuminosityClass = LuminosityClass::kLuminosity_Unknown;
    SpecialPeculiarity SpecialMark = static_cast<std::uint32_t>(SpecialPeculiarities::kCode_Null);

    ParseState State = ParseState::kBegin;
    std::size_t Index = 0;
    bool bParsingAmStar = false;

    while (State != ParseState::kEnd) {
        unsigned char Char = 0;
        unsigned char NextChar = Index + 1 >= StellarClassStr.size() ? '\0' : StellarClassStr[Index + 1];
        if (Index == StellarClassStr.size()) {
            Char = '\0';
        } else {
            Char = StellarClassStr[Index];
        }

        switch (State) {
        case ParseState::kBegin:
            State = ParseStarType(Char, StarType, HSpectralClass, Index);
            break;

        case ParseState::kSpectralClass:
            if (!bParsingAmStar) {
                State = ParseSpectralClass(Char, HSpectralClass, Index);
            } else {
                State = ParseSpectralClass(Char, MSpectralClass, Index);
            }

            break;

        case ParseState::kWolfRayetStar:
            State = ParseWolfRayetStar(Char, HSpectralClass, Index);
            break;

        case ParseState::kWhiteDwarf:
            State = ParseWhiteDwarf(Char, HSpectralClass, Index);
            break;

        case ParseState::kWhiteDwarfEx:
            State = ParseWhiteDwarfEx(Char, StellarClassStr[Index - 1], HSpectralClass, Index);
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
                if (!bParsingAmStar) {
                    Subclass = Char - '0';
                } else {
                    AmSubclass = Char - '0';
                }

                State = ParseState::kSubclassDecimal;
                ++Index;
            } else {
                State = ParseState::kSpecialMark;
            }

            break;

        case ParseState::kSubclassDecimal:
            if (Char == '.') {
                State = ParseState::kSubclassDecimalFinal;
                ++Index;
            } else {
                State = ParseState::kSpecialMark;
            }

            break;

        case ParseState::kSubclassDecimalFinal:
            if (std::isdigit(Char)) {
                if (!bParsingAmStar) {
                    Subclass += 0.1 * (Char - '0');
                } else {
                    AmSubclass += 0.1 * (Char - '0');
                }
            }

            State = ParseState::kSpecialMark;
            ++Index;
            break;

        case ParseState::kSpecialMark:
            if ((State = ParseSpecialMark(Char, NextChar, SpecialMark, Index)) == ParseState::kSpectralClass) {
                bParsingAmStar = true;
                bIsAmStar = true;
            }

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

    return { StarType, { HSpectralClass, Subclass, bIsAmStar, MSpectralClass, AmSubclass, LuminosityClass, SpecialMark } };
}

StellarClass::SpectralType StellarClass::Data() const {
    StarType     Type{};
    SpectralType StructSpectralType{};

    Type                               = static_cast<StarType>(_SpectralType >> 62 & 0x3);
    StructSpectralType.HSpectralClass  = static_cast<SpectralClass>(_SpectralType >> 58 & 0xF);
    StructSpectralType.Subclass        = (_SpectralType >> 54 & 0xF) + (_SpectralType >> 50 & 0xF) / 10.0;
    StructSpectralType.bIsAmStar       = _SpectralType >> 49 & 0x1;
    StructSpectralType.MSpectralClass  = static_cast<SpectralClass>(_SpectralType >> 45 & 0xF);
    StructSpectralType.AmSubclass      = (_SpectralType >> 41 & 0xF) + (_SpectralType >> 37 & 0xF) / 10.0;
    StructSpectralType.LuminosityClass = static_cast<LuminosityClass>(_SpectralType >> 33 & 0xF);
    StructSpectralType.SpecialMark     = static_cast<SpecialPeculiarity>(_SpectralType & 0x1FFFFFFFF);

    if (StructSpectralType.HSpectralClass == SpectralClass::kSpectral_Unknown) {
        StructSpectralType = { SpectralClass::kSpectral_Unknown, 0.0, false, SpectralClass::kSpectral_Unknown, 0.0, LuminosityClass::kLuminosity_Unknown, 0 };
    }

    return StructSpectralType;
}

bool StellarClass::Load(const SpectralType& SpectralType) {
    // 光谱型位结构
    // ---------------------------------------------------------------------------------------------------
    // std::uint64_t
    // |----|------|------|------|---|------|------|------|------|---------------------------------------|
    // | 00 | 0000 | 0000 | 0000 | 0 | 0000 | 0000 | 0000 | 0000 | 0 00000000 00000000 00000000 00000000 |
    // |----|------|------|------|---|------|------|------|------|---------------------------------------|
    // 恒星类型 光谱 亚型高位 亚型低位 Am m 光谱 m 亚型高位 m亚型低位 光度级 特殊标识

    std::uint64_t Data         = 0;
    std::uint32_t SubclassHigh = static_cast<std::uint32_t>(SpectralType.Subclass);
    double        Intermediate = std::round((SpectralType.Subclass - SubclassHigh) * 1000.0) / 1000.0;
    std::uint32_t SubclassLow  = static_cast<std::uint32_t>(Intermediate * 10.0);

    Data |= static_cast<std::uint64_t>(_StarType)                    << 62;
    Data |= static_cast<std::uint64_t>(SpectralType.HSpectralClass)  << 58;
    Data |= static_cast<std::uint64_t>(SubclassHigh)                 << 54;
    Data |= static_cast<std::uint64_t>(SubclassLow)                  << 50;
    Data |= static_cast<std::uint64_t>(SpectralType.bIsAmStar)       << 49;
    Data |= static_cast<std::uint64_t>(SpectralType.MSpectralClass)  << 45;

    SubclassHigh = static_cast<std::uint32_t>(SpectralType.AmSubclass);
    Intermediate = std::round((SpectralType.AmSubclass - SubclassHigh) * 1000.0) / 1000.0;
    SubclassLow  = static_cast<std::uint32_t>(Intermediate * 10.0);

    Data |= static_cast<std::uint64_t>(SubclassHigh)                 << 41;
    Data |= static_cast<std::uint64_t>(SubclassLow)                  << 37;
    Data |= static_cast<std::uint64_t>(SpectralType.LuminosityClass) << 33;
    Data |= static_cast<std::uint64_t>(SpectralType.SpecialMark)     << 0;

    _SpectralType = Data;
    return true;
}

std::string StellarClass::ToString() const {
    SpectralType StructSpectralType = Data();

    if (StructSpectralType.HSpectralClass == SpectralClass::kSpectral_Unknown) {
        return "Unknown";
    }

    std::string SpectralTypeStr = SpectralToStr(StructSpectralType.HSpectralClass, StructSpectralType.Subclass);

    if (StructSpectralType.bIsAmStar) {
        SpectralTypeStr += "m" + SpectralToStr(StructSpectralType.MSpectralClass, StructSpectralType.AmSubclass);
    }

    SpectralTypeStr += LuminosityClassToStr(StructSpectralType.LuminosityClass);
    SpectralTypeStr += SpecialMarkToStr(static_cast<SpecialPeculiarities>(StructSpectralType.SpecialMark));

    return SpectralTypeStr;
}

// Tool functions implementations
// ------------------------------
static ParseState ParseStarType(unsigned char Char, StellarClass::StarType& StarType, StellarClass::SpectralClass& HSpectralClass, std::size_t& Index) {
    switch (Char) {
    case 'X':
        StarType = StellarClass::StarType::kBlackHole;
        return ParseState::kEnd;
    case 'Q':
        StarType = StellarClass::StarType::kNeutronStar;
        return ParseState::kEnd;
    case 'D':
        StarType = StellarClass::StarType::kWhiteDwarf;
        HSpectralClass = StellarClass::SpectralClass::kSpectral_D;
        ++Index;
        return ParseState::kWhiteDwarf;
    case 's': // sd 前缀
        StarType = StellarClass::StarType::kNormalStar;
        ++Index;
        return ParseState::kSubdwarfPerfix;
    case '?':
        return ParseState::kEnd;
    default:
        StarType = StellarClass::StarType::kNormalStar;
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
        if (LuminosityClass == StellarClass::LuminosityClass::kLuminosity_Unknown) {
            LuminosityClass =  StellarClass::LuminosityClass::kLuminosity_0;
            return ParseState::kSpecialMark;
        } else {
            return ParseState::kEnd;
        }
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
        return ParseState::kSpecialMark;
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
        return ParseState::kSpecialMark;
    case 'I':
        ++Index;
        return ParseState::kLuminosityClassII;
    case 'V':
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_IV;
        ++Index;
        return ParseState::kSpecialMark;
    default:
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_I;
        return ParseState::kSpecialMark;
    }
}

static ParseState ParseLuminosityClassIa(unsigned char Char, StellarClass::LuminosityClass& LuminosityClass) {
    switch (Char) {
    case '+':
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_IaPlus;
        return ParseState::kSpecialMark;
    case 'b':
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_Iab;
        return ParseState::kSpecialMark;
    default:
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_Ia;
        return ParseState::kSpecialMark;
    }
}

static ParseState ParseLuminosityClassII(unsigned char Char, StellarClass::LuminosityClass& LuminosityClass) {
    switch (Char) {
    case 'I':
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_III;
        return ParseState::kSpecialMark;
    default:
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_II;
        return ParseState::kSpecialMark;
    }
}

static ParseState ParseLuminosityClassV(unsigned char Char, StellarClass::LuminosityClass& LuminosityClass) {
    switch (Char) {
    case 'I':
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_VI;
        return ParseState::kSpecialMark;
    default:
        LuminosityClass = StellarClass::LuminosityClass::kLuminosity_V;
        return ParseState::kSpecialMark;
    }
}

static ParseState ParseSpecialMark(unsigned char Char, unsigned char NextChar, StellarClass::SpecialPeculiarity& SpecialMark, std::size_t& Index) {
    switch (Char) {
    case 'm':
        SpecialMark |= static_cast<std::uint32_t>(StellarClass::SpecialPeculiarities::kCode_m);
        ++Index;
        return ParseState::kSpectralClass;
    case 'f':
        SpecialMark |= static_cast<std::uint32_t>(StellarClass::SpecialPeculiarities::kCode_f);
        ++Index;
        return (std::isalpha(NextChar) && std::islower(NextChar)) ? ParseState::kSpecialMark : ParseState::kEnd;
    case 'h':
        SpecialMark |= static_cast<std::uint32_t>(StellarClass::SpecialPeculiarities::kCode_h);
        ++Index;
        return (std::isalpha(NextChar) && std::islower(NextChar)) ? ParseState::kSpecialMark : ParseState::kEnd;
    case 'p':
        SpecialMark |= static_cast<std::uint32_t>(StellarClass::SpecialPeculiarities::kCode_h);
        ++Index;
        return (std::isalpha(NextChar) && std::islower(NextChar)) ? ParseState::kSpecialMark : ParseState::kEnd;
    case '+':
        ++Index;
        return ParseState::kSpecialMark;
    case ' ':
        ++Index;
        return ParseState::kSpecialMark;
    case '\0':
        return ParseState::kEnd;
    default:
        return ParseState::kLuminosityClass;
    }
}

static std::string SpectralToStr(StellarClass::SpectralClass Spectral, double Subclass) {
    std::ostringstream Stream;

    if (Subclass == std::floor(Subclass)) {
        Stream << std::fixed << std::setprecision(0) << Subclass;
    } else {
        Subclass = std::round(Subclass * 10.0) / 10.0;
        Stream << std::fixed << std::setprecision(1) << Subclass;
    }

    switch (Spectral) {
    case StellarClass::SpectralClass::kSpectral_O:
        return std::string("O") + Stream.str();
    case StellarClass::SpectralClass::kSpectral_B:
        return std::string("B") + Stream.str();
    case StellarClass::SpectralClass::kSpectral_A:
        return std::string("A") + Stream.str();
    case StellarClass::SpectralClass::kSpectral_F:
        return std::string("F") + Stream.str();
    case StellarClass::SpectralClass::kSpectral_G:
        return std::string("G") + Stream.str();
    case StellarClass::SpectralClass::kSpectral_K:
        return std::string("K") + Stream.str();
    case StellarClass::SpectralClass::kSpectral_M:
        return std::string("M") + Stream.str();
    case StellarClass::SpectralClass::kSpectral_R:
        return std::string("R") + Stream.str();
    case StellarClass::SpectralClass::kSpectral_N:
        return std::string("N") + Stream.str();
    case StellarClass::SpectralClass::kSpectral_C:
        return std::string("C") + Stream.str();
    case StellarClass::SpectralClass::kSpectral_S:
        return std::string("S") + Stream.str();
    case StellarClass::SpectralClass::kSpectral_WO:
        return std::string("WO") + Stream.str();
    case StellarClass::SpectralClass::kSpectral_WN:
        return std::string("WN") + Stream.str();
    case StellarClass::SpectralClass::kSpectral_WC:
        return std::string("WC") + Stream.str();
    case StellarClass::SpectralClass::kSpectral_L:
        return std::string("L") + Stream.str();
    case StellarClass::SpectralClass::kSpectral_T:
        return std::string("T") + Stream.str();
    case StellarClass::SpectralClass::kSpectral_Y:
        return std::string("Y") + Stream.str();
    default:
        return std::string("Unknown");
    }
}

static std::string LuminosityClassToStr(StellarClass::LuminosityClass Luminosity) {
    switch (Luminosity) {
    case StellarClass::LuminosityClass::kLuminosity_0:
        return std::string("0");
    case StellarClass::LuminosityClass::kLuminosity_IaPlus:
        return std::string("Ia+");
    case StellarClass::LuminosityClass::kLuminosity_Ia:
        return std::string("Ia");
    case StellarClass::LuminosityClass::kLuminosity_Ib:
        return std::string("Ib");
    case StellarClass::LuminosityClass::kLuminosity_Iab:
        return std::string("Iab");
    case StellarClass::LuminosityClass::kLuminosity_I:
        return std::string("I");
    case StellarClass::LuminosityClass::kLuminosity_II:
        return std::string("II");
    case StellarClass::LuminosityClass::kLuminosity_III:
        return std::string("III");
    case StellarClass::LuminosityClass::kLuminosity_IV:
        return std::string("IV");
    case StellarClass::LuminosityClass::kLuminosity_V:
        return std::string("V");
    case StellarClass::LuminosityClass::kLuminosity_VI:
        return std::string("VI");
    default:
        return std::string("");
    }
}

static std::string SpecialMarkToStr(StellarClass::SpecialPeculiarities SpecialMark) {
    switch (SpecialMark) {
    case StellarClass::SpecialPeculiarities::kCode_Null:
        return std::string("");
    case StellarClass::SpecialPeculiarities::kCode_f:
        return std::string("f");
    case StellarClass::SpecialPeculiarities::kCode_h:
        return std::string("h");
    case StellarClass::SpecialPeculiarities::kCode_p:
        return std::string("p");
    default:
        return std::string("");
    }
}

_MODULES_END
_NPGS_END
