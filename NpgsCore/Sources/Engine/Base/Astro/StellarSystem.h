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

struct FBaryCenter : public INpgsObject
{
	glm::vec3   Position{};     // 位置，使用 3 个 float 分量的向量存储
	glm::vec2   Normal{};       // 法向量，(theta, phi)
	std::size_t DistanceRank{}; // 距离 (0, 0, 0) 的排名
	std::string Name;           // 质心名字

	FBaryCenter() = default;
	FBaryCenter(const glm::vec3& Position, const glm::vec2& Normal, std::size_t DistanceRank, const std::string& Name);
};

class FOrbit
{
public:
	enum class EObjectType
	{
		kBaryCenter,
		kStar,
		kPlanet,
		kAsteroidCluster,
		kArtifactCluster
	};

	union FObjectPointer
	{
		FBaryCenter*        SystemBary;
		AStar*              Star;
		APlanet*            Planet;
		AAsteroidCluster*   Asteroids;
		Intelli::AArtifact* Artifacts;

		FObjectPointer() : SystemBary(nullptr) {}
	};

	struct FKeplerElements
	{
		float SemiMajorAxis{};            // 半长轴，单位 AU
		float Eccentricity{};             // 离心率
		float Inclination{};              // 轨道倾角，单位 rad
		float LongitudeOfAscendingNode{}; // 升交点经度，单位 rad
		float ArgumentOfPeriapsis{};      // 近心点幅角，单位 rad
		float TrueAnomaly{};              // 真近点角，单位 rad
	};

	class FOrbitalObject
	{
	public:
		FOrbitalObject();
		FOrbitalObject(INpgsObject* Object, EObjectType Type);
		~FOrbitalObject() = default;

		FOrbitalObject& SetObject(FBaryCenter* SystemBary);
		FOrbitalObject& SetObject(AStar* Star);
		FOrbitalObject& SetObject(APlanet* Planet);
		FOrbitalObject& SetObject(AAsteroidCluster* AsteroidCluster);
		FOrbitalObject& SetObject(Intelli::AArtifact* AsteroidCluster);

		EObjectType GetObjectType() const;

		template <typename ObjectType>
		ObjectType* GetObject() const;

	private:
		FObjectPointer _Object;
		EObjectType    _Type;
	};

	class FOrbitalDetails
	{
	public:
		FOrbitalDetails();
		FOrbitalDetails(INpgsObject* Object, EObjectType Type, FOrbit* HostOrbit, float InitialTrueAnomaly = 0.0f);
		~FOrbitalDetails() = default;

		FOrbitalDetails& SetHostOrbit(FOrbit* HostOrbit);
		FOrbitalDetails& SetOrbitalObject(INpgsObject* Object, EObjectType Type);
		FOrbitalDetails& SetInitialTrueAnomaly(float InitialTrueAnomaly);

		FOrbit* GetHostOrbit();
		FOrbitalObject& GetOrbitalObject();
		float GetInitialTrueAnomaly() const;

		std::vector<FOrbit*>& DirectOrbitsData();

	private:
		std::vector<FOrbit*> _DirectOrbits;       // 直接下级轨道
		FOrbit*              _HostOrbit;          // 所在轨道
		FOrbitalObject       _Object;             // 天体
		float                _InitialTrueAnomaly; // 初始真近点角，单位 rad
	};

public:
	FOrbit();
	~FOrbit() = default;

	FOrbit& SetSemiMajorAxis(float SemiMajorAxis);
	FOrbit& SetEccentricity(float Eccentricity);
	FOrbit& SetInclination(float Inclination);
	FOrbit& SetLongitudeOfAscendingNode(float LongitudeOfAscendingNode);
	FOrbit& SetArgumentOfPeriapsis(float ArgumentOfPeriapsis);
	FOrbit& SetTrueAnomaly(float TrueAnomaly);
	FOrbit& SetParent(INpgsObject* Object, EObjectType Type);
	FOrbit& SetNormal(const glm::vec2& Normal);
	FOrbit& SetPeriod(float Period);

	float GetSemiMajorAxis() const;
	float GetEccentricity() const;
	float GetInclination() const;
	float GetLongitudeOfAscendingNode() const;
	float GetArgumentOfPeriapsis() const;
	float GetTrueAnomaly() const;
	const FOrbitalObject& GetParent() const;
	const glm::vec2& GetNormal() const;
	float GetPeriod() const;

	std::vector<FOrbitalDetails>& ObjectsData();

private:
	FKeplerElements              _OrbitElements;
	std::vector<FOrbitalDetails> _Objects; // 轨道上的天体
	FOrbitalObject               _Parent;  // 上级天体
	glm::vec2                    _Normal;  // 轨道法向量 (theta, phi)
	float                        _Period;  // 轨道周期，单位 s
};

class FStellarSystem : public INpgsObject
{
public:
	FStellarSystem() = default;
	FStellarSystem(const FBaryCenter& SystemBary);
	FStellarSystem(const FStellarSystem&)     = delete;
	FStellarSystem(FStellarSystem&&) noexcept = default;
	~FStellarSystem() = default;

	FStellarSystem& operator=(const FStellarSystem&)     = delete;
	FStellarSystem& operator=(FStellarSystem&&) noexcept = default;

	FStellarSystem& SetBaryPosition(const glm::vec3& Poisition);
	FStellarSystem& SetBaryNormal(const glm::vec2& Normal);
	FStellarSystem& SetBaryDistanceRank(std::size_t DistanceRank);
	FStellarSystem& SetBaryName(const std::string& Name);

	const glm::vec3& GetBaryPosition() const;
	const glm::vec2& GetBaryNormal() const;
	std::size_t GetBaryDistanceRank() const;
	const std::string& GetBaryName() const;

	FBaryCenter* GetBaryCenter();
	std::vector<std::unique_ptr<Astro::AStar>>& StarsData();
	std::vector<std::unique_ptr<Astro::APlanet>>& PlanetsData();
	std::vector<std::unique_ptr<Astro::AAsteroidCluster>>& AsteroidClustersData();
	std::vector<std::unique_ptr<FOrbit>>& OrbitsData();

private:
	FBaryCenter                                           _SystemBary;
	std::vector<std::unique_ptr<Astro::AStar>>            _Stars;
	std::vector<std::unique_ptr<Astro::APlanet>>          _Planets;
	std::vector<std::unique_ptr<Astro::AAsteroidCluster>> _AsteroidClusters;
	std::vector<std::unique_ptr<FOrbit>>                  _Orbits;
};

_ASTRO_END
_NPGS_END

#include "StellarSystem.inl"
