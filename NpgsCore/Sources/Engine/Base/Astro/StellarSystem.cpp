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

Orbit::OrbitalDetails& Orbit::OrbitalDetails::SetOrbitalObject(NpgsObject* Object, ObjectType Type)
{
	switch (Type)
	{
	case ObjectType::kBaryCenter:
		_Object.SetObject(static_cast<BaryCenter*>(Object));
		break;
	case ObjectType::kStar:
		_Object.SetObject(static_cast<Star*>(Object));
		break;
	case ObjectType::kPlanet:
		_Object.SetObject(static_cast<Planet*>(Object));
		break;
	case ObjectType::kAsteroidCluster:
		_Object.SetObject(static_cast<AsteroidCluster*>(Object));
		break;
	case ObjectType::kArtifactCluster:
		_Object.SetObject(static_cast<Intelli::Artifact*>(Object));
		break;
	default:
		break;
	}

	return *this;
}

Orbit& Orbit::SetParent(NpgsObject* Object, ObjectType Type)
{
	switch (Type)
	{
	case ObjectType::kBaryCenter:
		_Parent.SetObject(static_cast<BaryCenter*>(Object));
		break;
	case ObjectType::kStar:
		_Parent.SetObject(static_cast<Star*>(Object));
		break;
	case ObjectType::kPlanet:
		_Parent.SetObject(static_cast<Planet*>(Object));
		break;
	case ObjectType::kAsteroidCluster:
		_Parent.SetObject(static_cast<AsteroidCluster*>(Object));
		break;
	case ObjectType::kArtifactCluster:
		_Parent.SetObject(static_cast<Intelli::Artifact*>(Object));
		break;
	default:
		break;
	}

	return *this;
}

StellarSystem::StellarSystem(const BaryCenter& SystemBary)
	: _SystemBary(SystemBary)
{
}

_ASTRO_END
_NPGS_END
