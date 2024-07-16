#include "StellarClass.h"

#include <cctype>
#include "Engine/Core/Assert.h"

enum class ParseState {
    kBegin,
    kEnd,
    // Parsing star type
    // -----------------
    kWolfRayetStar,
    kWhiteDwarf,
    kWhiteDwarfExA,
    kWhiteDwarfExB,
    kSubdwarfPerfix,

    kSpectralClass,
    kSubclass,
};

StellarClass::StellarClass(StarType StarType, SpectralClass SpectralClass, std::int32_t Subclass, LuminosityClass LuminosityClass)
    : _StarType(StarType), _SpectralClass(SpectralClass), _Subclass(Subclass), _LuminosityClass(LuminosityClass) {}

StellarClass StellarClass::Parse(const std::string_view StellarClassStr) {
    NpgsAssert(!StellarClassStr.empty(), "StellarClassStr is empty.");

    StarType StarType = StarType::kNormalStar;
    SpectralClass SpectralClass = SpectralClass::kSpectral_Unknown;
    std::int32_t Subclass = 0;
    LuminosityClass LuminosityClass = LuminosityClass::kLuminosity_Unknown;

    ParseState State = ParseState::kBegin;
    std::int32_t Index = 0;

    while (State != ParseState::kEnd) {
        char Char = StellarClassStr[Index];

        switch (State) {
        case ParseState::kBegin:
            switch (Char) {
            case 'Q':
                StarType = StarType::kNeutronStar;
                State = ParseState::kEnd;
                break;
            case 'X':
                StarType = StarType::kBlackHole;
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
                break;
            case 'O':
                SpectralClass = SpectralClass::kSpectral_O;
                State = ParseState::kSpectralClass;
                break;
            case 'B':
                SpectralClass = SpectralClass::kSpectral_B;
                State = ParseState::kSpectralClass;
                break;
            case 'A':
                SpectralClass = SpectralClass::kSpectral_A;
                State = ParseState::kSpectralClass;
                break;
            case 'F':
                SpectralClass = SpectralClass::kSpectral_F;
                State = ParseState::kSpectralClass;
                break;
            case 'G':
                SpectralClass = SpectralClass::kSpectral_G;
                State = ParseState::kSpectralClass;
                break;
            case 'K':
                SpectralClass = SpectralClass::kSpectral_K;
                State = ParseState::kSpectralClass;
                break;
            case 'M':
                SpectralClass = SpectralClass::kSpectral_M;
                State = ParseState::kSpectralClass;
                break;
            case 'R':
                SpectralClass = SpectralClass::kSpectral_R;
                State = ParseState::kSpectralClass;
                break;
            case 'N':
                SpectralClass = SpectralClass::kSpectral_N;
                State = ParseState::kSpectralClass;
                break;
            case 'C':
                SpectralClass = SpectralClass::kSpectral_C;
                State = ParseState::kSpectralClass;
                break;
            case 'S':
                SpectralClass = SpectralClass::kSpectral_S;
                State = ParseState::kSpectralClass;
                break;
            case 'L':
                SpectralClass = SpectralClass::kSpectral_L;
                State = ParseState::kSpectralClass;
                break;
            case 'T':
                SpectralClass = SpectralClass::kSpectral_T;
                State = ParseState::kSpectralClass;
                break;
            case 'Y':
                SpectralClass = SpectralClass::kSpectral_Y;
                State = ParseState::kSpectralClass;
                break;
            default:
                State = ParseState::kEnd;
                break;
            }

            ++Index;
            break;

        case ParseState::kWolfRayetStar:
            switch (Char) {
            case 'C':
                SpectralClass = SpectralClass::kSpectral_WC;
                State = ParseState::kSubclass;
                ++Index;
                break;
            case 'N':
                if (StellarClassStr.back() == 'h') {
                    SpectralClass = SpectralClass::kSpectral_WNxh;
                } else {
                    SpectralClass = SpectralClass::kSpectral_WN;
                }

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
                State = ParseState::kWhiteDwarfExA;
                ++Index;
                break;
            case 'B':
                SpectralClass = SpectralClass::kSpectral_DB;
                State = ParseState::kWhiteDwarfExB;
                ++Index;
                break;
            case 'C':
                SpectralClass = SpectralClass::kSpectral_DC;
                State = ParseState::kSubclass;
                ++Index;
                break;
            case 'O':
                SpectralClass = SpectralClass::kSpectral_DO;
                State = ParseState::kSubclass;
                ++Index;
                break;
            case 'Q':
                SpectralClass = SpectralClass::kSpectral_DQ;
                State = ParseState::kSubclass;
                ++Index;
                break;
            case 'X':
                SpectralClass = SpectralClass::kSpectral_DX;
                State = ParseState::kSubclass;
                ++Index;
                break;
            case 'Z':
                SpectralClass = SpectralClass::kSpectral_DZ;
                State = ParseState::kSubclass;
                ++Index;
                break;
            default:
                SpectralClass = SpectralClass::kSpectral_D;
                break;
            }

            break;

        case ParseState::kWhiteDwarfExA:
            switch (Char) {
            case 'B':
                SpectralClass = SpectralClass::kSpectral_DAB;
                State = ParseState::kSubclass;
                ++Index;
                break;
            case 'O':
                SpectralClass = SpectralClass::kSpectral_DAO;
                State = ParseState::kSubclass;
                ++Index;
                break;
            case 'Z':
                SpectralClass = SpectralClass::kSpectral_DAZ;
                State = ParseState::kSubclass;
                ++Index;
                break;
            default:
                State = ParseState::kSubclass;
                break;
            }

            break;

        case ParseState::kWhiteDwarfExB:
            switch (Char) {
            case 'Z':
                SpectralClass = SpectralClass::kSpectral_DBZ;
                State = ParseState::kSubclass;
                ++Index;
                break;
            default:
                State = ParseState::kSubclass;
                break;
            }

            break;

        case ParseState::kSubdwarfPerfix:
            switch (Char) {
            case 'd':
                LuminosityClass = LuminosityClass::kLuminosity_VI;
                State = ParseState::kSubclass;
                ++Index;
                break;
            default:
                State = ParseState::kEnd;
                break;
            }

            break;
        }
    }
}

std::uint16_t StellarClass::Data() const {
    return static_cast<std::uint16_t>(_StarType)      << 12 |
           static_cast<std::uint16_t>(_SpectralClass) << 8  |
           static_cast<std::uint16_t>(_Subclass)      << 4  |
           static_cast<std::uint16_t>(_LuminosityClass);
}

bool StellarClass::Load(std::uint16_t PackagedSpectralType) {
    return false;
}
