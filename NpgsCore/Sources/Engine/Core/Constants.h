#pragma once

#include <cstdint>
#include "Engine/Core/Base.h"

_NPGS_BEGIN

// Calculation constants
// ---------------------
constexpr inline float  kSolarMass              = 1.9884e30f;
constexpr inline float  kSolarLuminosity        = 3.828e26f;
constexpr inline float  kSolarRadius            = 6.957e8f;
constexpr inline float  kSolarFeH               = 0.0f;
constexpr inline float  kSolarTeff              = 5772.0f;
constexpr inline float  kSolarCoreTemp          = 1.57e7f;
constexpr inline float  kSolarAbsoluteMagnitude = 4.83f;

constexpr inline float  kJupiterMass            = 1.8982e27f;
constexpr inline int    kJupiterRadius          = 69911000;

constexpr inline float  kEarthMass              = 5.972168e24f;
constexpr inline int    kEarthRadius            = 6371000;

constexpr inline float  kSolarMassToEarth       = static_cast<float>(kSolarMass   / kEarthMass);
constexpr inline float  kSolarRadiusToEarth     = static_cast<float>(kSolarRadius / kEarthRadius);
constexpr inline float  kEarthMassToSolar       = static_cast<float>(kEarthMass   / kSolarMass);
constexpr inline float  kEarthRadiusToSolar     = static_cast<float>(kEarthRadius / kSolarRadius);

constexpr inline int    kSpeedOfLight           = 299792458;
constexpr inline float  kGravityConstant        = 6.6743e-11f;
constexpr inline float  kPi                     = 3.1415927f;
constexpr inline float  kStefanBoltzmann        = 5.6703744e-8f;
constexpr inline int    kYearInSeconds          = 31536000;
constexpr inline std::uint64_t kAuToMeter       = 149597870700;
constexpr inline int    kPascalToAtm            = 101325;

_NPGS_END
