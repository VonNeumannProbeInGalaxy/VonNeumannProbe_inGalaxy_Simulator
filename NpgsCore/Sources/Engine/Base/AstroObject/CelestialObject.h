#pragma once

#include <string>
#include <glm/glm.hpp>

#include "Engine/Core/Base.h"

_NPGS_BEGIN
_ASTROOBJECT_BEGIN

class NPGS_API CelestialBody {
public:
    struct BaryCenter {
        std::string Name;                // 质心名字
        glm::vec3   Position;            // 位置，使用 3 个 float 分量的向量存储
        std::size_t DistanceRank;        // 距离排名
    };

    struct OrbitProperties {
        BaryCenter ParentBody;           // 环绕的上级天体
        float Epoch;                     // 历元，单位儒略日
        float Period;                    // 周期，单位 yr
        float SemiMajorAxis;             // 半长轴，单位 AU
        float Eccentricity;              // 离心率
        float Inclination;               // 轨道倾角，单位度
        float LongitudeOfAscendingNode;  // 升交点经度，单位度
        float ArgumentOfPeriapsis;       // 近日点幅角，单位度 
        float MeanAnomaly;               // 平近点角，单位度
    };

    struct BasicProperties {
        double Age;                      // 年龄，单位年
        float  Radius;                   // 半径，单位 km
        float  Spin;                     // 对于普通天体表示自转周期，单位 s；对于黑洞表示无量纲自旋参数
        float  Oblateness;               // 扁率
        float  AxisTilt;                 // 自转轴倾角，单位度
        float  EscapeVelocity;           // 逃逸速度，单位 m/s
        float  MagneticField;            // 磁场强度，单位 T
        std::string Name;                // 名字

        OrbitProperties Orbit{};
    };

public:
    CelestialBody() = default;
    CelestialBody(const BasicProperties& Properties);
    ~CelestialBody() = default;

    CelestialBody& SetBasicProperties(const BasicProperties& Properties);
    CelestialBody& SetOrbitProperties(const OrbitProperties& Properties);
    const BasicProperties& GetBasicProperties() const;
    const OrbitProperties& GetOrbitProperties() const;

    // Setters
    // Setters for BasicProperties
    // ---------------------------
    CelestialBody& SetName(const std::string& Name);
    CelestialBody& SetAge(double Age);
    CelestialBody& SetRadius(float Radius);
    CelestialBody& SetSpin(float Spin);
    CelestialBody& SetOblateness(float Oblateness);
    CelestialBody& SetAxisTilt(float AxisTilt);
    CelestialBody& SetEscapeVelocity(float EscapeVelocity);
    CelestialBody& SetMagneticField(float MagneticField);

    // Setters for OrbitProperties
    // ---------------------------
    CelestialBody& SetParentBody(const BaryCenter& ParentBody);
    CelestialBody& SetEpoch(float Epoch);
    CelestialBody& SetPeriod(float Period);
    CelestialBody& SetSemiMajorAxis(float SemiMajorAxis);
    CelestialBody& SetEccentricity(float Eccentricity);
    CelestialBody& SetInclination(float Inclination);
    CelestialBody& SetLongitudeOfAscendingNode(float LongitudeOfAscendingNode);
    CelestialBody& SetArgumentOfPeriapsis(float ArgumentOfPeriapsis);
    CelestialBody& SetMeanAnomaly(float MeanAnomaly);

    // Getters
    // Getters for BasicProperties
    // ---------------------------
    std::string GetName() const;
    double GetAge() const;
    float GetRadius() const;
    float GetSpin() const;
    float GetOblateness() const;
    float GetAxisTilt() const;
    float GetEscapeVelocity() const;
    float GetMagneticField() const;

    // Getters for OrbitProperties
    // ---------------------------
    BaryCenter GetParentBody() const;
    float GetEpoch() const;
    float GetPeriod() const;
    float GetSemiMajorAxis() const;
    float GetEccentricity() const;
    float GetInclination() const;
    float GetLongitudeOfAscendingNode() const;
    float GetArgumentOfPeriapsis() const;
    float GetMeanAnomaly() const;

private:
    BasicProperties _Properties{};
};

class NPGS_API MolecularCloud {
public:
    struct BasicProperties {
        float Radius;
        float Mass;
        float Temperature;
        float Density;
        float MagneticField;
    };

public:
    MolecularCloud() = default;
    MolecularCloud(const BasicProperties& Properties);
    ~MolecularCloud() = default;

private:
    BasicProperties _Properties{};
};

_ASTROOBJECT_END
_NPGS_END

#include "CelestialObject.inl"
