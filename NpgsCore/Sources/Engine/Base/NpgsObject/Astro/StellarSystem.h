#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Engine/Base/NpgsObject/Astro/CelestialObject.h"
#include "Engine/Base/NpgsObject/Astro/Planet.h"
#include "Engine/Base/NpgsObject/Astro/Star.h"
#include "Engine/Base/NpgsObject/NpgsObject.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_ASTRO_BEGIN

class StellarSystem : public NpgsObject {
public:
    struct BaryCenter {
        glm::vec3   Position;     // 位置，使用 3 个 float 分量的向量存储
        glm::vec2   Normal;       // 法向量，(theta, phi)
        std::size_t DistanceRank; // 距离 (0, 0, 0) 的排名
        std::string Name;         // 质心名字

        BaryCenter() = default;
        BaryCenter(const glm::vec3& Position, const glm::vec2& Normal, std::size_t DistanceRank, const std::string& Name)
            : Position(Position), Normal(Normal), DistanceRank(DistanceRank), Name(Name)
        {}
    };

    struct Orbit {
        enum class ObjectType {
            kBaryCenter,
            kStar,
            kPlanet,
            kAsteroidCluster
        };

        union ObjectPointer {
            const BaryCenter* SystemBary;
            const Astro::Star* Star;
            const Astro::Planet* Planet;
            const Astro::AsteroidCluster* AsteroidCluster;

            ObjectPointer() : SystemBary(nullptr) {}
        };

        struct OrbitalObject {
            ObjectPointer Object;
            ObjectType    Type;
            float         InitialTrueAnomaly;
        };

        std::vector<OrbitalObject> Objects; // 轨道上的天体
        ObjectPointer ParentBody;           // 轨道环绕的上级天体
        ObjectType    ParentBodyType;       // 上级天体类型
        glm::vec2     Normal;               // 轨道法向量 (theta, phi)

        // float Epoch{ 0.0f };                    // 历元，单位儒略日
        float Period{ 0.0f };                      // 周期，单位 s
        float SemiMajorAxis{ 0.0f };               // 半长轴，单位 AU
        float Eccentricity{ 0.0f };                // 离心率
        // float Inclination{ 0.0f };              // 轨道倾角，单位度
        // float LongitudeOfAscendingNode{ 0.0f }; // 升交点经度，单位度
        // float ArgumentOfPeriapsis{ 0.0f };      // 近心点幅角，单位度
        // float TrueAnomaly{ 0.0f };              // 真近点角，单位度
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
    std::vector<Orbit>& OrbitData();

private:
    BaryCenter                                           _SystemBary;
    std::vector<std::unique_ptr<Astro::Star>>            _Stars;
    std::vector<std::unique_ptr<Astro::Planet>>          _Planets;
    std::vector<std::unique_ptr<Astro::AsteroidCluster>> _AsteroidClusters;
    std::vector<Orbit>                                   _Orbits;
};

_ASTRO_END
_NPGS_END

#include "StellarSystem.inl"
