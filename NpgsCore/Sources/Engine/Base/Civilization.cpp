#include "Civilization.h"

_NPGS_BEGIN

Civilization::Civilization(const LifeProperties& LifeProperties, const CivilizationProperties& CivilizationProperties)
	: _LifeProperties(LifeProperties), _CivilizationProperties(CivilizationProperties)
{
}

const float Civilization::_kNull                           = 0.0f;
const float Civilization::_kCarbonBasedGeneralIntelligence = 1.0f;
const float Civilization::_kUrgesellschaft                 = 2.0f;
const float Civilization::_kPrevIndustrielle               = 3.0f;
const float Civilization::_kSteamAge                       = 4.0f;
const float Civilization::_kElectricAge                    = 5.0f;
const float Civilization::_kAtomicAge                      = 6.0f;
const float Civilization::_kDigitalAge                     = 7.0f;
const float Civilization::_kPrevAsiAge                     = 8.0f;

_NPGS_END
