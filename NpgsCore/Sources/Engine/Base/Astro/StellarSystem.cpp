#include "StellarSystem.h"

_NPGS_BEGIN
_ASTRO_BEGIN

BaryCenter::BaryCenter(const glm::vec3& Position, const glm::vec2& Normal, std::size_t DistanceRank, const std::string& Name)
	: Position(Position), Normal(Normal), DistanceRank(DistanceRank), Name(Name)
{
}

Orbit::OrbitalObject::OrbitalObject(NpgsObject* Object, ObjectType Type)
	: _Type(Type)
{
	switch (Type)
	{
	case ObjectType::kBaryCenter:
		_Object.SystemBary = static_cast<BaryCenter*>(Object);
		break;
	case ObjectType::kStar:
		_Object.Star = static_cast<Star*>(Object);
		break;
	case ObjectType::kPlanet:
		_Object.Planet = static_cast<Planet*>(Object);
		break;
	case ObjectType::kAsteroidCluster:
		_Object.Asteroids = static_cast<AsteroidCluster*>(Object);
		break;
	case ObjectType::kArtifactCluster:
		_Object.Artifacts = static_cast<Intelli::Artifact*>(Object);
		break;
	}
}

Orbit::OrbitalDetails::OrbitalDetails(NpgsObject* Object, ObjectType Type, Orbit* HostOrbit, float InitialTrueAnomaly)
	: _Object{ OrbitalObject(Object, Type) }, _HostOrbit(HostOrbit), _InitialTrueAnomaly(InitialTrueAnomaly)
{
}

StellarSystem::StellarSystem(const BaryCenter& SystemBary)
	: _SystemBary(SystemBary)
{
}

_ASTRO_END
_NPGS_END
