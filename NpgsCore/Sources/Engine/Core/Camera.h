#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// @brief: 相机移动方向。
enum class Movement
{
	kForward, kBack, kLeft, kRight, kUp, kDown
};

// @brief 相机向量类型。
enum class VectorType
{
	kPosition, kFront, kUp, kRight
};

constexpr glm::vec3 kPosition    = glm::vec3(0.0f);
constexpr glm::vec3 kFront       = glm::vec3(0.0f, 0.0f, -1.0f);
constexpr glm::vec3 kWorldUp     = glm::vec3(0.0f, 1.0f, 0.0f);
constexpr GLfloat   kPitch       = 0.0f;
constexpr GLfloat   kYaw         = -90.0f;
constexpr GLfloat   kZoom        = 45.0f;
constexpr GLdouble  kSensitivity = 0.1;
constexpr GLdouble  kSpeed       = 2.5;

class Camera
{
public:
	// @brief: 构造函数。
	// @param: Position 相机位置。
	// @param: WorldUp  世界坐标系的上向量。
	// @param: Pitch    俯仰角。
	// @param: Yaw      偏航角。
	Camera(const glm::vec3& Position = kPosition, const glm::vec3& WorldUp = kWorldUp, GLfloat Pitch = kPitch, GLfloat Yaw = kYaw);
	~Camera() = default;

	// @brief: 获取相机向量。
	// @param: Type 向量类型。
	const glm::vec3& GetCameraVector(VectorType Type) const;

	// @brief: 处理相机输入。
	// @param: Movement  相机移动方向。
	// @param: DeltaTime 时间间隔，使不同帧数下移动速度相同。
	GLvoid ProcessKeyboard(Movement Direction, GLdouble DeltaTime);

	// @brief: 处理鼠标输入。
	// @param: OffsetX 鼠标 X 轴偏移量。
	// @param: OffsetY 鼠标 Y 轴偏移量。
	// @param: ConstrainPitch 是否限制俯仰角。必须为 true。
	GLvoid ProcessMouseMovement(GLdouble OffsetX, GLdouble OffsetY, GLboolean ConstrainPitch = GL_TRUE);

	// @brief: 处理鼠标滚轮输入。
	// @param: OffsetY 鼠标滚轮 Y 轴偏移量。
	GLvoid ProcessMouseScroll(GLdouble OffsetY);

	// @brief: 获取相机缩放。
	// @return: 相机缩放。
	GLfloat GetCameraZoom() const;

	// @brief: 获取相机视图矩阵。
	// @return: 相机视图矩阵。
	glm::mat4x4 GetViewMatrix() const;

private:
	GLvoid UpdateVectors();

private:
	glm::vec3 _Position;
	glm::vec3 _Front;
	glm::vec3 _Up;
	glm::vec3 _Right;
	glm::vec3 _WorldUp;
	GLfloat   _Pitch;
	GLfloat   _Yaw;
	GLdouble  _Sensitivity;
	GLdouble  _Speed;
	GLfloat   _Zoom;
};

#include "Camera.inl"
