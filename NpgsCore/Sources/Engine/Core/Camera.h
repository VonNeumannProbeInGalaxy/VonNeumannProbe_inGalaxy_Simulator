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
	// @brief: 构造函数。
	// @param: Position 相机位置。
	// @param: WorldUp  世界坐标系的上向量。
	// @param: Pitch    俯仰角。
	// @param: Yaw      偏航角。
	Camera(const glm::vec3& Position = kPosition,
		   const glm::vec3& WorldUp  = kWorldUp,
		   float Sensitivity         = kSensitivity,
		   float Speed               = kSpeed,
		   float Zoom                = kZoom);

	~Camera() = default;

	// @brief: 获取相机向量。
	// @param: Type 向量类型。
	const glm::vec3& GetCameraVector(VectorType Type) const;

	// @brief: 处理相机输入。
	// @param: Movement  相机移动方向。
	// @param: DeltaTime 时间间隔，使不同帧数下移动速度相同。
	void ProcessKeyboard(Movement Direction, double DeltaTime);

	// @brief: 处理鼠标输入。
	// @param: OffsetX 鼠标 X 轴偏移量。
	// @param: OffsetY 鼠标 Y 轴偏移量。
	// @param: ConstrainPitch 是否限制俯仰角。必须为 true。
	void ProcessMouseMovement(double OffsetX, double OffsetY);

	// @brief: 处理鼠标滚轮输入。
	// @param: OffsetY 鼠标滚轮 Y 轴偏移量。
	void ProcessMouseScroll(double OffsetY);

	// @brief: 获取相机缩放。
	// @return: 相机缩放。
	float GetCameraZoom() const;

	// @brief: 获取相机视图矩阵。
	// @return: 相机视图矩阵。
	glm::mat4x4 GetViewMatrix() const;

	// @brief: 设置相机四元数。
	// @param: Orientation 四元数
	void SetOrientation(const glm::quat& Orientation);

	// @brief: 获取相机四元数。
	// @return: 相机四元数。
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
	glm::vec3 _CurrentVelocity = glm::vec3(0.0f);
	float     _Sensitivity;
	float     _Speed;
	float     _Zoom;
	float     _PrevOffsetX;
	float     _PrevOffsetY;
};

_NPGS_END

#include "Camera.inl"
