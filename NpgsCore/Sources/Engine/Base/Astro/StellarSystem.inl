#pragma once

#include "StellarSystem.h"

_NPGS_BEGIN
_ASTRO_BEGIN

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

NPGS_INLINE StellarSystem::BaryCenter* StellarSystem::GetBaryCenter()
{
	return &_SystemBary;
}

NPGS_INLINE std::vector<std::unique_ptr<Astro::Star>>& StellarSystem::StarData()
{
	return _Stars;
}

NPGS_INLINE std::vector<std::unique_ptr<Astro::Planet>>& StellarSystem::PlanetData()
{
	return _Planets;
}

NPGS_INLINE std::vector<std::unique_ptr<Astro::AsteroidCluster>>& StellarSystem::AsteroidClusterData()
{
	return _AsteroidClusters;
}

NPGS_INLINE std::vector<std::unique_ptr<StellarSystem::Orbit>>& StellarSystem::OrbitData()
{
	return _Orbits;
}

_ASTRO_END
_NPGS_END
