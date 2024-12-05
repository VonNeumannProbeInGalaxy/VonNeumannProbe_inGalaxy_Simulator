#pragma warning(disable : 4715)

#include "Camera.h"
#include "Engine/Core/Assert.h"

_NPGS_BEGIN

Camera::Camera(const glm::vec3& Position, const glm::vec3& WorldUp) :
	_Orientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
	_Position(Position),
	_Front(kFront),
	_WorldUp(WorldUp),
	_Sensitivity(kSensitivity),
	_Speed(kSpeed),
	_Zoom(kZoom)
{
	UpdateVectors();
}

const glm::vec3& Camera::GetCameraVector(VectorType Type) const
{
	switch (Type)
	{
	case VectorType::kPosition:
		return _Position;
	case VectorType::kFront:
		return _Front;
	case VectorType::kUp:
		return _Up;
	case VectorType::kRight:
		return _Right;
	default:
		NpgsAssert(false, "Invalid vector type");
	}
}

void Camera::ProcessKeyboard(Movement Direction, double DeltaTime)
{
	float Velocity = static_cast<float>(_Speed * DeltaTime);

	switch (Direction)
	{
	case Movement::kForward:
		_Position += _Front * Velocity;
		break;
	case Movement::kBack:
		_Position -= _Front * Velocity;
		break;
	case Movement::kLeft:
		_Position -= _Right * Velocity;
		break;
	case Movement::kRight:
		_Position += _Right * Velocity;
		break;
	case Movement::kUp:
		_Position += _Up * Velocity;
		break;
	case Movement::kDown:
		_Position -= _Up * Velocity;
		break;
	case Movement::kRollLeft:
		ProcessRotation(0.0f, 0.0f,  10.0f * Velocity);  // 使用新的函数
		break;
	case Movement::kRollRight:
		ProcessRotation(0.0f, 0.0f, -10.0f * Velocity);  // 使用新的函数
		break;
	}

	UpdateVectors();
}

void Camera::ProcessMouseMovement(double OffsetX, double OffsetY, bool)
{
    float HorizontalAngle = static_cast<float>(_Sensitivity *  OffsetX);
    float VerticalAngle   = static_cast<float>(_Sensitivity * -OffsetY);

    ProcessRotation(HorizontalAngle, VerticalAngle, 0.0f);  // 使用新的函数
}

void Camera::ProcessMouseScroll(double OffsetY)
{
	_Speed += OffsetY * 0.1;

	if (_Speed <= 0)
	{
		_Speed = 0;
	}
}

void Camera::ProcessRotation(float Yaw, float Pitch, float Roll)
{
    // 创建欧拉角旋转的四元数
    glm::quat QuatYaw   = glm::angleAxis(glm::radians(Yaw),   glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat QuatPitch = glm::angleAxis(glm::radians(Pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::quat QuatRoll  = glm::angleAxis(glm::radians(Roll),  glm::vec3(0.0f, 0.0f, 1.0f));
    
    // 组合旋转并更新方向
	_Orientation = glm::normalize(QuatYaw * QuatPitch * QuatRoll * _Orientation);
    
    UpdateVectors();
}

void Camera::UpdateVectors()
{
	_Orientation = glm::normalize(_Orientation);
	glm::quat ConjugateOrient = glm::conjugate(_Orientation);
	
	_Front = glm::normalize(ConjugateOrient * glm::vec3(0.0f, 0.0f, -1.0f));
	_Right = glm::normalize(ConjugateOrient * glm::vec3(1.0f, 0.0f,  0.0f));
	_Up    = glm::normalize(ConjugateOrient * glm::vec3(0.0f, 1.0f,  0.0f));
}

_NPGS_END
