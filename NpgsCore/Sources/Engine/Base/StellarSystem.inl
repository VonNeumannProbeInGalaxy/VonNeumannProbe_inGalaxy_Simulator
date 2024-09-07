#pragma once

#include "StellarSystem.h"

_NPGS_BEGIN

inline StellarSystem& StellarSystem::SetBaryPosition(const glm::vec3& Position) {
    _SystemBary.Position = Position;
    return *this;
}

inline StellarSystem& StellarSystem::SetBaryNormal(const glm::vec2& Normal) {
    _SystemBary.Normal = Normal;
    return *this;
}

inline StellarSystem& StellarSystem::SetBaryDistanceRank(std::size_t DistanceRank) {
    _SystemBary.DistanceRank = DistanceRank;
    return *this;
}

inline StellarSystem& StellarSystem::SetBaryName(const std::string& Name) {
    _SystemBary.Name = Name;
    return *this;
}

inline const glm::vec3& StellarSystem::GetBaryPosition() const {
    return _SystemBary.Position;
}

inline const glm::vec2& StellarSystem::GetBaryNormal() const {
    return _SystemBary.Normal;
}

inline std::size_t StellarSystem::GetBaryDistanceRank() const {
    return _SystemBary.DistanceRank;
}

inline const std::string& StellarSystem::GetBaryName() const {
    return _SystemBary.Name;
}

inline std::vector<AstroObject::Star>& StellarSystem::StarData() {
    return _Stars;
}

inline std::vector<AstroObject::Planet>& StellarSystem::PlanetData() {
    return _Planets;
}

inline std::vector<std::pair<StellarSystem::OrbitalElements, std::vector<AstroObject::AstroObject*>>>& StellarSystem::OrbitData() {
    return _Orbits;
}

_NPGS_END
