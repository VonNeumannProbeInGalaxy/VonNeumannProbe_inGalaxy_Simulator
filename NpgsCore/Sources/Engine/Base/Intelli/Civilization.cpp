#include "Civilization.h"

_NPGS_BEGIN
_INTELLI_BEGIN

Standard::Standard(const LifeProperties& LifeProperties, const CivilizationProperties& CivilizationProperties)
	: _LifeProperties(LifeProperties), _CivilizationProperties(CivilizationProperties)
{
}

const float Standard::_kNull                           = 0.0f;
const float Standard::_kCarbonBasedGeneralIntelligence = 1.0f;
const float Standard::_kUrgesellschaft                 = 2.0f;
const float Standard::_kPrevIndustrielle               = 3.0f;
const float Standard::_kSteamAge                       = 4.0f;
const float Standard::_kElectricAge                    = 5.0f;
const float Standard::_kAtomicAge                      = 6.0f;
const float Standard::_kDigitalAge                     = 7.0f;
const float Standard::_kPrevAsiAge                     = 8.0f;

_INTELLI_END
_NPGS_END
