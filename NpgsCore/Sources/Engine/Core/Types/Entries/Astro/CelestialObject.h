#pragma once

#include <string>
#include <glm/glm.hpp>
#include "Engine/Base/NpgsObject.h"
#include "Engine/Core/Base/Base.h"

_NPGS_BEGIN
_ASTRO_BEGIN

class IAstroObject : public INpgsObject
{
public:
	IAstroObject() = default;
	virtual ~IAstroObject() = default;
};

class FCelestialBody : public IAstroObject
{
public:
	struct FBasicProperties
	{
		std::string Name;              // 名字
		glm::vec2   Normal{};          // 法向量，球坐标表示，(theta, phi)

		double Age{};                  // 年龄，单位年
		float  Radius{};               // 半径，单位 m
		float  Spin{};                 // 对于普通天体表示自转周期，单位 s；对于黑洞表示无量纲自旋参数
		float  Oblateness{};           // 扁率
		float  EscapeVelocity{};       // 逃逸速度，单位 m/s
		float  MagneticField{};        // 磁场强度，单位 T
	};

public:
	FCelestialBody() = default;
	FCelestialBody(const FBasicProperties& Properties);
	virtual ~FCelestialBody() = default;

	FCelestialBody& SetBasicProperties(const FBasicProperties& Properties);
	const FBasicProperties& GetBasicProperties() const;

	// Setters
	// Setters for BasicProperties
	// ---------------------------
	FCelestialBody& SetNormal(const glm::vec2& Normal);
	FCelestialBody& SetName(const std::string& Name);
	FCelestialBody& SetAge(double Age);
	FCelestialBody& SetRadius(float Radius);
	FCelestialBody& SetSpin(float Spin);
	FCelestialBody& SetOblateness(float Oblateness);
	FCelestialBody& SetEscapeVelocity(float EscapeVelocity);
	FCelestialBody& SetMagneticField(float MagneticField);

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
	FBasicProperties _Properties{};
};

_ASTRO_END
_NPGS_END

#include "CelestialObject.inl"
