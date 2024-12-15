#pragma once

#include "StellarSystem.h"
#include <type_traits>

_NPGS_BEGIN
_ASTRO_BEGIN

NPGS_INLINE Orbit::OrbitalObject& Orbit::OrbitalObject::SetObject(BaryCenter* SystemBary)
{
	_Object.SystemBary = SystemBary;
	_Type = ObjectType::kBaryCenter;
	return *this;
}

NPGS_INLINE Orbit::OrbitalObject& Orbit::OrbitalObject::SetObject(Star* Star)
{
	_Object.Star = Star;
	_Type = ObjectType::kStar;
	return *this;
}

NPGS_INLINE Orbit::OrbitalObject& Orbit::OrbitalObject::SetObject(Planet* Planet)
{
	_Object.Planet = Planet;
	_Type = ObjectType::kPlanet;
	return *this;
}

NPGS_INLINE Orbit::OrbitalObject& Orbit::OrbitalObject::SetObject(AsteroidCluster* AsteroidCluster)
{
	_Object.Asteroids = AsteroidCluster;
	_Type = ObjectType::kAsteroidCluster;
	return *this;
}

NPGS_INLINE Orbit::OrbitalObject& Orbit::OrbitalObject::SetObject(Intelli::Artifact* ArtifactCluster)
{
	_Object.Artifacts = ArtifactCluster;
	_Type = ObjectType::kArtifactCluster;
	return *this;
}

NPGS_INLINE Orbit::ObjectType Orbit::OrbitalObject::GetObjectType() const
{
	return _Type;
}

template<typename T>
NPGS_INLINE T* Orbit::OrbitalObject::GetObject() const
{
	if constexpr (std::is_same_v<T, BaryCenter>)
	{
		return _Type == ObjectType::kBaryCenter ? _Object.SystemBary : nullptr;
	}
	else if constexpr (std::is_same_v<T, Star>)
	{
		return _Type == ObjectType::kStar ? _Object.Star : nullptr;
	}
	else if constexpr (std::is_same_v<T, Planet>)
	{
		return _Type == ObjectType::kPlanet ? _Object.Planet : nullptr;
	}
	else if constexpr (std::is_same_v<T, AsteroidCluster>)
	{
		return _Type == ObjectType::kAsteroidCluster ? _Object.Asteroids : nullptr;
	}
	else if constexpr (std::is_same_v<T, Intelli::Artifact>)
	{
		return _Type == ObjectType::kArtifactCluster ? _Object.Artifacts : nullptr;
	}

	return nullptr;
}

NPGS_INLINE Orbit::OrbitalDetails& Orbit::OrbitalDetails::SetHostOrbit(Orbit* HostOrbit)
{
	_HostOrbit = HostOrbit;
	return *this;
}

NPGS_INLINE Orbit::OrbitalDetails& Orbit::OrbitalDetails::SetInitialTrueAnomaly(float InitialTrueAnomaly)
{
	_InitialTrueAnomaly = InitialTrueAnomaly;
	return *this;
}

NPGS_INLINE Orbit* Orbit::OrbitalDetails::GetHostOrbit()
{
	return _HostOrbit;
}

NPGS_INLINE Orbit::OrbitalObject& Orbit::OrbitalDetails::GetOrbitalObject()
{
	return _Object;
}

NPGS_INLINE float Orbit::OrbitalDetails::GetInitialTrueAnomaly() const
{
	return _InitialTrueAnomaly;
}

NPGS_INLINE std::vector<Orbit*>& Orbit::OrbitalDetails::DirectOrbitsData()
{
	return _DirectOrbits;
}

NPGS_INLINE Orbit& Orbit::SetSemiMajorAxis(float SemiMajorAxis)
{
	_OrbitElements.SemiMajorAxis = SemiMajorAxis;
	return *this;
}

NPGS_INLINE Orbit& Orbit::SetEccentricity(float Eccentricity)
{
	_OrbitElements.Eccentricity = Eccentricity;
	return *this;
}

NPGS_INLINE Orbit& Orbit::SetInclination(float Inclination)
{
	_OrbitElements.Inclination = Inclination;
	return *this;
}

