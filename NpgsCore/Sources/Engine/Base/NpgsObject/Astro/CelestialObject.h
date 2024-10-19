#pragma once

#include <string>
#include <glm/glm.hpp>

#include "Engine/Base/NpgsObject/NpgsObject.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_ASTRO_BEGIN

class NPGS_API AstroObject : public NpgsObject {
public:
    AstroObject() = default;
    virtual ~AstroObject() = default;
};

class NPGS_API CelestialBody : public AstroObject {
public:
    struct BasicProperties {
        glm::vec2 Normal;      // 法向量，球坐标表示，(theta, phi)

        double Age;            // 年龄，单位年
        float  Radius;         // 半径，单位 m
        float  Spin;           // 对于普通天体表示自转周期，单位 s；对于黑洞表示无量纲自旋参数
        float  Oblateness;     // 扁率
        float  EscapeVelocity; // 逃逸速度，单位 m/s
        float  MagneticField;  // 磁场强度，单位 T

        std::string Name;      // 名字
    };

public:
    CelestialBody() = default;
    CelestialBody(const BasicProperties& Properties);
    virtual ~CelestialBody() = default;

    CelestialBody& SetBasicProperties(const BasicProperties& Properties);
    const BasicProperties& GetBasicProperties() const;

    // Setters
    // Setters for BasicProperties
    // ---------------------------
    CelestialBody& SetNormal(const glm::vec2& Normal);
    CelestialBody& SetName(const std::string& Name);
    CelestialBody& SetAge(double Age);
    CelestialBody& SetRadius(float Radius);
    CelestialBody& SetSpin(float Spin);
    CelestialBody& SetOblateness(float Oblateness);
    CelestialBody& SetEscapeVelocity(float EscapeVelocity);
    CelestialBody& SetMagneticField(float MagneticField);

    // Getters
    // Getters for BasicProperties
    // ---------------------------
    const glm::vec2& GetNormal() const;
    const std::string& GetName() const;
    double GetAge() const;
    float  GetRadius() const;
    float  GetSpin() const;
    float  GetOblateness() const;
    float  GetEscapeVelocity() const;
    float  GetMagneticField() const;

private:
    BasicProperties _Properties{};
};

_ASTRO_END
_NPGS_END

#include "CelestialObject.inl"
