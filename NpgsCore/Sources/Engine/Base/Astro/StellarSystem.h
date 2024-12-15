#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Engine/Base/Astro/CelestialObject.h"
#include "Engine/Base/Astro/Planet.h"
#include "Engine/Base/Astro/Star.h"
#include "Engine/Base/Intelli/Artifact.h"
#include "Engine/Base/NpgsObject.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_ASTRO_BEGIN

struct BaryCenter : public NpgsObject
{
	glm::vec3   Position{};     // 位置，使用 3 个 float 分量的向量存储
	glm::vec2   Normal{};       // 法向量，(theta, phi)
	std::size_t DistanceRank{}; // 距离 (0, 0, 0) 的排名
	std::string Name;           // 质心名字

	BaryCenter() = default;
	BaryCenter(const glm::vec3& Position, const glm::vec2& Normal, std::size_t DistanceRank, const std::string& Name);
};

class Orbit
{
public:
	enum class ObjectType
	{
		kBaryCenter,
		kStar,
		kPlanet,
		kAsteroidCluster,
		kArtifactCluster
	};

	union ObjectPointer
	{
		BaryCenter*        SystemBary;
		Star*              Star;
		Planet*            Planet;
		AsteroidCluster*   Asteroids;
		Intelli::Artifact* Artifacts;

		ObjectPointer() : SystemBary(nullptr) {}
	};

	struct KeplerElements
	{
		float SemiMajorAxis{};            // 半长轴，单位 AU
		float Eccentricity{};             // 离心率
		float Inclination{};              // 轨道倾角，单位 rad
		float LongitudeOfAscendingNode{}; // 升交点经度，单位 rad
		float ArgumentOfPeriapsis{};      // 近心点幅角，单位 rad
		float TrueAnomaly{};              // 真近点角，单位 rad
	};

	class OrbitalObject
	{
	public:
		OrbitalObject() = default;
		OrbitalObject(NpgsObject* Object, ObjectType Type);
		~OrbitalObject() = default;

		OrbitalObject& SetObject(BaryCenter* SystemBary);
		OrbitalObject& SetObject(Star* Star);
		OrbitalObject& SetObject(Planet* Planet);
		OrbitalObject& SetObject(AsteroidCluster* AsteroidCluster);
		OrbitalObject& SetObject(Intelli::Artifact* AsteroidCluster);

		ObjectType GetObjectType() const;

		template <typename T>
		T* GetObject() const;

	private:
		ObjectPointer _Object{};
		ObjectType    _Type{ ObjectType::kBaryCenter };
	};

	class OrbitalDetails
	{
	public:
		OrbitalDetails() = default;
		OrbitalDetails(NpgsObject* Object, ObjectType Type, Orbit* HostOrbit, float InitialTrueAnomaly = 0.0f);
		~OrbitalDetails() = default;

		OrbitalDetails& SetHostOrbit(Orbit* HostOrbit);
		OrbitalDetails& SetOrbitalObject(NpgsObject* Object, ObjectType Type);
		OrbitalDetails& SetInitialTrueAnomaly(float InitialTrueAnomaly);

		Orbit* GetHostOrbit();
		OrbitalObject& GetOrbitalObject();
		float GetInitialTrueAnomaly() const;

		std::vector<Orbit*>& DirectOrbitsData();

	private:
		std::vector<Orbit*> _DirectOrbits;         // 直接下级轨道
		Orbit*              _HostOrbit{};          // 所在轨道
		OrbitalObject       _Object;               // 天体
		float               _InitialTrueAnomaly{}; // 初始真近点角，单位 rad
	};

public:
	Orbit() = default;
	~Orbit() = default;

	Orbit& SetSemiMajorAxis(float SemiMajorAxis);
	Orbit& SetEccentricity(float Eccentricity);
	Orbit& SetInclination(float Inclination);
	Orbit& SetLongitudeOfAscendingNode(float LongitudeOfAscendingNode);
	Orbit& SetArgumentOfPeriapsis(float ArgumentOfPeriapsis);
	Orbit& SetTrueAnomaly(float TrueAnomaly);
	Orbit& SetParent(NpgsObject* Object, ObjectType Type);
	Orbit& SetNormal(const glm::vec2& Normal);
	Orbit& SetPeriod(float Period);

	float GetSemiMajorAxis() const;
	float GetEccentricity() const;
	float GetInclination() const;
	float GetLongitudeOfAscendingNode() const;
	float GetArgumentOfPeriapsis() const;
	float GetTrueAnomaly() const;
	const OrbitalObject& GetParent() const;
	const glm::vec2& GetNormal() const;
	float GetPeriod() const;
	
	std::vector<OrbitalDetails>& ObjectsData();

private:
	KeplerElements              _OrbitElements;
	std::vector<OrbitalDetails> _Objects;  // 轨道上的天体
	OrbitalObject               _Parent;   // 上级天体
	glm::vec2                   _Normal{}; // 轨道法向量 (theta, phi)
	float                       _Period{}; // 轨道周期，单位 s
};

class NPGS_API StellarSystem : public NpgsObject
{
public:
	StellarSystem() = default;
	StellarSystem(const BaryCenter& SystemBary);
	StellarSystem(const StellarSystem&) = delete;
	StellarSystem(StellarSystem&&) noexcept = default;
	~StellarSystem() = default;

	StellarSystem& operator=(const StellarSystem&) = delete;
	StellarSystem& operator=(StellarSystem&&) noexcept = default;

	StellarSystem& SetBaryPosition(const glm::vec3& Poisition);
	StellarSystem& SetBaryNormal(const glm::vec2& Normal);
	StellarSystem& SetBaryDistanceRank(std::size_t DistanceRank);
	StellarSystem& SetBaryName(const std::string& Name);

	const glm::vec3& GetBaryPosition() const;
	const glm::vec2& GetBaryNormal() const;
	std::size_t GetBaryDistanceRank() const;
	const std::string& GetBaryName() const;

	BaryCenter* GetBaryCenter();
	std::vector<std::unique_ptr<Astro::Star>>& StarsData();
	std::vector<std::unique_ptr<Astro::Planet>>& PlanetsData();
	std::vector<std::unique_ptr<Astro::AsteroidCluster>>& AsteroidClustersData();
	std::vector<std::unique_ptr<Orbit>>& OrbitsData();

private:
	BaryCenter                                           _SystemBary;
	std::vector<std::unique_ptr<Astro::Star>>            _Stars;
	std::vector<std::unique_ptr<Astro::Planet>>          _Planets;
	std::vector<std::unique_ptr<Astro::AsteroidCluster>> _AsteroidClusters;
	std::vector<std::unique_ptr<Orbit>>                  _Orbits;
};

_ASTRO_END
_NPGS_END

#include "StellarSystem.inl"
