#include "StellarClass.h"

#include <cctype>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <utility>

#include "Engine/Core/Assert.h"

_NPGS_BEGIN
_UTIL_BEGIN

// StellarClass implementations
// ----------------------------
StellarClass::StellarClass() : _StarType(StarType::kNormalStar), _SpectralType(0)
{
}

StellarClass::StellarClass(StarType StarType, const SpectralType& SpectralType)
	: _StarType(StarType), _SpectralType(0)
{
	Load(SpectralType);
}

StellarClass::SpectralType StellarClass::Data() const
{
	SpectralType StructSpectralType{};

	StructSpectralType.HSpectralClass  = static_cast<SpectralClass>(_SpectralType >> 57 & 0x1F);
	StructSpectralType.Subclass        = (_SpectralType >> 53 & 0xF) + (_SpectralType >> 49 & 0xF) / 10.0f;
	StructSpectralType.bIsAmStar       = _SpectralType >> 48 & 0x1;
	StructSpectralType.MSpectralClass  = static_cast<SpectralClass>(_SpectralType >> 44 & 0xF);
	StructSpectralType.AmSubclass      = (_SpectralType >> 40 & 0xF) + (_SpectralType >> 36 & 0xF) / 10.0f;
	StructSpectralType.LuminosityClass = static_cast<LuminosityClass>(_SpectralType >> 32 & 0xF);
	StructSpectralType.SpecialMark     = static_cast<SpecialMarkDigital>(_SpectralType & 0x1FFFFFFFF);

	if (StructSpectralType.HSpectralClass == SpectralClass::kSpectral_Unknown)
	{
		StructSpectralType =
		{
			SpectralClass::kSpectral_Unknown,
			SpectralClass::kSpectral_Unknown,
			LuminosityClass::kLuminosity_Unknown,
			0, 0.0f, 0.0f, false
		};
	}

	return StructSpectralType;
}

bool StellarClass::Load(const SpectralType& SpectralType)
{
	// 结构
	// --------------------------------------------------------------------------------------------------
	// std::uint64_t
	// |----|-------|------|------|---|------|------|------|------|-------------------------------------|
	// | 00 | 00000 | 0000 | 0000 | 0 | 0000 | 0000 | 0000 | 0000 | 00000000 00000000 00000000 00000000 |
	// |----|-------|------|------|---|------|------|------|------|-------------------------------------|
	// 恒星类型 光谱  亚型高位 亚型低位 Am m 光谱 m 亚型高位 m 亚型低位 光度级 特殊标识

	std::uint64_t Data         = 0;
	std::uint64_t SubclassHigh = static_cast<std::uint64_t>(SpectralType.Subclass);
	float         Intermediate = std::round((SpectralType.Subclass - SubclassHigh) * 1000.0f) / 1000.0f;
	std::uint64_t SubclassLow  = static_cast<std::uint64_t>(Intermediate * 10.0f);

	Data |= static_cast<std::uint64_t>(_StarType)                    << 62;
	Data |= static_cast<std::uint64_t>(SpectralType.HSpectralClass)  << 57;
	Data |= SubclassHigh                                             << 53;
	Data |= SubclassLow                                              << 49;
	Data |= static_cast<std::uint64_t>(SpectralType.bIsAmStar)       << 48;
	Data |= static_cast<std::uint64_t>(SpectralType.MSpectralClass)  << 44;

	SubclassHigh = static_cast<std::uint64_t>(SpectralType.AmSubclass);
	Intermediate = std::round((SpectralType.AmSubclass - SubclassHigh) * 1000.0f) / 1000.0f;
	SubclassLow  = static_cast<std::uint64_t>(Intermediate * 10.0f);

	Data |= SubclassHigh                                             << 40;
	Data |= SubclassLow                                              << 36;
	Data |= static_cast<std::uint64_t>(SpectralType.LuminosityClass) << 32;
	Data |= static_cast<std::uint64_t>(SpectralType.SpecialMark)     << 0;

	_SpectralType = Data;
	return true;
}

std::string StellarClass::ToString() const
{
	SpectralType StructSpectralType = Data();

	if (StructSpectralType.HSpectralClass == SpectralClass::kSpectral_Unknown)
	{
		return "Unknown";
	}

	std::string SpectralTypeStr = SpectralToString(StructSpectralType.HSpectralClass, StructSpectralType.Subclass);

	if (StructSpectralType.bIsAmStar)
	{
		SpectralTypeStr += "m" + SpectralToString(StructSpectralType.MSpectralClass, StructSpectralType.AmSubclass);
	}

	SpectralTypeStr += LuminosityClassToString(StructSpectralType.LuminosityClass);
	SpectralTypeStr += SpecialMarkToString(static_cast<SpecialMark>(StructSpectralType.SpecialMark));

	return SpectralTypeStr;
}

