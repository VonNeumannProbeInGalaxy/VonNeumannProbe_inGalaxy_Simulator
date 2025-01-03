#include "StellarSystem.h"

_NPGS_BEGIN
_ASTRO_BEGIN

FBaryCenter::FBaryCenter(const glm::vec3& Position, const glm::vec2& Normal, std::size_t DistanceRank, const std::string& Name)
	: Position(Position), Normal(Normal), DistanceRank(DistanceRank), Name(Name)
{
}

FOrbit::FOrbitalObject::FOrbitalObject()
	: _Object{}, _Type(EObjectType::kBaryCenter)
{
}

FOrbit::FOrbitalObject::FOrbitalObject(INpgsObject* Object, EObjectType Type)
	: _Type(Type)
{
	switch (Type)
	{
	case EObjectType::kBaryCenter:
		_Object.SystemBary = static_cast<FBaryCenter*>(Object);
		break;
	case EObjectType::kStar:
		_Object.Star = static_cast<AStar*>(Object);
		break;
	case EObjectType::kPlanet:
		_Object.Planet = static_cast<APlanet*>(Object);
		break;
	case EObjectType::kAsteroidCluster:
		_Object.Asteroids = static_cast<AAsteroidCluster*>(Object);
		break;
	case EObjectType::kArtifactCluster:
		_Object.Artifacts = static_cast<Intelli::AArtifact*>(Object);
		break;
	}
}

FOrbit::FOrbitalDetails::FOrbitalDetails()
	: _HostOrbit(nullptr), _Object{}, _InitialTrueAnomaly(0.0f)
{
}

FOrbit::FOrbitalDetails::FOrbitalDetails(INpgsObject* Object, EObjectType Type, FOrbit* HostOrbit, float InitialTrueAnomaly)
	: _Object(FOrbitalObject(Object, Type)), _HostOrbit(HostOrbit), _InitialTrueAnomaly(InitialTrueAnomaly)
{
}

FOrbit::FOrbitalDetails& FOrbit::FOrbitalDetails::SetOrbitalObject(INpgsObject* Object, EObjectType Type)
{
	switch (Type)
	{
	case EObjectType::kBaryCenter:
		_Object.SetObject(static_cast<FBaryCenter*>(Object));
		break;
	case EObjectType::kStar:
		_Object.SetObject(static_cast<AStar*>(Object));
		break;
	case EObjectType::kPlanet:
		_Object.SetObject(static_cast<APlanet*>(Object));
		break;
	case EObjectType::kAsteroidCluster:
		_Object.SetObject(static_cast<AAsteroidCluster*>(Object));
		break;
	case EObjectType::kArtifactCluster:
		_Object.SetObject(static_cast<Intelli::AArtifact*>(Object));
		break;
	default:
		break;
	}

	return *this;
}

FOrbit::FOrbit()
	: _Parent{}, _Normal(), _Period(0.0f)
{
}

FOrbit& FOrbit::SetParent(INpgsObject* Object, EObjectType Type)
{
	switch (Type)
	{
	case EObjectType::kBaryCenter:
		_Parent.SetObject(static_cast<FBaryCenter*>(Object));
		break;
	case EObjectType::kStar:
		_Parent.SetObject(static_cast<AStar*>(Object));
		break;
	case EObjectType::kPlanet:
		_Parent.SetObject(static_cast<APlanet*>(Object));
		break;
	case EObjectType::kAsteroidCluster:
		_Parent.SetObject(static_cast<AAsteroidCluster*>(Object));
		break;
	case EObjectType::kArtifactCluster:
		_Parent.SetObject(static_cast<Intelli::AArtifact*>(Object));
		break;
	default:
		break;
	}

	return *this;
}

FStellarSystem::FStellarSystem(const FBaryCenter& SystemBary)
	: _SystemBary(SystemBary)
{
}

_ASTRO_END
_NPGS_END
