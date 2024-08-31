#pragma once

#include <string>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

#include "Engine/Base/AstroObject/CelestialObject.h"
#include "Engine/Base/AstroObject/Star.h"
#include "Engine/Base/AstroObject/Planet.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN

class NPGS_API StellarSystem {
public:
    struct BaryCenter {
        glm::vec3   Position;           // 位置，使用 3 个 float 分量的向量存储
        glm::vec2   Normal;             // 法向量，(theta, phi)
        std::size_t DistanceRank;       // 距离 (0, 0, 0) 的排名
        std::string Name;               // 质心名字
    };

    struct OrbitalElements {
        AstroObject::CelestialBody* ParentBody;
        float Epoch;                    // 历元，单位儒略日
        float Period;                   // 周期，单位 s
        float SemiMajorAxis;            // 半长轴，单位 AU
        float Eccentricity;             // 离心率
        float Inclination;              // 轨道倾角，单位度
        float LongitudeOfAscendingNode; // 升交点经度，单位度
        float ArgumentOfPeriapsis;      // 近日点幅角，单位度
        float MeanAnomaly;              // 平近点角，单位度
    };

public:
    StellarSystem() = default;
    StellarSystem(const BaryCenter& SystemBary, const std::vector<AstroObject::Star>& Stars, const std::vector<AstroObject::Planet>& Planets = {});
    ~StellarSystem() = default;

public:
    StellarSystem& SetBaryPosition(const glm::vec3& Poisition);
    StellarSystem& SetBaryNormal(const glm::vec2& Normal);
    StellarSystem& SetBaryDistanceRank(std::size_t DistanceRank);
    StellarSystem& SetBaryName(const std::string& Name);

    const glm::vec3& GetBaryPosition() const;
    const glm::vec2& GetBaryNormal() const;
    std::size_t GetBaryDistanceRank() const;
    const std::string& GetBaryName() const;

    std::vector<AstroObject::Star>& StarData();
    std::vector<AstroObject::Planet>& PlanetData();

private:
    BaryCenter _SystemBary;
    std::vector<AstroObject::Star>   _Stars;
    std::vector<AstroObject::Planet> _Planets;
    std::vector<std::pair<OrbitalElements, AstroObject::CelestialBody*>> _Orbits;
};

_NPGS_END

#include "StellarSystem.inl"