StellarClass StellarClass::Parse(const std::string& StellarClassStr)
{
	NpgsAssert(!StellarClassStr.empty(), "StellarClassStr is empty.");

	StarType StarType               = StarType::kNormalStar;
	SpectralClass HSpectralClass    = SpectralClass::kSpectral_Unknown;
	SpectralClass MSpectralClass    = SpectralClass::kSpectral_Unknown;
	LuminosityClass LuminosityClass = LuminosityClass::kLuminosity_Unknown;
	SpecialMarkDigital SpecialMark  = std::to_underlying(SpecialMark::kCode_Null);
	float Subclass                  = 0.0f;
	float AmSubclass                = 0.0f;
	bool bIsAmStar                  = false;

	ParseState  State   = ParseState::kBegin;
	std::size_t Index   = 0;
	bool bParsingAmStar = false;

	while (State != ParseState::kEnd)
	{
		unsigned char Char = 0;
		unsigned char NextChar = Index + 1 >= StellarClassStr.size() ? '\0' : StellarClassStr[Index + 1];
		if (Index == StellarClassStr.size())
		{
			Char = '\0';
		}
		else
		{
			Char = StellarClassStr[Index];
		}

		switch (State)
		{
		case ParseState::kBegin:
			State = ParseStarType(Char, StarType, HSpectralClass, Index);
			break;

		case ParseState::kSpectralClass:
			if (!bParsingAmStar)
			{
				State = ParseSpectralClass(Char, HSpectralClass, Index);
			}
			else
			{
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
			switch (Char)
			{
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
			if (std::isdigit(Char))
			{
				if (!bParsingAmStar)
				{
					Subclass = static_cast<float>(Char - '0');
				}
				else
				{
					AmSubclass = static_cast<float>(Char - '0');
				}

				State = ParseState::kSubclassDecimal;
				++Index;
			}
			else
			{
				State = ParseState::kSpecialMark;
			}

			break;

		case ParseState::kSubclassDecimal:
			if (Char == '.')
			{
				State = ParseState::kSubclassDecimalFinal;
				++Index;
			}
			else
			{
				State = ParseState::kSpecialMark;
			}

			break;

		case ParseState::kSubclassDecimalFinal:
			if (std::isdigit(Char))
			{
				if (!bParsingAmStar)
				{
					Subclass += 0.1f * (Char - '0');
				}
				else
				{
					AmSubclass += 0.1f * (Char - '0');
				}
			}

			State = ParseState::kSpecialMark;
			++Index;
			break;

		case ParseState::kSpecialMark:
			if ((State = ParseSpecialMark(Char, NextChar, SpecialMark, Index)) == ParseState::kSpectralClass)
			{
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

	return { StarType, { HSpectralClass, MSpectralClass, LuminosityClass, SpecialMark, Subclass, AmSubclass, bIsAmStar } };
}

// Processor functions implementations
// -----------------------------------
StellarClass::ParseState StellarClass::ParseStarType(unsigned char Char, StellarClass::StarType& StarType, StellarClass::SpectralClass& HSpectralClass, std::size_t& Index)
{
	switch (Char)
	{
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

StellarClass::ParseState StellarClass::ParseSpectralClass(unsigned char Char, StellarClass::SpectralClass& SpectralClass, std::size_t& Index)
{
	switch (Char)
	{
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

StellarClass::ParseState StellarClass::ParseWolfRayetStar(unsigned char Char, StellarClass::SpectralClass& SpectralClass, std::size_t& Index)
{
	switch (Char)
	{
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

StellarClass::ParseState StellarClass::ParseWhiteDwarf(unsigned char Char, StellarClass::SpectralClass& SpectralClass, std::size_t& Index)
{
	++Index;

	switch (Char)
	{
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

StellarClass::ParseState StellarClass::ParseWhiteDwarfEx(unsigned char Char, unsigned char PrevChar, StellarClass::SpectralClass& SpectralClass, std::size_t& Index)
{
	if (Char == PrevChar)
	{
		NpgsAssert(false, "Invalid white dwarf extended type.");
	}

	switch (Char)
	{
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

StellarClass::ParseState StellarClass::ParseLuminosityClass(unsigned char Char, StellarClass::LuminosityClass& LuminosityClass, std::size_t& Index)
{
	switch (Char)
	{
	case '0':
		if (LuminosityClass == StellarClass::LuminosityClass::kLuminosity_Unknown)
		{
			LuminosityClass = StellarClass::LuminosityClass::kLuminosity_0;
			return ParseState::kSpecialMark;
		}
		else
		{
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

StellarClass::ParseState StellarClass::ParseLuminosityClassI(unsigned char Char, StellarClass::LuminosityClass& LuminosityClass, std::size_t& Index)
{
	switch (Char)
	{
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

StellarClass::ParseState StellarClass::ParseLuminosityClassIa(unsigned char Char, StellarClass::LuminosityClass& LuminosityClass)
{
	switch (Char)
	{
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

StellarClass::ParseState StellarClass::ParseLuminosityClassII(unsigned char Char, StellarClass::LuminosityClass& LuminosityClass)
{
	switch (Char)
	{
	case 'I':
		LuminosityClass = StellarClass::LuminosityClass::kLuminosity_III;
		return ParseState::kSpecialMark;
	default:
		LuminosityClass = StellarClass::LuminosityClass::kLuminosity_II;
		return ParseState::kSpecialMark;
	}
}

StellarClass::ParseState StellarClass::ParseLuminosityClassV(unsigned char Char, StellarClass::LuminosityClass& LuminosityClass)
{
	switch (Char)
	{
	case 'I':
		LuminosityClass = StellarClass::LuminosityClass::kLuminosity_VI;
		return ParseState::kSpecialMark;
	default:
		LuminosityClass = StellarClass::LuminosityClass::kLuminosity_V;
		return ParseState::kSpecialMark;
	}
}

StellarClass::ParseState StellarClass::ParseSpecialMark(unsigned char Char, unsigned char NextChar, StellarClass::SpecialMarkDigital& SpecialMark, std::size_t& Index)
{
	switch (Char)
	{
	case 'm':
		SpecialMark |= static_cast<std::uint32_t>(StellarClass::SpecialMark::kCode_m);
		++Index;
		return ParseState::kSpectralClass;
	case 'f':
		SpecialMark |= static_cast<std::uint32_t>(StellarClass::SpecialMark::kCode_f);
		++Index;
		return (std::isalpha(NextChar) && std::islower(NextChar)) ? ParseState::kSpecialMark : ParseState::kEnd;
	case 'h':
		SpecialMark |= static_cast<std::uint32_t>(StellarClass::SpecialMark::kCode_h);
		++Index;
		return (std::isalpha(NextChar) && std::islower(NextChar)) ? ParseState::kSpecialMark : ParseState::kEnd;
	case 'p':
		SpecialMark |= static_cast<std::uint32_t>(StellarClass::SpecialMark::kCode_h);
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

std::string StellarClass::SpectralToString(StellarClass::SpectralClass Spectral, float Subclass)
{
	std::ostringstream Stream;

	if (Subclass == std::floor(Subclass))
	{
		Stream << std::fixed << std::setprecision(0) << Subclass;
	}
	else
	{
		Subclass = std::round(Subclass * 10.0f) / 10.0f;
		Stream << std::fixed << std::setprecision(1) << Subclass;
	}

	switch (Spectral)
	{
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
	case StellarClass::SpectralClass::kSpectral_D:
		return std::string("D") + Stream.str();
	case StellarClass::SpectralClass::kSpectral_DA:
		return std::string("DA") + Stream.str();
	case StellarClass::SpectralClass::kSpectral_DB:
		return std::string("DB") + Stream.str();
	case StellarClass::SpectralClass::kSpectral_DC:
		return std::string("DC") + Stream.str();
	case StellarClass::SpectralClass::kSpectral_DO:
		return std::string("DO") + Stream.str();
	case StellarClass::SpectralClass::kSpectral_DQ:
		return std::string("DQ") + Stream.str();
	case StellarClass::SpectralClass::kSpectral_DX:
		return std::string("DX") + Stream.str();
	case StellarClass::SpectralClass::kSpectral_DZ:
		return std::string("DZ") + Stream.str();
	case StellarClass::SpectralClass::kSpectral_Q:
		return std::string("Q");
	case StellarClass::SpectralClass::kSpectral_X:
		return std::string("X");
	default:
		return std::string("Unknown");
	}
}

std::string StellarClass::LuminosityClassToString(StellarClass::LuminosityClass Luminosity)
{
	switch (Luminosity)
	{
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

std::string StellarClass::SpecialMarkToString(StellarClass::SpecialMark SpecialMark)
{
	switch (SpecialMark)
	{
	case StellarClass::SpecialMark::kCode_Null:
		return std::string("");
	case StellarClass::SpecialMark::kCode_f:
		return std::string("f");
	case StellarClass::SpecialMark::kCode_h:
		return std::string("h");
	case StellarClass::SpecialMark::kCode_p:
		return std::string("p");
	default:
		return std::string("");
	}
}

_UTIL_END
_NPGS_END
