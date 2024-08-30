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
        glm::vec3 Position;              // 位置，使用 3 个 float 分量的向量存储
        std::size_t DistanceRank;        // 距离，但是是排名
    };

    struct OrbitProperties {
        BaryCenter ParentBody;           // 环绕的上级天体
        double Epoch;                    // 历元，单位儒略日
        double Period;                   // 周期，单位 yr
        double SemiMajorAxis;            // 半长轴，单位 AU
        double Eccentricity;             // 离心率
        double Inclination;              // 轨道倾角，单位度
        double LongitudeOfAscendingNode; // 升交点经度，单位度
        double ArgumentOfPeriapsis;      // 近日点幅角，单位度 
        double MeanAnomaly;              // 平近点角，单位度
    };

    struct BasicProperties {
        std::string Name;                // 名字
        double Mass;                     // 质量，单位 kg
        double Radius;                   // 半径，单位 km
        double Spin;                     // 对于普通天体表示自转周期，单位 s；对于黑洞表示无量纲自旋参数
        double Oblateness;               // 扁率
        double AxisTilt;                 // 自转轴倾角，单位度
        double Age;                      // 年龄，单位年
        double EscapeVelocity;           // 逃逸速度，单位 m/s
        double MagneticField;            // 磁场强度，单位 T

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
    CelestialBody& SetRadius(double Radius);
    CelestialBody& SetMass(double Mass);
    CelestialBody& SetSpin(double Spin);
    CelestialBody& SetOblateness(double Oblateness);
    CelestialBody& SetAxisTilt(double AxisTilt);
    CelestialBody& SetAge(double Age);
    CelestialBody& SetEscapeVelocity(double EscapeVelocity);
    CelestialBody& SetMagneticField(double MagneticField);

    // Setters for OrbitProperties
    // ---------------------------
    CelestialBody& SetParentBody(const BaryCenter& ParentBody);
    CelestialBody& SetEpoch(double Epoch);
    CelestialBody& SetPeriod(double Period);
    CelestialBody& SetSemiMajorAxis(double SemiMajorAxis);
    CelestialBody& SetEccentricity(double Eccentricity);
    CelestialBody& SetInclination(double Inclination);
    CelestialBody& SetLongitudeOfAscendingNode(double LongitudeOfAscendingNode);
    CelestialBody& SetArgumentOfPeriapsis(double ArgumentOfPeriapsis);
    CelestialBody& SetMeanAnomaly(double MeanAnomaly);

    // Getters
    // Getters for BasicProperties
    // ---------------------------
    std::string GetName() const;
    double GetRadius() const;
    double GetMass() const;
    double GetSpin() const;
    double GetOblateness() const;
    double GetAxisTilt() const;
    double GetAge() const;
    double GetEscapeVelocity() const;
    double GetMagneticField() const;

    // Getters for OrbitProperties
    // ---------------------------
    BaryCenter GetParentBody() const;
    double GetEpoch() const;
    double GetPeriod() const;
    double GetSemiMajorAxis() const;
    double GetEccentricity() const;
    double GetInclination() const;
    double GetLongitudeOfAscendingNode() const;
    double GetArgumentOfPeriapsis() const;
    double GetMeanAnomaly() const;

private:
    BasicProperties _Properties{};
};

class NPGS_API MolecularCloud {
public:
    struct BasicProperties {
        double Radius;
        double Mass;
        double Temperature;
        double Density;
        double MagneticField;
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
