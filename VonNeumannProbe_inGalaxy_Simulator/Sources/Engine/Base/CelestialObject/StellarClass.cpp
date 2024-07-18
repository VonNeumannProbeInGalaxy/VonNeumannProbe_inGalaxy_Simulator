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

StellarClass::StellarClass(StarType StarType, SpectralClass SpectralClass, double Subclass, LuminosityClass LuminosityClass)
    : _StarType(StarType), _SpectralClass(SpectralClass), _Subclass(Subclass), _LuminosityClass(LuminosityClass)
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
            switch (Char) {
            case 'X':
                StarType = StarType::kBlackHole;
                State = ParseState::kEnd;
                break;
            case 'Q':
                StarType = StarType::kNeutronStar;
                State = ParseState::kEnd;
                break;
            case 'D':
                StarType = StarType::kWhiteDwarf;
                SpectralClass = SpectralClass::kSpectral_D;
                State = ParseState::kWhiteDwarf;
                ++Index;
                break;
            case 's': // sd 前缀
                StarType = StarType::kNormalStar;
                State = ParseState::kSubdwarfPerfix;
                ++Index;
                break;
            case '?':
                State = ParseState::kEnd;
                break;
            default:
                State = ParseState::kSpectralClass;
                break;
            }

            break;

        case ParseState::kSpectralClass:
            switch (Char) {
            case 'W':
                State = ParseState::kWolfRayetStar;
                ++Index;
                break;
            case 'O':
                SpectralClass = SpectralClass::kSpectral_O;
                State = ParseState::kSubclass;
                ++Index;
                break;
            case 'B':
                SpectralClass = SpectralClass::kSpectral_B;
                State = ParseState::kSubclass;
                ++Index;
                break;
            case 'A':
                SpectralClass = SpectralClass::kSpectral_A;
                State = ParseState::kSubclass;
                ++Index;
                break;
            case 'F':
                SpectralClass = SpectralClass::kSpectral_F;
                State = ParseState::kSubclass;
                ++Index;
                break;
            case 'G':
                SpectralClass = SpectralClass::kSpectral_G;
                State = ParseState::kSubclass;
                ++Index;
                break;
            case 'K':
                SpectralClass = SpectralClass::kSpectral_K;
                State = ParseState::kSubclass;
                ++Index;
                break;
            case 'M':
                SpectralClass = SpectralClass::kSpectral_M;
                State = ParseState::kSubclass;
                ++Index;
                break;
            case 'R':
                SpectralClass = SpectralClass::kSpectral_R;
                State = ParseState::kSubclass;
                ++Index;
                break;
            case 'N':
                SpectralClass = SpectralClass::kSpectral_N;
                State = ParseState::kSubclass;
                ++Index;
                break;
            case 'C':
                SpectralClass = SpectralClass::kSpectral_C;
                State = ParseState::kSubclass;
                ++Index;
                break;
            case 'S':
                SpectralClass = SpectralClass::kSpectral_S;
                State = ParseState::kSubclass;
                ++Index;
                break;
            case 'L':
                SpectralClass = SpectralClass::kSpectral_L;
                State = ParseState::kSubclass;
                ++Index;
                break;
            case 'T':
                SpectralClass = SpectralClass::kSpectral_T;
                State = ParseState::kSubclass;
                ++Index;
                break;
            case 'Y':
                SpectralClass = SpectralClass::kSpectral_Y;
                State = ParseState::kSubclass;
                ++Index;
                break;
            default:
                State = ParseState::kEnd;
                break;
            }

            break;

        case ParseState::kWolfRayetStar:
            switch (Char) {
            case 'C':
                SpectralClass = SpectralClass::kSpectral_WC;
                State = ParseState::kSubclass;
                ++Index;
                break;
            case 'N':
                SpectralClass = SpectralClass::kSpectral_WN;
                State = ParseState::kSubclass;
                ++Index;
                break;
            case 'O':
                SpectralClass = SpectralClass::kSpectral_WO;
                State = ParseState::kSubclass;
                ++Index;
                break;
            default:
                State = ParseState::kEnd;
                break;
            }

            break;

        case ParseState::kWhiteDwarf:
            switch (Char) {
            case 'A':
                SpectralClass = SpectralClass::kSpectral_DA;
                State = ParseState::kWhiteDwarfEx;
                break;
            case 'B':
                SpectralClass = SpectralClass::kSpectral_DB;
                State = ParseState::kWhiteDwarfEx;
                break;
            case 'C':
                SpectralClass = SpectralClass::kSpectral_DC;
                State = ParseState::kWhiteDwarfEx;
                break;
            case 'O':
                SpectralClass = SpectralClass::kSpectral_DO;
                State = ParseState::kWhiteDwarfEx;
                break;
            case 'Q':
                SpectralClass = SpectralClass::kSpectral_DQ;
                State = ParseState::kWhiteDwarfEx;
                break;
            case 'X':
                SpectralClass = SpectralClass::kSpectral_DX;
                State = ParseState::kWhiteDwarfEx;
                break;
            case 'Z':
                SpectralClass = SpectralClass::kSpectral_DZ;
                State = ParseState::kWhiteDwarfEx;
                break;
            default:
                SpectralClass = SpectralClass::kSpectral_D;
                State = ParseState::kSubclass;
                break;
            }

            ++Index;
            break;

        case ParseState::kWhiteDwarfEx:
            if (Char == StellarClassStr[Index - 1]) {
                NpgsAssert(false, "Invalid white dwarf extended type.");
            }

            switch (Char) {
            case 'A':
                break;
            case 'B':
                break;
            case 'C':
                break;
            case 'O':
                break;
            case 'Q':
                break;
            case 'X':
                break;
            case 'Z':
                break;
            default:
                break;
            }

            State = ParseState::kSubclass;
            ++Index;
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
            switch (Char) {
            case '0':
                LuminosityClass = LuminosityClass::kLuminosity_0;
                State = ParseState::kEnd;
                break;
            case 'I':
                State = ParseState::kLuminosityClassI;
                ++Index;
                break;
            case 'V':
                State = ParseState::kLuminosityClassV;
                ++Index;
                break;
            case ' ':
                ++Index;
                break;
            default:
                State = ParseState::kEnd;
                break;
            }

            break;

        case ParseState::kLuminosityClassI:
            switch (Char) {
            case 'a':
                State = ParseState::kLuminosityClassIa;
                ++Index;
                break;
            case 'b':
                LuminosityClass = LuminosityClass::kLuminosity_Ib;
                State = ParseState::kEnd;
                ++Index;
                break;
            case 'I':
                State = ParseState::kLuminosityClassII;
                ++Index;
                break;
            case 'V':
                LuminosityClass = LuminosityClass::kLuminosity_IV;
                State = ParseState::kEnd;
                ++Index;
                break;
            default:
                LuminosityClass = LuminosityClass::kLuminosity_I;
                State = ParseState::kEnd;
                break;
            }

            break;

        case ParseState::kLuminosityClassIa:
            switch (Char) {
            case '+':
                LuminosityClass = LuminosityClass::kLuminosity_IaPlus;
                State = ParseState::kEnd;
                break;
            case 'b':
                LuminosityClass = LuminosityClass::kLuminosity_Iab;
                State = ParseState::kEnd;
                break;
            default:
                LuminosityClass = LuminosityClass::kLuminosity_Ia;
                State = ParseState::kEnd;
                break;
            }

            break;

        case ParseState::kLuminosityClassII:
            switch (Char) {
            case 'I':
                LuminosityClass = LuminosityClass::kLuminosity_III;
                State = ParseState::kEnd;
                break;
            default:
                LuminosityClass = LuminosityClass::kLuminosity_II;
                State = ParseState::kEnd;
                break;
            }

            break;

        case ParseState::kLuminosityClassV:
            switch (Char) {
            case 'I':
                LuminosityClass = LuminosityClass::kLuminosity_VI;
                State = ParseState::kEnd;
                break;
            default:
                LuminosityClass = LuminosityClass::kLuminosity_V;
                State = ParseState::kEnd;
                break;
            }

            break;
        }
    }

    return { StarType, SpectralClass, Subclass, LuminosityClass };
}

std::uint32_t StellarClass::Data() const {
    // TODO
    return 0;
}

bool StellarClass::Load(std::uint32_t PackagedSpectralType) {
    // TODO
    return false;
}