NPGS_INLINE Orbit& Orbit::SetLongitudeOfAscendingNode(float LongitudeOfAscendingNode)
{
	_OrbitElements.LongitudeOfAscendingNode = LongitudeOfAscendingNode;
	return *this;
}

NPGS_INLINE Orbit& Orbit::SetArgumentOfPeriapsis(float ArgumentOfPeriapsis)
{
	_OrbitElements.ArgumentOfPeriapsis = ArgumentOfPeriapsis;
	return *this;
}

NPGS_INLINE Orbit& Orbit::SetTrueAnomaly(float TrueAnomaly)
{
	_OrbitElements.TrueAnomaly = TrueAnomaly;
	return *this;
}

NPGS_INLINE Orbit& Orbit::SetNormal(const glm::vec2& Normal)
{
	_Normal = Normal;
	return *this;
}

NPGS_INLINE Orbit& Orbit::SetPeriod(float Period)
{
	_Period = Period;
	return *this;
}

NPGS_INLINE float Orbit::GetSemiMajorAxis() const
{
	return _OrbitElements.SemiMajorAxis;
}

NPGS_INLINE float Orbit::GetEccentricity() const
{
	return _OrbitElements.Eccentricity;
}

NPGS_INLINE float Orbit::GetInclination() const
{
	return _OrbitElements.Inclination;
}

NPGS_INLINE float Orbit::GetLongitudeOfAscendingNode() const
{
	return _OrbitElements.LongitudeOfAscendingNode;
}

NPGS_INLINE float Orbit::GetArgumentOfPeriapsis() const
{
	return _OrbitElements.ArgumentOfPeriapsis;
}

NPGS_INLINE float Orbit::GetTrueAnomaly() const
{
	return _OrbitElements.TrueAnomaly;
}

NPGS_INLINE const Orbit::OrbitalObject& Orbit::GetParent() const
{
	return _Parent;
}

NPGS_INLINE const glm::vec2& Orbit::GetNormal() const
{
	return _Normal;
}

NPGS_INLINE float Orbit::GetPeriod() const
{
	return _Period;
}

NPGS_INLINE std::vector<Orbit::OrbitalDetails>& Orbit::ObjectsData()
{
	return _Objects;
}

NPGS_INLINE StellarSystem& StellarSystem::SetBaryPosition(const glm::vec3& Position)
{
	_SystemBary.Position = Position;
	return *this;
}

NPGS_INLINE StellarSystem& StellarSystem::SetBaryNormal(const glm::vec2& Normal)
{
	_SystemBary.Normal = Normal;
	return *this;
}

NPGS_INLINE StellarSystem& StellarSystem::SetBaryDistanceRank(std::size_t DistanceRank)
{
	_SystemBary.DistanceRank = DistanceRank;
	return *this;
}

NPGS_INLINE StellarSystem& StellarSystem::SetBaryName(const std::string& Name)
{
	_SystemBary.Name = Name;
	return *this;
}

NPGS_INLINE const glm::vec3& StellarSystem::GetBaryPosition() const
{
	return _SystemBary.Position;
}

NPGS_INLINE const glm::vec2& StellarSystem::GetBaryNormal() const
{
	return _SystemBary.Normal;
}

NPGS_INLINE std::size_t StellarSystem::GetBaryDistanceRank() const
{
	return _SystemBary.DistanceRank;
}

NPGS_INLINE const std::string& StellarSystem::GetBaryName() const
{
	return _SystemBary.Name;
}

NPGS_INLINE BaryCenter* StellarSystem::GetBaryCenter()
{
	return &_SystemBary;
}

NPGS_INLINE std::vector<std::unique_ptr<Astro::Star>>& StellarSystem::StarsData()
{
	return _Stars;
}

NPGS_INLINE std::vector<std::unique_ptr<Astro::Planet>>& StellarSystem::PlanetsData()
{
	return _Planets;
}

NPGS_INLINE std::vector<std::unique_ptr<Astro::AsteroidCluster>>& StellarSystem::AsteroidClustersData()
{
	return _AsteroidClusters;
}

NPGS_INLINE std::vector<std::unique_ptr<Orbit>>& StellarSystem::OrbitsData()
{
	return _Orbits;
}

_ASTRO_END
_NPGS_END
