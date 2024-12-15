#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Engine/Core/Base.h"

_NPGS_BEGIN

constexpr glm::vec3 kPosition    = glm::vec3(0.0f);
constexpr glm::vec3 kFront       = glm::vec3(0.0f, 0.0f, -1.0f);
constexpr glm::vec3 kWorldUp     = glm::vec3(0.0f, 1.0f,  0.0f);
constexpr float     kSensitivity = 0.05f;
constexpr float     kSpeed       = 2.5f;
constexpr float     kZoom        = 45.0f;

class Camera
{
public:
	// @brief: 相机移动方向。
	enum class Movement
	{
		kForward,
		kBack,
		kLeft,
		kRight,
		kUp,
		kDown,
		kRollLeft,
		kRollRight
	};

	// @brief 相机向量类型。
	enum class VectorType
	{
		kPosition,
		kFront,
		kUp,
		kRight
	};

public:
	Camera(const glm::vec3& Position = kPosition, const glm::vec3& WorldUp = kWorldUp, float Sensitivity = kSensitivity,
		   float Speed = kSpeed, float Zoom = kZoom);

	~Camera() = default;

	const glm::vec3& GetCameraVector(VectorType Type) const;
	void ProcessKeyboard(Movement Direction, double DeltaTime);
	void ProcessMouseMovement(double OffsetX, double OffsetY);
	void ProcessMouseScroll(double OffsetY);
	float GetCameraZoom() const;
	glm::mat4x4 GetViewMatrix() const;
	void SetOrientation(const glm::quat& Orientation);
	const glm::quat& GetOrientation() const;

private:
	void ProcessRotation(float Yaw, float Pitch, float Roll);
	void UpdateVectors();

private:
	glm::quat _Orientation;
	glm::vec3 _Position;
	glm::vec3 _Front;
	glm::vec3 _Up;
	glm::vec3 _Right;
	glm::vec3 _WorldUp;
	float     _Sensitivity;
	float     _Speed;
	float     _Zoom;
	float     _PrevOffsetX;
	float     _PrevOffsetY;
};

_NPGS_END

#include "Camera.inl"
