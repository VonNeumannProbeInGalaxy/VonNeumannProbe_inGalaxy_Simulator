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

class StellarSystem : public NpgsObject
{
public:
	struct BaryCenter : public NpgsObject
	{
		glm::vec3   Position{};     // 位置，使用 3 个 float 分量的向量存储
		glm::vec2   Normal{};       // 法向量，(theta, phi)
		std::size_t DistanceRank{}; // 距离 (0, 0, 0) 的排名
		std::string Name;           // 质心名字

		BaryCenter() = default;
		BaryCenter(const glm::vec3& Position, const glm::vec2& Normal, std::size_t DistanceRank, const std::string& Name)
			: Position(Position), Normal(Normal), DistanceRank(DistanceRank), Name(Name)
		{
		}
	};

	struct Orbit
	{
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

		struct OrbitalObject
		{
			Orbit*              HostOrbit{};          // 所在轨道
			std::vector<Orbit*> DirectOrbits;         // 直接下级轨道
			ObjectPointer       Object{};
			ObjectType          Type{ ObjectType::kBaryCenter };
			float               InitialTrueAnomaly{}; // 初始真近点角，单位 rad

			OrbitalObject() = default;
			OrbitalObject(NpgsObject* Object, ObjectType Type, Orbit* HostOrbit, float InitialTrueAnomaly = 0.0f)
				: Type(Type), HostOrbit(HostOrbit), InitialTrueAnomaly(InitialTrueAnomaly)
			{
				switch (Type)
				{
				case ObjectType::kBaryCenter:
					this->Object.SystemBary = static_cast<BaryCenter*>(Object);
					break;
				case ObjectType::kStar:
					this->Object.Star = static_cast<Star*>(Object);
					break;
				case ObjectType::kPlanet:
					this->Object.Planet = static_cast<Planet*>(Object);
					break;
				case ObjectType::kAsteroidCluster:
					this->Object.Asteroids = static_cast<AsteroidCluster*>(Object);
					break;
				case ObjectType::kArtifactCluster:
					this->Object.Artifacts = static_cast<Intelli::Artifact*>(Object);
				}
			}
		};

		std::vector<OrbitalObject> Objects;                               // 轨道上的天体
		ObjectPointer              Parent;                                // 轨道环绕的上级天体
		ObjectType                 ParentType{ ObjectType::kBaryCenter }; // 上级天体类型
		glm::vec2                  Normal{};                              // 轨道法向量 (theta, phi)

		float Period{};                   // 周期，单位 s
		float SemiMajorAxis{};            // 半长轴，单位 AU
		float Eccentricity{};             // 离心率
		float Inclination{};              // 轨道倾角，单位 rad
		float LongitudeOfAscendingNode{}; // 升交点经度，单位 rad
		float ArgumentOfPeriapsis{};      // 近心点幅角，单位 rad
		float TrueAnomaly{};              // 真近点角，单位 rad
	};

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
	std::vector<std::unique_ptr<Astro::Star>>& StarData();
	std::vector<std::unique_ptr<Astro::Planet>>& PlanetData();
	std::vector<std::unique_ptr<Astro::AsteroidCluster>>& AsteroidClusterData();
	std::vector<std::unique_ptr<Orbit>>& OrbitData();

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
