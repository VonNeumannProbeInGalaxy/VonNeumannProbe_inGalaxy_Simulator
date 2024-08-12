#pragma once

#include <string>
#include <glm/glm.hpp>
#include "Engine/Core/Base.h"

_NPGS_BEGIN
_ASTROOBJECT_BEGIN

class CelestialBody {
public:
    struct BaryCenter {
        std::string Name;                // ��������
        glm::dvec3 Position;             // λ�ã�ʹ�� 3 �� double �����������洢
        double Distance;                 // ���룬��λ pc
    };

    struct OrbitProperties {
        BaryCenter ParentBody;           // ���Ƶ��ϼ�����
        double Epoch;                    // ��Ԫ����λ������
        double Period;                   // ���ڣ���λ yr
        double SemiMajorAxis;            // �볤�ᣬ��λ AU
        double Eccentricity;             // ������
        double Inclination;              // �����ǣ���λ��
        double LongitudeOfAscendingNode; // �����㾭�ȣ���λ��
        double ArgumentOfPeriapsis;      // ���յ���ǣ���λ�� 
        double MeanAnomaly;              // ƽ����ǣ���λ��
    };

    struct BasicProperties {
        std::string Name;                // ����
        double Mass;                     // ��������λ kg
        double Radius;                   // �뾶����λ km
        double RotationPeriod;           // ��ת���ڣ���λ s
        double Oblateness;               // ����
        double AxisTilt;                 // ��ת����ǣ���λ��
        double Albedo;                   // ������
        double Age;                      // ���䣬��λ��
        double EscapeVelocity;           // �����ٶȣ���λ m/s
        double MagneticField;            // �ų�ǿ�ȣ���λ T

        OrbitProperties Orbit;
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
    CelestialBody& SetRotationPeriod(double RotationPeriod);
    CelestialBody& SetOblateness(double Oblateness);
    CelestialBody& SetAxisTilt(double AxisTilt);
    CelestialBody& SetAlbedo(double Albedo);
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
    double GetRotationPeriod() const;
    double GetOblateness() const;
    double GetAxisTilt() const;
    double GetAlbedo() const;
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
    BasicProperties _Properties;
};

class MolecularCloud {
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
    BasicProperties _Properties;
};

_ASTROOBJECT_END
_NPGS_END

#include "CelestialObject.inl"
