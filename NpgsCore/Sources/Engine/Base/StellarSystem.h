#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Engine/Base/NpgsObject/Astro/CelestialObject.h"
#include "Engine/Base/NpgsObject/Astro/Star.h"
#include "Engine/Base/NpgsObject/Astro/Planet.h"
#include "Engine/Base/NpgsObject/NpgsObject.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN

class NPGS_API StellarSystem : public NpgsObject {
public:
    struct BaryCenter : public Astro::AstroObject {
        glm::vec3   Position;           // 位置，使用 3 个 float 分量的向量存储
        glm::vec2   Normal;             // 法向量，(theta, phi)
        std::size_t DistanceRank;       // 距离 (0, 0, 0) 的排名
        std::string Name;               // 质心名字

        BaryCenter() = default;
        BaryCenter(const glm::vec3& Position, const glm::vec2& Normal, std::size_t DistanceRank, const std::string& Name)
            : Position(Position), Normal(Normal), DistanceRank(DistanceRank), Name(Name)
        {}
    };

    struct OrbitalElements {
        Astro::AstroObject* ParentBody; // 上级天体

        float Epoch;                    // 历元，单位儒略日
        float Period;                   // 周期，单位 s
        float SemiMajorAxis;            // 半长轴，单位 AU
        float Eccentricity;             // 离心率
        float Inclination;              // 轨道倾角，单位度
        float LongitudeOfAscendingNode; // 升交点经度，单位度
        float ArgumentOfPeriapsis;      // 近心点幅角，单位度
        float TrueAnomaly;              // 真近点角，单位度

        std::vector<Astro::Star*> Stars;
        std::vector<Astro::Planet*> Planets; // 广义 Planet，实际上行星、卫星都包括这里
        std::vector<Astro::AsteroidCluster*> AsteroidClusers;
    };

public:
    explicit StellarSystem() = default;
    explicit StellarSystem(const BaryCenter& SystemBary);
    StellarSystem(const StellarSystem&) = delete;
    StellarSystem(StellarSystem&&) = default;
    ~StellarSystem() = default;

    StellarSystem& operator=(const StellarSystem&) = delete;
    StellarSystem& operator=(StellarSystem&&) = default;

public:
    StellarSystem& SetBaryPosition(const glm::vec3& Poisition);
    StellarSystem& SetBaryNormal(const glm::vec2& Normal);
    StellarSystem& SetBaryDistanceRank(std::size_t DistanceRank);
    StellarSystem& SetBaryName(const std::string& Name);

    const glm::vec3& GetBaryPosition() const;
    const glm::vec2& GetBaryNormal() const;
    std::size_t GetBaryDistanceRank() const;
    const std::string& GetBaryName() const;

    std::vector<std::unique_ptr<Astro::Star>>& StarData();
    std::vector<std::unique_ptr<Astro::Planet>>& PlanetData();
    std::vector<std::unique_ptr<Astro::AsteroidCluster>>& AsteroidClusterData();
    std::vector<OrbitalElements>& OrbitData();

private:
    BaryCenter _SystemBary;
    std::vector<std::unique_ptr<Astro::Star>> _Stars;
    std::vector<std::unique_ptr<Astro::Planet>> _Planets;
    std::vector<std::unique_ptr<Astro::AsteroidCluster>> _AsteroidClusters;
    std::vector<OrbitalElements> _Orbits;
};

_NPGS_END

#include "StellarSystem.inl"
